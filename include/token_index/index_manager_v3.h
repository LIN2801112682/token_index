#pragma once

#include "token_index/types.h"

namespace ti
{
    class index_manager_v3
    {
    public:
        index_manager_v3();
        void push_col_file(const path_t &col_file_path);
        void push_doc_line(const str_t &doc_line);
        void print_inverted_index() const;
        const frequency_t calc_frequency(const str_t &token) const;
        const result_union_set_t retrieve_union(const query_t &query) const;
        const result_intersection_set_t retrieve_intersection(const query_t &query) const;
    private:
        index_manager_v3(const index_manager_v3 &other) = delete;
        index_manager_v3(index_manager_v3 &&other) = delete;
        index_manager_v3 operator=(const index_manager_v3 &other) = delete;
        index_manager_v3 operator=(index_manager_v3 &&other) = delete;
        inverted_index_v3_t _inverted_index;
    };
}