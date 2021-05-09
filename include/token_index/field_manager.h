#pragma once

#include "token_index/types.h"
#include "token_index/index_manager_v1.h"
#include "token_index/index_manager_v2.h"
#include "token_index/index_manager_v3.h"
#include "token_index/index_manager_v3_1.h"

namespace ti
{
    class field_manager
    {
    public:
<<<<<<< HEAD
        using index_manager_t = index_manager_v3_1;
=======
        using index_manager_t = index_manager_v1;
>>>>>>> 053df62318c207f3908262971ade929b1c30b02d

        field_manager();
        ~field_manager();
        void push_field_dir(const path_t &field_dir_path);
        const result_union_set_t retrieve_field_union(const std::string &field, const query_t &query) const;
        const result_intersection_set_t retrieve_field_intersection(const std::string &field, const query_t &query) const;
    private:
        field_manager(const field_manager &other) = delete;
        field_manager(field_manager &&other) = delete;
        field_manager operator=(const field_manager &other) = delete;
        field_manager operator=(field_manager &&other) = delete;
        std::unordered_map<path_t, index_manager_t *> _field_map;
    };
}