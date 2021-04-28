#include "token_index/types.h"
#include "token_index/index_manager.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

/*
void test_save_and_load_inverted_index(const ti::path_t &file_path, const ti::path_t &index_path)
{
    {
        ti::index_manager manager;
        manager.push_file(file_path);
        std::cout << "before save:" << std::endl;
        std::cout << "print collection:" << std::endl;
        manager.print_collection();
        std::cout << "print inverted index:" << std::endl;
        manager.print_inverted_index();
        manager.save_inverted_index(index_path);
    }
    {
        ti::index_manager manager;
        manager.load_inverted_index(index_path);
        std::cout << "after load:" << std::endl;
        std::cout << "print collection:" << std::endl;
        manager.print_collection();
        std::cout << "print inverted index:" << std::endl;
        manager.print_inverted_index();
    }
}

void create_and_save_inverted_index(const ti::path_t &file_path, const ti::path_t &index_path)
{
    ti::index_manager manager;
    manager.push_file(file_path);
    manager.save_inverted_index(index_path);
}
*/

void test_query(const ti::index_manager &manager, const ti::query_vec_t &query_vec,
                     const bool &is_union, const bool &is_out, std::ostream &os = std::cout)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &query : query_vec)
    {
        auto begin_time = std::chrono::high_resolution_clock::now();
        std::size_t set_size{0};
        if (is_union)
        {
            const auto &union_set = manager.retrieve_union(query);
            set_size = union_set.size();
            if (is_out)
            {
                for (const auto &token : query)
                    os << token << ',';
                os << ':';
                os << union_set << std::endl;
            }
        }
        else
        {
            const auto &intersection_set = manager.retrieve_intersection(query);
            set_size = intersection_set.size();
            if (is_out)
            {
                for (const auto &token : query)
                    os << token << ',';
                os << ':';
                os << intersection_set << std::endl;
            }
        }
        /*
        if (is_out)
        {
        }
        */
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time: " << program_times
                  << ", Result doc size: " << set_size << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Location time: " << program_times << std::endl;
    double avg_time;
    if (query_vec.empty())
        avg_time = 0.0;
    else
        avg_time = double(program_times) / double(query_vec.size());
    std::cout << "Average location time: " << avg_time << std::endl;
}

void test_query_group(const ti::path_t &doc_path, const ti::path_t &index_path, const ti::path_t &query_path,
                const ti::path_t &union_result_path, const ti::path_t &intersection_result_path)
{
    ti::index_manager manager{};
    manager.push_col_file(doc_path);
    auto query_vec = ti::load_query_vec(query_path);
    std::ofstream ofs;

    std::cout << "doc_path: " << doc_path << ", union, cout ----------" << std::endl;
    test_query(manager, query_vec, true, false);

    std::cout << "doc_path: " << doc_path << ", intersection, cout ----------" << std::endl;
    test_query(manager, query_vec, false, false);

    std::cout << "doc_path: " << doc_path << ", union, ofs ----------" << std::endl;
    ofs.open(union_result_path, std::ofstream::out);
    test_query(manager, query_vec, true, true, ofs);
    ofs.close();

    std::cout << "doc_path: " << doc_path << ", intersection, ofs ----------" << std::endl;
    ofs.open(intersection_result_path, std::ofstream::out);
    test_query(manager, query_vec, false, true, ofs);
    ofs.close();
}

void test_bm(const ti::path_t &doc_path, const ti::path_t &index_path, const ti::path_t &query_path)
{
    ti::index_manager manager{};
    manager.push_col_file(doc_path);

    std::ifstream ifs;
    ti::line_t line;

    ti::line_vec_t docs{};
    ifs.open(doc_path, std::ifstream::in);
    while (getline(ifs, line))
        docs.push_back(line);
    ifs.close();

    auto query_vec = ti::load_query_vec(query_path);
    
    ti::line_vec_t querys{};
    ifs.open(query_path, std::ifstream::in);
    while (getline(ifs, line))
        querys.push_back(line);
    ifs.close();

    for (std::size_t i = 0; i < query_vec.size(); ++i)
    {
        const auto &query = query_vec[i];
        const auto &query_line = querys[i];
        const auto &doc_id_map = manager.retrieve_intersection(query);
        for (const auto &doc_id_map_pair : doc_id_map)
        {
            const auto &doc_id = doc_id_map_pair.first;
            const auto &doc_line = docs[doc_id];
            const auto &offset_begin_vec = bm::BoyerMoore(doc_line.c_str(), doc_line.size(), query_line.c_str(), query_line.size());
            std::cout << "  query: " << query_line << ", " << std::endl;
            std::cout << "  doc: " << doc_line << ", " << std::endl;
            for (const auto &position_offset : doc_id_map_pair.second)
            {
                const auto &position = position_offset.position;
                std::cout << "      position: " << position << std::endl;
                const auto &offset = position_offset.offset;
                std::cout << "      offset: " << offset.begin << " ~ " << offset.end << std::endl;
            }
            std::cout << std::endl;
            std::cout << "  BM: ";
            for (const auto &offset_begin : offset_begin_vec)
                std::cout << offset_begin << ", ";
            std::cout << std::endl;
        }
    }
}

static const ti::path_t small_doc_path{"../resource/small_doc.txt"};
static const ti::path_t small_query_path{"../resource/small_query.txt"};
static const ti::path_t pattern_doc_path{"../resource/doc.txt"};
static const ti::path_t depattern_doc_path{"../resource/depattern_doc.txt"};
static const ti::path_t query_path{"../resource/query.txt"};
static const ti::path_t index_path{"../resource/index.txt"};
static const ti::path_t union_result_path{"../resource/union_result.txt"};
static const ti::path_t intersection_result_path{"../resource/intersection_result.txt"};

int main()
{
    //test_save_and_load_inverted_index(small_doc_path, index_path);
    test_query_group(small_doc_path, index_path, small_query_path, union_result_path, intersection_result_path);
    //test_query_group(pattern_doc_path, index_path, query_path, union_result_path, intersection_result_path);
    //test_query_group(depattern_doc_path, index_path, query_path, union_result_path, intersection_result_path);
    //test_bm(small_doc_path, index_path, small_query_path);
    //test_bm(depattern_doc_path, index_path, query_path);
    return 0;
}