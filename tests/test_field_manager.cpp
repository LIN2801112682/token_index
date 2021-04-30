#include "token_index/types.h"
#include "token_index/common.h"
#include "token_index/index_manager_v1.h"
#include "token_index/index_manager_v2.h"
#include "token_index/index_manager_v3.h"
#include "token_index/field_manager.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void test_query(const ti::field_manager &manager, const std::string &field, const std::vector<ti::query_t> &query_vec,
                     const bool &is_union, const bool &is_out, std::ostream &os = std::cout)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &query : query_vec)
    {
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
            const auto &intersection_set = manager.retrieve_field_intersection(field, query);
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
    auto query_vec = ti::load_query_vec(query_path);
    std::ofstream ofs;

    std::cout << "field: " << field << ", union, cout ----------" << std::endl;
    test_query(manager, field, query_vec, true, false);

    std::cout << "field: " << field << ", intersection, cout ----------" << std::endl;
    test_query(manager, field, query_vec, false, false);

    std::cout << "field: " << field << ", union, ofs ----------" << std::endl;
    ofs.open(union_result_path, std::ofstream::out);
    test_query(manager, field, query_vec, true, true, ofs);
    ofs.close();

    std::cout << "field: " << field << ", intersection, ofs ----------" << std::endl;
    ofs.open(intersection_result_path, std::ofstream::out);
    test_query(manager, field, query_vec, false, true, ofs);
    ofs.close();
}

void test_bm(const ti::path_t &field_dir, const std::string &field, const ti::path_t &query_path)
{
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);

    std::ifstream ifs;
    ti::line_t line;

    std::vector<ti::line_t> docs{};
    ifs.open(field_dir, std::ifstream::in);
    while (getline(ifs, line))
        docs.push_back(line);
    ifs.close();

    auto query_vec = ti::load_query_vec(query_path);
    
    std::vector<ti::line_t> querys{};
    ifs.open(query_path, std::ifstream::in);
    while (getline(ifs, line))
        querys.push_back(line);
    ifs.close();

    for (std::size_t i = 0; i < query_vec.size(); ++i)
    {
        const auto &query = query_vec[i];
        const auto &query_line = querys[i];
        const auto &intersection_set = manager.retrieve_field_intersection(field, query);
        for (const auto &doc_id_position_offset : intersection_set)
        {
            const auto &doc_id = doc_id_position_offset.doc_id;
            const auto &doc_line = docs[doc_id];
            const auto &offset_begin_vec = bm::BoyerMoore(doc_line, query_line);
            std::cout << "  doc_line: " << doc_line << ", " << std::endl;
            std::cout << "  query_line: " << query_line << ", " << std::endl;
            std::cout << "  BM: ";
            for (const auto &offset_begin : offset_begin_vec)
                std::cout << offset_begin << ", ";
            std::cout << std::endl;
            std::cout << "  intersection_set: " << intersection_set << std::endl;;
        }
        std::cout << "----" << std::endl;
    }
}

static const ti::path_t field_dir{"../resource/field_dir"};
static const ti::path_t query_path{"../resource/query.txt"};
static const ti::path_t union_result_path{"../resource/union_result.txt"};
static const ti::path_t intersection_result_path{"../resource/intersection_result.txt"};

int main()
{
    std::string field{"db.txt"};
    test_query_group(field_dir, field, query_path, union_result_path, intersection_result_path);
    //test_bm(field_dir, field, query_path);
    return 0;
}