#pragma once

#include "token_index/types.h"
#include "token_index/index_manager_v1.h"
#include "token_index/index_manager_v2.h"
#include "token_index/index_manager_v3.h"

namespace ti
{
    class field_manager
    {
    public:
        field_manager();
        void push_field_dir(const path_t &field_dir_path);
        const result_union_set_t retrieve_field_union(const std::string &field, const query_t &query) const;
        const result_intersection_set_t retrieve_field_intersection(const std::string &field, const query_t &query) const;
    private:
        field_manager(const field_manager &other) = delete;
        field_manager(field_manager &&other) = delete;
        field_manager operator=(const field_manager &other) = delete;
        field_manager operator=(field_manager &&other) = delete;
        std::unordered_map<path_t, index_manager_v3 *> _field_map;
    };
}