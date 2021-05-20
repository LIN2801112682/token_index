#pragma once

#include "token_index/types.h"

namespace ti
{
    class index_manager_v4_0
    {
    public:
        index_manager_v4_0();
        void push_col_file(const path_t &col_file_path);
        void push_doc_line(const str_t &doc_line);
        void print_inverted_index() const noexcept;
        frequency_t calc_frequency(const str_t &token) const noexcept;
        result_union_set_t retrieve_union(const query_t &query) const noexcept;
        result_intersection_set_t retrieve_intersection(const query_t &query, const str_t &) const noexcept;
    private:
        index_manager_v4_0(const index_manager_v4_0 &other) = delete;
        index_manager_v4_0(index_manager_v4_0 &&other) = delete;
        index_manager_v4_0 operator=(const index_manager_v4_0 &other) = delete;
        index_manager_v4_0 operator=(index_manager_v4_0 &&other) = delete;
        inverted_index_v4_0_t _inverted_index;
    };
}