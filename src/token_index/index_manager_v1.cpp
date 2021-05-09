#include "token_index/index_manager_v1.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace ti
{
    index_manager_v1::index_manager_v1()
        : _inverted_index{} {}

    void
    index_manager_v1::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        str_t doc_line;
        while (getline(ifs, doc_line))
        {
            std::transform(std::begin(doc_line), std::end(doc_line), std::begin(doc_line), tolower);
            push_doc_line(doc_line);
        }
        ifs.close();
    }

    void
    index_manager_v1::push_doc_line(const str_t &doc_line)
    {
        str_t token;
        std::istringstream iss{doc_line};
        iss >> token;
        doc_id_t doc_id{stoul(token)};
        str_t new_line{doc_line.substr(token.size() + 1)};

        position_t position{0};
        str_idx_t begin, end;
        bool is_find_begin{false};
        for (str_idx_t i{0}; i < new_line.size(); ++i)
        {
            const ch_t &ch = new_line[i]; 
            if (ch != ' ')
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            else
            {
                if (is_find_begin)
                {
                    is_find_begin = false;
                    token = new_line.substr(begin, end - begin + 1);

                    if (_inverted_index.count(token) == 0)
                        _inverted_index.emplace(token, doc_id_map_t{});
                    auto &doc_id_map = _inverted_index[token];

                    if (doc_id_map.count(doc_id) == 0)
                        doc_id_map.emplace(doc_id, position_offset_vec_t{});
                    auto &position_offset_vec = doc_id_map[doc_id];

                    position_offset_vec.emplace_back(
                        position_offset_t{
                            position++,
                            offset_t{
                                begin,
                                end,
                            }});
                }
            }
        }
    }

    void
    index_manager_v1::print_inverted_index() const
    {
        std::cout << _inverted_index;
    }

    const frequency_t
    index_manager_v1::calc_frequency(const str_t &token) const
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
            return 0;

        frequency_t frequency{0};
        for (const auto &doc_id_map_pair : inverted_index_iter->second)
            frequency += doc_id_map_pair.second.size();
        return frequency;
    }

    const result_union_set_t
    index_manager_v1::retrieve_union(const query_t &query) const
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                continue;

            for (const auto &doc_id_map_pair : inverted_index_iter->second)
                union_set.insert(doc_id_map_pair.first);
        }
        return union_set;
    }

    const result_intersection_set_t
    index_manager_v1::retrieve_intersection(const query_t &query) const
    {
        const auto &first_token = query[0];
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        auto intersection_doc_id_map{intersection_inverted_index_iter->second};

        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &inverted_index_iter= _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {}; 
            const auto &doc_id_map{inverted_index_iter->second};
            decltype(intersection_doc_id_map) temp_doc_id_map{};

            for (const auto &intersection_doc_id_map_pair : intersection_doc_id_map)
            {
                const auto &doc_id = intersection_doc_id_map_pair.first;
                const auto &doc_id_map_iter = doc_id_map.find(doc_id);
                if (std::end(doc_id_map) == doc_id_map_iter)
                    continue;
                const auto &position_offset_vec{doc_id_map_iter->second};

                for (const auto &intersection_position_offset : intersection_doc_id_map_pair.second)
                {
                    const auto &position = intersection_position_offset.position;
                    for (const auto &position_offset : position_offset_vec)
                    {
                        if (position + i != position_offset.position)
                            continue;
                        if (temp_doc_id_map.count(doc_id) == 0)
                            temp_doc_id_map.emplace(doc_id, position_offset_vec_t{});
                        temp_doc_id_map[doc_id].emplace_back(
                            position_offset_t{
                                position,
                                offset_t{
                                    intersection_position_offset.offset.begin,
                                    position_offset.offset.end,
                                }});
                    }
                }
            }
            if (temp_doc_id_map.empty())
                return {};
            intersection_doc_id_map = temp_doc_id_map;
        }
        return to_result_intersection_set_t(intersection_doc_id_map);
    }
}