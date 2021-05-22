#pragma once

#include "token_index/types.h"
//#include "levenshtein_automaton/trie_tree.h"

namespace ti
{
    class index_manager_v4_0_2
    {
    public:
        index_manager_v4_0_2();
        void push_col_file(const path_t &col_file_path);
        void push_doc_line(const str_t &doc_line);
        void print_inverted_index() const noexcept;
        frequency_t calc_frequency(const str_t &token) const noexcept;
        result_union_set_t retrieve_union(const query_t &query) const noexcept;
        result_intersection_set_t retrieve_intersection(const query_t &query, const str_t &query_line) const noexcept;
    private:
        index_manager_v4_0_2(const index_manager_v4_0_2 &other) = delete;
        index_manager_v4_0_2(index_manager_v4_0_2 &&other) = delete;
        index_manager_v4_0_2 operator=(const index_manager_v4_0_2 &other) = delete;
        index_manager_v4_0_2 operator=(index_manager_v4_0_2 &&other) = delete;
        // ti_trie_tree _inverted_index;
        doc_line_index_t _doc_line_index;
        std::vector<token_relative_position_frequency_t> gen_token_relative_position_frequency_vec(const query_t &query) const noexcept;
        result_intersection_set_t low_frequency_retrieve_intersection(std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec, const str_t &query_line) const noexcept;
        result_intersection_set_t not_low_frequency_retrieve_intersection(std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec) const noexcept;
    };
}