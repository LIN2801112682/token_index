#include "token_index/header.h"
#include "token_index/token_manager.h"
#include <chrono>

void test_save_and_load_inverted_index(const token_index::path_t &file_path, const token_index::path_t &index_path)
{
    token_index::index_manager manager;
    manager.push_file(file_path);
    std::cout << "before save:" << std::endl;
    manager.print_collection();
    manager.print_inverted_index();
    manager.save_inverted_index(index_path);
    manager.load_inverted_index(index_path);
    std::cout << "after load:" << std::endl;
    manager.print_collection();
    manager.print_inverted_index();
}

void create_and_save_inverted_index(const token_index::path_t &file_path, const token_index::path_t &index_path)
{
    token_index::index_manager manager;
    manager.push_file(file_path);
    manager.save_inverted_index(index_path);
}

void test_out_result(const token_index::index_manager &manager, const token_index::query_vec_t &query_vec,
                     const bool &is_union, std::ostream &os)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &query : query_vec)
    {
        auto begin_time = std::chrono::high_resolution_clock::now();
        token_index::index_set_t index_set;
        if (is_union)
            index_set = manager.retrieve_union(query);
        else
            index_set = manager.retrieve_intersection(query);
        for (const token_index::token_t &token : query)
            os << token << token_index::SET_DLM;
        os << ':';
        for (const token_index::index_t &index : index_set)
            os << index << token_index::SET_DLM;
        os << std::endl;
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time:" << program_times
                  << ",Result doc size:" << index_set.size() << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Location time:" << program_times << std::endl;
    std::cout << "Average location time:" << double(program_times) / double(query_vec.size()) << std::endl;
}

static const token_index::path_t query_path{"../resource/query.txt"};

static const token_index::path_t file_path{"../resource/doc.txt"};
static const token_index::path_t index_path{"../resource/index.txt"};
static const token_index::path_t union_result_path{"../resource/union_result.txt"};
static const token_index::path_t intersection_result_path{"../resource/intersection_result.txt"};

static const token_index::path_t depattern_file_path{"../resource/depattern_doc.txt"};
static const token_index::path_t depattern_index_path{"../resource/depattern_index.txt"};
static const token_index::path_t depattern_union_result_path{"../resource/depattern_union_result.txt"};
static const token_index::path_t depattern_intersection_result_path{"../resource/depattern_intersection_result.txt"};

int main()
{
    //test_save_and_load_inverted_index(file_path, index_path);

    token_index::query_vec_t query_vec = token_index::index_manager::load_query(query_path);
    std::ofstream ofs;

    {
        create_and_save_inverted_index(file_path, index_path);
        token_index::index_manager manager;
        manager.load_inverted_index(index_path);
        std::cout << "file, union, cout ----------" << std::endl;
        test_out_result(manager, query_vec, true, std::cout);
        std::cout << "file, intersection, cout ----------" << std::endl;
        test_out_result(manager, query_vec, false, std::cout);
        std::cout << "file, union, ofs ----------" << std::endl;
        ofs.open(union_result_path, std::ofstream::out);
        test_out_result(manager, query_vec, true, ofs);
        ofs.close();
        std::cout << "file, intersection, ofs ----------" << std::endl;
        ofs.open(intersection_result_path, std::ofstream::out);
        test_out_result(manager, query_vec, false, ofs);
        ofs.close();
        std::cout << "file, frequency ----------" << std::endl;
        manager.print_token_frequency();
    }

    {
        create_and_save_inverted_index(depattern_file_path, depattern_index_path);
        token_index::index_manager manager;
        manager.load_inverted_index(depattern_index_path);
        std::cout << "depattern, union, cout ----------" << std::endl;
        test_out_result(manager, query_vec, true, std::cout);
        std::cout << "depattern, intersection, cout ----------" << std::endl;
        test_out_result(manager, query_vec, false, std::cout);
        std::cout << "depattern, union, ofs ----------" << std::endl;
        ofs.open(depattern_union_result_path, std::ofstream::out);
        test_out_result(manager, query_vec, true, ofs);
        ofs.close();
        std::cout << "depattern, intersection, ofs ----------" << std::endl;
        ofs.open(depattern_intersection_result_path, std::ofstream::out);
        test_out_result(manager, query_vec, false, ofs);
        ofs.close();
        std::cout << "depattern, frequency ----------" << std::endl;
        manager.print_token_frequency();
    }

    return 0;
}