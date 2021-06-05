#pragma once

#include "token_index/types.h"
#include "token_index/index_manager_v4_0.h"
#include "token_index/index_manager_v4_0_1.h"
#include "token_index/index_manager_v4_0_2.h"
#include "token_index/index_manager_v4_1.h"
#include "token_index/index_manager_v3_0.h"
#include "token_index/index_manager_v3_1.h"

namespace ti
{
    class field_manager
    {
    public:
        using index_manager_t = index_manager_v4_1;
        field_manager();
        ~field_manager();
        void push_field_dir(const path_t &field_dir_path);
        void print_field_inverted_index(const path_t &field) const;
        result_union_set_t retrieve_field_union(const path_t &field, const query_t &query) const;
        result_intersection_set_t retrieve_field_intersection(const path_t &field, const query_t &query, const str_t &query_line) const;
        result_regex_t retrieve_field_regex(const std::string &field, const std::regex &pattern) const;

    private:
        field_manager(const field_manager &other) = delete;
        field_manager(field_manager &&other) = delete;
        field_manager operator=(const field_manager &other) = delete;
        field_manager operator=(field_manager &&other) = delete;
        std::unordered_map<path_t, index_manager_t *> _field_map;
    };
}