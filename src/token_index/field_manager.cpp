#include "token_index/field_manager.h"
#include <dirent.h>

namespace ti
{
    field_manager::field_manager()
        : _field_map{} {}

    void
    field_manager::push_field_dir(const path_t &field_dir_path)
    {
       struct dirent* p_dirent;
       DIR* p_dir = opendir(field_dir_path.c_str());
       if (p_dir != nullptr)
       {
              while ((p_dirent = readdir(p_dir)) != nullptr)
              {
                    path_t col_file_name = p_dirent->d_name;
                    std::cout << col_file_name << std::endl;
                    if (col_file_name == "." || col_file_name == "..")
                        continue;
                    std::cout << col_file_name << std::endl;
                    path_t col_file_path = field_dir_path + "/" + col_file_name;
                    index_manager_v1* p_index_manager = new index_manager_v1();
                    p_index_manager->push_col_file(col_file_path);
                    _field_map[col_file_name] = p_index_manager;
              }
       }
    }

    const result_union_set_t 
    field_manager::retrieve_field_union(const std::string &field, const query_t &query) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return {};
        auto p_index_manager = iter->second; 
        return p_index_manager->retrieve_union(query);
    }
    
    const result_intersection_set_t 
    field_manager::retrieve_field_intersection(const std::string &field, const query_t &query) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return {};
        auto p_index_manager = iter->second; 
        return p_index_manager->retrieve_intersection(query);
    }
}