#include "token_index/types.h"
#include "token_index/token_manager.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void test_save_and_load_inverted_index(const token_index::path_t &file_path, const token_index::path_t &index_path)
{
    {
        token_index::index_manager manager;
        manager.push_file(file_path);
        std::cout << "before save:" << std::endl;
        std::cout << "print collection:" << std::endl;
        manager.print_collection();
        std::cout << "print inverted index:" << std::endl;
        manager.print_inverted_index();
        manager.save_inverted_index(index_path);
    }
    {
        token_index::index_manager manager;
        manager.load_inverted_index(index_path);
        std::cout << "after load:" << std::endl;
        std::cout << "print collection:" << std::endl;
        manager.print_collection();
        std::cout << "print inverted index:" << std::endl;
        manager.print_inverted_index();
    }
}

void create_and_save_inverted_index(const token_index::path_t &file_path, const token_index::path_t &index_path)
{
    token_index::index_manager manager;
    manager.push_file(file_path);
    manager.save_inverted_index(index_path);
}

void test_query(const token_index::index_manager &manager, const token_index::query_vec_t &query_vec,
                     const bool &is_union, const bool &is_out, std::ostream &os = std::cout)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &query : query_vec)
    {
        size_t size{0};
        auto begin_time = std::chrono::high_resolution_clock::now();
        if (is_union)
        {
            auto union_set = manager.retrieve_union(query);
            size = union_set.size();
            if (is_out)
            {
                for (const auto &token : query)
                    os << token << ',';
                os << ':';
                for (const auto &index : union_set)
                    os << index << ',';
                os << std::endl;
            }
        }
        else
        {
            auto intersection_set = manager.retrieve_intersection(query);
            size = intersection_set.size();
            if (is_out)
            {
                for (const auto &token : query)
                    os << token << ',';
                os << ':';
                for (const auto &pair : intersection_set)
                {
                    auto &index = pair.first; 
                    auto &offset_vec = pair.second;
                    os << index << '=';
                    for (auto &offset : offset_vec)
                        os << offset << ',';
                    os << ';';
                }
                os << std::endl;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time:" << program_times
                  << ",Result doc size:" << size << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Location time:" << program_times << std::endl;
    double avg_time;
    if (query_vec.empty())
        avg_time = 0.0;
    else
        avg_time = double(program_times) / double(query_vec.size());
    std::cout << "Average location time:" << avg_time << std::endl;
}

void test_query_group(const token_index::path_t &doc_path, const token_index::path_t &index_path, const token_index::path_t &query_path,
                const token_index::path_t &union_result_path, const token_index::path_t &intersection_result_path)
{
    create_and_save_inverted_index(doc_path, index_path);
    token_index::index_manager manager;
    manager.load_inverted_index(index_path);
    auto query_vec = token_index::load_query_vec(query_path);
    std::ofstream ofs;

    std::cout << "doc_path:" << doc_path << ", union, cout ----------" << std::endl;
    test_query(manager, query_vec, true, false);

    std::cout << "doc_path:" << doc_path << ", intersection, cout ----------" << std::endl;
    test_query(manager, query_vec, false, false);

    std::cout << "doc_path:" << doc_path << ", union, ofs ----------" << std::endl;
    ofs.open(union_result_path, std::ofstream::out);
    test_query(manager, query_vec, true, true, ofs);
    ofs.close();

    std::cout << "doc_path:" << doc_path << ", intersection, ofs ----------" << std::endl;
    ofs.open(intersection_result_path, std::ofstream::out);
    test_query(manager, query_vec, false, true, ofs);
    ofs.close();
}

void test_bm(const token_index::path_t &doc_path, const token_index::path_t &index_path, const token_index::path_t &query_path)
{
    token_index::index_manager manager;
    manager.push_file(doc_path);

    std::ifstream ifs;
    token_index::line_t line;

    token_index::line_vec_t documents;
    ifs.open(doc_path, std::ifstream::in);
    while (getline(ifs, line))
        documents.push_back(line);
    ifs.close();

    auto query_vec = token_index::load_query_vec(query_path);
    
    token_index::line_vec_t querys;
    ifs.open(query_path, std::ifstream::in);
    while (getline(ifs, line))
        querys.push_back(line);
    ifs.close();

    for (std::size_t i = 0; i < query_vec.size(); ++i)
    {
        token_index::query_t query = query_vec[i];
        token_index::line_t query_line = querys[i];
        token_index::index_map_t index_map = manager.retrieve_intersection(query);
        for (const auto &pair : index_map)
        {
            const auto &index = pair.first;
            token_index::line_t document_line = documents[index];
            const auto &result = bm::BM(document_line.c_str(), query_line.c_str());
            std::cout << "  query:" << query_line << "," << std::endl;
            std::cout << "  document:" << document_line << "," << std::endl;
            std::cout << "  offset_set:";
            for (const auto &offset : pair.second)
                std::cout << offset << ',';
            std::cout << std::endl;
            std::cout << "  BM:";
            for (const auto &num : result)
                std::cout << num << ',';
            std::cout << std::endl;
        }
    }
}

static const token_index::path_t small_doc_path{"../resource/small_doc.txt"};
static const token_index::path_t small_query_path{"../resource/small_query.txt"};
static const token_index::path_t doc_path{"../resource/doc.txt"};
static const token_index::path_t depattern_doc_path{"../resource/depattern_doc.txt"};
static const token_index::path_t query_path{"../resource/query.txt"};
static const token_index::path_t index_path{"../resource/index.txt"};
static const token_index::path_t union_result_path{"../resource/union_result.txt"};
static const token_index::path_t intersection_result_path{"../resource/intersection_result.txt"};

int main()
{
    //test_save_and_load_inverted_index(small_doc_path, index_path);
    test_query_group(small_doc_path, index_path, small_query_path, union_result_path, intersection_result_path);
    //test_query_group(doc_path, index_path, query_path, union_result_path, intersection_result_path);
    //test_query_group(depattern_doc_path, index_path, query_path, union_result_path, intersection_result_path);
    test_bm(small_doc_path, index_path, small_query_path);
    //test_bm(depattern_doc_path, index_path, query_path);
    return 0;
}