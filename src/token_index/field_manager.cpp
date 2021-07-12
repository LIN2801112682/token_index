#include "token_index/field_manager.h"
#include <dirent.h>

namespace ti
{
    field_manager::field_manager()
        : _field_map{} {}

    field_manager::~field_manager()
    {
        for (auto pair : _field_map)
            delete pair.second;
    }

    void
    field_manager::push_field_dir(const path_t &field_dir_path)
    {
        struct dirent *p_dirent;
        DIR *p_dir = opendir(field_dir_path.c_str());
        if (p_dir != nullptr)
        {
            while ((p_dirent = readdir(p_dir)) != nullptr)
            {
                path_t col_file_name = p_dirent->d_name;
                if (col_file_name == "." || col_file_name == "..")
                    continue;
                path_t col_file_path = field_dir_path + "/" + col_file_name;
                auto p_index_manager = new index_manager_t{};
                p_index_manager->push_col_file(col_file_path);
                _field_map[col_file_name] = p_index_manager;
            }
        }
    }

    void
    field_manager::push_col_file(const std::string &field, const path_t col_file_path)
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return;
        auto p_index_manager = iter->second;
        p_index_manager->push_col_file(col_file_path);
    }

    void 
    field_manager::push_doc_line(const std::string &field, const std::string &doc_line)
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return;
        auto p_index_manager = iter->second; 
        p_index_manager->push_doc_line(doc_line);
    }

    void
    field_manager::push_doc_line_by_id(const std::string &field, const doc_id_t &doc_id, const str_t &new_line)
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return;
        auto p_index_manager = iter->second;
        p_index_manager->push_doc_line_by_id(doc_id, new_line);
    }

    bool
    field_manager::del_doc_by_id(const std::string &field, const doc_id_t &doc_id)
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return false;
        auto p_index_manager = iter->second; 
        return p_index_manager->del_doc_by_id(doc_id);
    }

    void
    field_manager::print_field_inverted_index(const std::string &field) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return;
        auto p_index_manager = iter->second; 
        p_index_manager->print_inverted_index();
    }

    result_union_set_t 
    field_manager::retrieve_field_union(const std::string &field, const query_t &query) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return {};
        auto p_index_manager = iter->second; 
        return p_index_manager->retrieve_union(query);
    }
    
    result_intersection_set_t 
    field_manager::retrieve_field_intersection(const std::string &field, const query_t &query, const str_t &query_line) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return {};
        auto p_index_manager = iter->second; 
        return p_index_manager->retrieve_intersection(query, query_line);
    }

    result_regex_t 
    field_manager::retrieve_field_regex(const std::string &field, const std::regex &pattern) const
    {
        auto iter = _field_map.find(field);
        if (std::end(_field_map) == iter)
            return {};
        auto p_index_manager = iter->second; 
        return p_index_manager->retrieve_regex(pattern);
    }
}