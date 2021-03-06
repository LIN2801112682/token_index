#include "token_index/types.h"
#include "token_index/common.h"
#include "token_index/field_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void test_query(const ti::field_manager &manager, const std::string &field, const std::vector<ti::query_t> &query_vec,
                const std::vector<ti::str_t> &query_line_vec, const bool &is_union, const bool &is_out, std::ostream &os = std::cout)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (std::size_t i{0}; i < query_vec.size(); ++i)
    {
        const auto &query{query_vec[i]};
        const auto &query_line{query_line_vec[i]};
        auto begin_time = std::chrono::high_resolution_clock::now();
        std::size_t set_size{0};
        if (is_union)
        {
            const auto &union_set = manager.retrieve_field_union(field, query);
            set_size = union_set.size();
            if (is_out)
                os << query << " : " << union_set << std::endl;
        }
        else
        {
            const auto &intersection_set = manager.retrieve_field_intersection(field, query, query_line);
            set_size = intersection_set.size();
            if (is_out)
                os << query << " : " << intersection_set << std::endl;
        }
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

void test_query_group(const ti::path_t &field_dir, const std::string &field, const ti::path_t &query_path,
                const ti::path_t &union_result_path, const ti::path_t &intersection_result_path)
{
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    // manager.print_field_inverted_index(field);
    auto [query_vec, query_line_vec] = ti::load_query_vec(query_path);
    std::ofstream ofs;

    std::cout << "field: " << field << ", union, cout ----------" << std::endl;
    test_query(manager, field, query_vec, query_line_vec, true, false);

    std::cout << "field: " << field << ", intersection, cout ----------" << std::endl;
    test_query(manager, field, query_vec, query_line_vec, false, false);

    std::cout << "field: " << field << ", union, ofs ----------" << std::endl;
    ofs.open(union_result_path, std::ofstream::out);
    test_query(manager, field, query_vec, query_line_vec, true, true, ofs);
    ofs.close();

    std::cout << "field: " << field << ", intersection, ofs ----------" << std::endl;
    ofs.open(intersection_result_path, std::ofstream::out);
    test_query(manager, field, query_vec, query_line_vec, false, true, ofs);
    ofs.close();
}

void test_build_insert_index(const ti::path_t &field_dir, const std::string &field, const ti::path_t &col_file_path)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Build time: " << program_times << std::endl;

    ti::doc_id_t doc_id{999999};
    ti::str_t new_line{"neu"};
    begin_time = std::chrono::high_resolution_clock::now();
    manager.push_col_file(field, col_file_path);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Push time: " << program_times << std::endl;
    //manager.print_field_inverted_index(field);
}

void test_build_delete_index(const ti::path_t &field_dir, const std::string &field, const ti::path_t &query_path)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Build time: " << program_times << std::endl;

    auto [query_vec, query_line_vec] = ti::load_query_vec(query_path);

    begin_time = std::chrono::high_resolution_clock::now();
    for (std::size_t i{0}; i < query_vec.size(); ++i)
    {
        const auto &query{query_vec[i]};
        const auto &query_line{query_line_vec[i]};
        auto begin_time = std::chrono::high_resolution_clock::now();
        const auto &union_set = manager.retrieve_field_union(field, query);
        auto set_size = union_set.size();
        auto del_count{0u};
        for (const auto&doc_id : union_set)
        {
            if (manager.del_doc_by_id(field, doc_id))
            {
                ++del_count;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time: " << program_times
                  << ", Find doc size: " << set_size
                  << ", Delete doc count: " << del_count << std::endl;
    }
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Location time: " << program_times << std::endl;
    double avg_time;
    if (query_vec.empty())
        avg_time = 0.0;
    else
        avg_time = double(program_times) / double(query_vec.size());
    std::cout << "Average location time: " << avg_time << std::endl;
    manager.print_field_inverted_index(field);
}

void test_build_update_index(const ti::path_t &field_dir, const std::string &field, const ti::path_t &query_path)
{
    ti::str_t new_line{"select"};

    auto begin_time = std::chrono::high_resolution_clock::now();
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Build time: " << program_times << std::endl;

    auto [query_vec, query_line_vec] = ti::load_query_vec(query_path);

    begin_time = std::chrono::high_resolution_clock::now();
    for (std::size_t i{0}; i < query_vec.size(); ++i)
    {
        const auto &query{query_vec[i]};
        const auto &query_line{query_line_vec[i]};
        auto begin_time = std::chrono::high_resolution_clock::now();
        const auto &union_set = manager.retrieve_field_union(field, query);
        auto set_size = union_set.size();
        auto update_count{0u};
        for (const auto&doc_id : union_set)
        {
            if (manager.del_doc_by_id(field, doc_id))
            {
                manager.push_doc_line_by_id(field, doc_id, new_line);
                ++update_count;
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time: " << program_times
                  << ", Find doc size: " << set_size
                  << ", Update doc count: " << update_count << std::endl;
    }
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Location time: " << program_times << std::endl;
    double avg_time;
    if (query_vec.empty())
        avg_time = 0.0;
    else
        avg_time = double(program_times) / double(query_vec.size());
    std::cout << "Average location time: " << avg_time << std::endl;
    manager.print_field_inverted_index(field);
}

void test_frequency(const ti::path_t &col_file_path, const ti::path_t &query_path)
{
    ti::index_manager_v4_0_1 manager{};
    manager.push_col_file(col_file_path);

    auto [query_vec, query_line_vec] = ti::load_query_vec(query_path);

    for (std::size_t i{0}; i < query_vec.size(); ++i)
    {
        const auto &query{query_vec[i]};
        const auto &query_line{query_line_vec[i]};

        std::cout << query_line << '\n';
        for (const auto &token : query)
        {
            std::cout << "  " << token << ':' << manager.calc_frequency(token) << '\n';
        }
    }
}


static const ti::path_t field{"query.txt"};
static const ti::path_t field2{"query2.txt"};
static const ti::path_t field_dir{"../resources/field_dir"};
static const ti::path_t query_path{"../resources/query.txt"};
static const ti::path_t query2_path{"../resources/query2.txt"};
static const ti::path_t query3_path{"../resources/query3.txt"};
static const ti::path_t union_result_path{"../resources/union_result.txt"};
static const ti::path_t intersection_result_path{"../resources/intersection_result.txt"};
static const ti::path_t col_file_path{"../resources/col_file.txt"};
static const ti::path_t col_file2_path{"../resources/field_dir/query.txt"};

int main()
{
    //test_query_group(field_dir, field, query_path, union_result_path, intersection_result_path);
    //test_build_insert_index(field_dir, field2, col_file_path);
    //test_build_delete_index(field_dir, field2, query2_path);
    //test_build_update_index(field_dir, field2, query2_path);
    test_frequency(col_file2_path, query3_path);
    return 0;
}