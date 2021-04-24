#pragma once

#include "token_index/header.h"

namespace token_index
{
    static const std::regex KEY_VALUE_REGEX{"(.*):(.*)"};
    static constexpr char KEY_VALUE_DLM{':'};
    static constexpr char SET_DLM{','};

    class index_manager
    {
    public:
        index_manager();
        void push_file(const path_t &path);
        void push_line(const line_t &line);
        void push_document(const document_t &document);                                         //向索引中加入一个文档
        static document_t line_to_document(const line_t &line);
        void print_collection();
        void print_inverted_index();
        void print_token_frequency();
        void save_inverted_index(const path_t &path);
        void load_inverted_index(const path_t &path);
        void inverted_index_build_collection(); 
        static query_vec_t load_query(const path_t &path);
        index_set_t retrieve(const token_t &token) const;
        index_set_t retrieve_union(const query_t &query) const;
        index_set_t retrieve_intersection(const query_t &query) const;
    private:
        index_manager(const index_manager &other) = delete;
        index_manager(index_manager &&other) = delete;
        index_manager operator=(const index_manager &other) = delete;
        index_manager operator=(index_manager &&other) = delete;
        collection_t _collection;
        inverted_index_t _inverted_index;
    };
}