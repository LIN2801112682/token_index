#include "token_index/index_manager_v1.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <tuple>

namespace ti
{
    index_manager_v1::index_manager_v1()
        : _inverted_index{} {}

    void
    index_manager_v1::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        line_t doc_line;
        while (getline(ifs, doc_line))
        {
            std::transform(std::begin(doc_line), std::end(doc_line), std::begin(doc_line), tolower);
            push_doc_line(doc_line);
        }
        ifs.close();
    }

    void
    index_manager_v1::push_doc_line(const line_t &doc_line)
    {
        auto [doc_id, doc, new_doc_line] = line_to_doc_id_and_doc(doc_line);

        std::map<token_t, std::vector<position_t>> token_position_vec_map{};
        for (position_t position{0}; position < doc.size(); ++position)
        {
            const auto &token = doc[position];
            if (token_position_vec_map.count(token) == 0)
                token_position_vec_map.emplace(token, std::vector<position_t>{});
            token_position_vec_map[token].emplace_back(position);
        }

        for (const auto &token_position_vec_pair : token_position_vec_map)
        {
            const auto &token = token_position_vec_pair.first;
            if (_inverted_index.count(token) == 0)
                _inverted_index.emplace(token, doc_id_map_t{});
            if (_inverted_index[token].count(doc_id) == 0)
                _inverted_index[token].emplace(doc_id, position_offset_vec_t{});

            const auto &offset_begin_vec = bm::BoyerMoore(new_doc_line, token);
            const auto &token_size = token.size();
            for (std::size_t i{0}; i < token_position_vec_pair.second.size(); ++i)
            {
                _inverted_index[token][doc_id].emplace_back(
                    position_offset_t{
                        token_position_vec_pair.second[i],
                        offset_t{
                            offset_begin_vec[i],
                            offset_begin_vec[i] + token_size,
                        }});
            }
        }
    }

    void
    index_manager_v1::print_inverted_index() const
    {
        std::cout << _inverted_index;
    }

    /*
    void
    index_manager_v1::print_token_frequency()
    {
        for (const auto &pair1 : _inverted_index)
        {
            const auto &token = pair1.first;
            //std::cout << token << KEY_VALUE_DLM1;
            std::cout << token.size() << SET_DLM1;
            position_map_t::size_type sum = 0;
            const auto &index_map = pair1.second;
            for (const auto &pair2 : index_map)
            {
                const auto &offset_set = pair2.second;
                sum += offset_set.size();
            }
            std::cout << sum << std::endl;
        }
    }

    void
    index_manager_v1::save_inverted_index(const path_t &path)
    {
        std::ofstream ofs{path, std::ofstream::out};
        for (const auto &pair1 : _inverted_index)
        {
            const auto &token = pair1.first;
            ofs << token << KEY_VALUE_DLM1;
            const auto &index_map = pair1.second;
            for (const auto &pair2 : index_map)
            {
                const auto &index = pair2.first;
                ofs << index << KEY_VALUE_DLM2;
                const auto &offset_set = pair2.second;
                for (const auto &offset : offset_set)
                    ofs << offset << SET_DLM2;
                ofs << SET_DLM1;
            }
            ofs << std::endl;
        }
        ofs.close();
    }

    void
    index_manager_v1::load_inverted_index(const path_t &path)
    {
        _inverted_index.clear();
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
        {
            doc_map_t index_map;
            std::smatch result;
            std::regex_search(line, result, KEY_VALUE_REGEX1);
            token_t token{result.str(1)};
            std::istringstream iss{result.str(2)};
            line_t index_map_str;
            while (getline(iss, index_map_str, SET_DLM1))
            {
                std::smatch result2;
                std::regex_search(index_map_str, result2, KEY_VALUE_REGEX2);
                doc_id_t index{static_cast<doc_id_t>(stoi(result2.str(1)))};
                std::istringstream iss2{result2.str(2)};
                position_map_t offset_set;
                line_t offset_str;
                while (getline(iss2, offset_str, SET_DLM2))
                    offset_set.insert(stoi(offset_str));
                index_map[index] = offset_set;
            }
            _inverted_index[token] = index_map;
        }
        inverted_index_build_collection();
    }

    void
    index_manager_v1::inverted_index_build_collection()
    {
        _collection.clear();
        for (const auto &pair1 : _inverted_index)
        {
            const auto &token = pair1.first;
            const auto &index_map = pair1.second;
            for (const auto &pair2 : index_map)
            {
                const auto &index = pair2.first;
                const auto &offset_set = pair2.second;
                if (index + 1 > _collection.size())
                    _collection.resize(index + 1);
                auto &document = _collection[index];
                for (const auto &offset : offset_set)
                {
                    if (offset + 1 > document.size())
                        document.resize(offset + 1);
                    document[offset] = token;
                }
            }
        }
    }
    */

    const frequency_t
    index_manager_v1::calc_frequency(const token_t &token) const
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
        auto intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        auto intersection_doc_id_map{intersection_inverted_index_iter->second};

        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            auto inverted_index_iter= _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {};     

            doc_id_map_t temp_doc_id_map{};
            for (const auto &intersection_doc_id_map_pair : intersection_doc_id_map)
            {
                const auto &doc_id = intersection_doc_id_map_pair.first;
                const auto &doc_id_map_iter = inverted_index_iter->second.find(doc_id);
                if (std::end(inverted_index_iter->second) == doc_id_map_iter)
                    continue;

                position_offset_vec_t temp_position_offset_vec{};
                for (const auto &intersection_position_offset : intersection_doc_id_map_pair.second)
                {
                    const auto &begin_position = intersection_position_offset.position;
                    const auto &end_position = begin_position + i;
                    for (const auto &position_offset : doc_id_map_iter->second)
                    {
                        if (position_offset.position == end_position)
                        {
                            if (temp_doc_id_map.count(doc_id) == 0)
                                temp_doc_id_map.emplace(doc_id, position_offset_vec_t{});
                            temp_doc_id_map[doc_id].emplace_back(
                                position_offset_t{
                                    begin_position,
                                    offset_t{
                                        intersection_position_offset.offset.begin,
                                        position_offset.offset.end,
                                    }});
                        }
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