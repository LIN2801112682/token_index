#pragma once

#include "token_index/types.h"

namespace ti
{
    class index_manager_v4_0_1
    {
    public:
        index_manager_v4_0_1();
        void push_col_file(const path_t &col_file_path);
        void push_doc_line(const str_t &doc_line);
        void print_inverted_index() const;
        frequency_t calc_frequency(const str_t &token) const;
        result_union_set_t retrieve_union(const query_t &query) const;
        result_intersection_set_t retrieve_intersection(const query_t &query, const str_t &query_line) const;
    private:
        index_manager_v4_0_1(const index_manager_v4_0_1 &other) = delete;
        index_manager_v4_0_1(index_manager_v4_0_1 &&other) = delete;
        index_manager_v4_0_1 operator=(const index_manager_v4_0_1 &other) = delete;
        index_manager_v4_0_1 operator=(index_manager_v4_0_1 &&other) = delete;
        inverted_index_v1_t _inverted_index;
        std::vector<token_relative_position_frequency_t> gen_token_relative_position_frequency_vec(const query_t &query) const;
        result_intersection_set_t low_frequency_retrieve_intersection(std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec) const;
        result_intersection_set_t not_low_frequency_retrieve_intersection(std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec) const;
    };
}