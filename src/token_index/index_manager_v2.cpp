#include "token_index/index_manager_v2.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace ti
{
    index_manager_v2::index_manager_v2()
        : _col{},
          _inverted_index{} {}

    void
    index_manager_v2::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        line_t doc_line;
        while (getline(ifs, doc_line))
            push_doc_line(doc_line);
        ifs.close();
    }

    void
    index_manager_v2::push_doc_line(const line_t &doc_line)
    {
        doc_t doc{line_to_token_vec(doc_line)};
        _col.push_back(doc);
        doc_id_t doc_id{_col.size() - 1};

        std::map<token_t, std::vector<position_t>> token_position_map{};
        for (position_t position{0}; position < doc.size(); ++position)
        {
            const auto &token = doc[position];
            auto token_position_iter = token_position_map.find(token);
            if (std::end(token_position_map) != token_position_iter)
                token_position_iter->second.push_back(position);
            else
            {
                std::vector<position_t> position_vec{position};
                token_position_map[token] = position_vec;
            }
        }

        for (const auto &token_position_pair : token_position_map)
        {
            const auto &token = token_position_pair.first;
            index_info_vec_t index_info_vec{};
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) != inverted_index_iter)
                index_info_vec = inverted_index_iter->second;

            const auto &position_vec = token_position_pair.second;
            const auto &offset_begin_vec = bm::BoyerMoore(doc_line.c_str(), doc_line.size(), token.c_str(), token.size());
            for (std::size_t i{0}; i < position_vec.size(); ++i)
            {
                const auto &position{position_vec[i]};
                const line_t::size_type &offset_begin = offset_begin_vec[i];
                const offset_t offset{offset_begin, offset_begin + token.size() - 1};
                index_info_t index_info{doc_id, position, offset};
                index_info_vec.push_back(index_info);
            }
            _inverted_index[token] = index_info_vec;
        }
    }

    void
    index_manager_v2::print_col() const
    {
        std::cout << _col;
    }

    void
    index_manager_v2::print_inverted_index() const
    {
        std::cout << _inverted_index;
    }

    /*
    void
    index_manager_v2::print_token_frequency()
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
    index_manager_v2::save_inverted_index(const path_t &path)
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
    index_manager_v2::load_inverted_index(const path_t &path)
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
    index_manager_v2::inverted_index_build_collection()
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

    frequency_t
    index_manager_v2::calc_frequency(const token_t &token) const
    {
        const auto &index_info_vec = retrieve(token);
        return index_info_vec.size();
    }

    const index_info_vec_t
    index_manager_v2::retrieve(const token_t &token) const
    {
        index_info_vec_t index_info_vec{};
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) != inverted_index_iter)
            index_info_vec = inverted_index_iter->second;
        return index_info_vec;
    }

    const doc_id_set_t
    index_manager_v2::retrieve_union(const query_t &query) const
    {
        doc_id_set_t union_set{};
        for (const auto &token : query)
        {
            const auto &index_info_vec = retrieve(token);
            for (const auto &index_info : index_info_vec)
            {
                const auto &doc_id = index_info.doc_id;
                union_set.insert(doc_id);
            }
        }
        return union_set;
    }

    const index_info_vec_t
    index_manager_v2::retrieve_intersection(const query_t &query) const
    {
        const auto &first_token = query[0];
        const auto &first_index_info_vec = retrieve(first_token);
        if (first_index_info_vec.empty())
            return {};
        auto intersection_set{first_index_info_vec};
        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &index_info_vec = retrieve(token);
            if (index_info_vec.empty())
                return {};
            index_info_vec_t temp_index_info_vec{};
            for (const auto &intersection_index_info : intersection_set)
            {
                const auto &doc_id = intersection_index_info.doc_id;
                const auto &position = intersection_index_info.position;
                for (const auto &index_info : index_info_vec)
                {
                    if (doc_id != index_info.doc_id)
                        continue;
                    if (position + i != index_info.position)
                        continue;
                    offset_t temp_offset{intersection_index_info.offset.begin, index_info.offset.end};
                    index_info_t temp_index_info{doc_id, position, temp_offset};
                    temp_index_info_vec.push_back(temp_index_info);
                }
            }
            if (temp_index_info_vec.empty())
                return {};
            intersection_set = temp_index_info_vec;
        }
        return intersection_set;
    }
}