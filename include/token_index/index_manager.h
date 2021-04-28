#pragma once

#include "token_index/types.h"

namespace ti
{
    class index_manager
    {
    public:
        index_manager();
        void push_col_file(const path_t &col_file_path);
        void push_doc_line(const line_t &doc_line);
        void print_col() const;
        void print_inverted_index() const;
        //void print_token_frequency();
        //void save_inverted_index(const path_t &path);
        //void load_inverted_index(const path_t &path);
        //void inverted_index_build_collection(); 
        frequency_t calc_frequency(const token_t &token) const;
        const doc_id_map_t retrieve(const token_t &token) const;
        const doc_id_set_t retrieve_union(const query_t &query) const;
        const doc_id_map_t retrieve_intersection(const query_t &query) const;
    private:
        index_manager(const index_manager &other) = delete;
        index_manager(index_manager &&other) = delete;
        index_manager operator=(const index_manager &other) = delete;
        index_manager operator=(index_manager &&other) = delete;
        col_t _col;
        inverted_index_t _inverted_index;
    };
}