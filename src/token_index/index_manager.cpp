#include "token_index/index_manager.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace ti
{
    static const std::regex KEY_VALUE_REGEX1{"(.*)=(.*)"};
    static constexpr char KEY_VALUE_DLM1{'='};
    static const std::regex KEY_VALUE_REGEX2{"(.*):(.*)"};
    static constexpr char KEY_VALUE_DLM2{':'};
    static constexpr char SET_DLM1{';'};
    static constexpr char SET_DLM2{','};

    index_manager::index_manager()
        : _collection{},
          _inverted_index{} {}

    void
    index_manager::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        line_t doc_line;
        while (getline(ifs, doc_line))
            push_doc_line(doc_line);
        ifs.close();
    }

    void
    index_manager::push_doc_line(const line_t &doc_line)
    {
        doc_t doc{line_to_token_vec(doc_line)};
        _collection.push_back(doc);
        doc_id_t doc_id{_collection.size() - 1};

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
            const auto &position_vec = token_position_pair.second;
            const auto &offset_begin_vec = bm::BoyerMoore(doc_line.c_str(), doc_line.size(), token.c_str(), token.size());
            position_offset_vec_t position_offset_vec{};
            for (position_offset_vec_t::size_type i{0}; position_offset_vec.size(); ++i)
            {
                const auto &position = position_vec[i];
                const line_t::size_type &offset_begin = offset_begin_vec[i];
                position_offset_t position_offset{position, offset_t{offset_begin, offset_begin + token.size()}};
                position_offset_vec.push_back(position_offset);
            }
            doc_id_map_t doc_id_map{};
            doc_id_map[doc_id] = position_offset_vec;
            _inverted_index[token] = doc_id_map;
        }
    }

    /*
    void
    index_manager::print_collection()
    {
        for (doc_id_t index = 0; index < _collection.size(); ++index)
        {
            std::cout << index << KEY_VALUE_DLM1;
            for (const token_t &token : _collection[index])
                std::cout << token << SET_DLM1;
            std::cout << std::endl;
        }
    }

    void
    index_manager::print_inverted_index()
    {
        for (const auto &pair1 : _inverted_index)
        {
            const auto &token = pair1.first;
            std::cout << token << KEY_VALUE_DLM1;
            const auto &index_map = pair1.second;
            for (const auto &pair2 : index_map)
            {
                const auto &index = pair2.first;
                std::cout << index << KEY_VALUE_DLM2;
                const auto &offset_set = pair2.second;
                for (const auto &offset : offset_set)
                    std::cout << offset << SET_DLM2;
                std::cout << SET_DLM1;
            }
            std::cout << std::endl;
        }
    }

    void
    index_manager::print_token_frequency()
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
    index_manager::save_inverted_index(const path_t &path)
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
    index_manager::load_inverted_index(const path_t &path)
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
    index_manager::inverted_index_build_collection()
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
    index_manager::calc_frequency(const token_t &token) const
    {
        frequency_t frequency{0};
        const auto &doc_id_map = retrieve(token);
        for (const auto &doc_id_map_pair : doc_id_map)
        {
            const auto &position_offset_vec = doc_id_map_pair.second;
            frequency += position_offset_vec.size();
        }
        return frequency;
    }

    const doc_id_map_t
    index_manager::retrieve(const token_t &token) const
    {
        doc_id_map_t doc_id_map{};
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) != inverted_index_iter)
            doc_id_map = inverted_index_iter->second;
        return doc_id_map;
    }

    const doc_id_set_t
    index_manager::retrieve_union(const query_t &query) const
    {
        doc_id_set_t union_set{};
        for (const auto &token : query)
        {
            const auto &doc_id_map = retrieve(token);
            for (const auto &doc_id_map_pair : doc_id_map)
            {
                const auto &doc_id = doc_id_map_pair.first;
                union_set.insert(doc_id);
            }
        }
        return union_set;
    }

    const doc_id_map_t
    index_manager::retrieve_intersection(const query_t &query) const
    {
        const auto &first_token = query[0];
        const auto &first_doc_id_map = retrieve(first_token);
        if (first_doc_id_map.empty())
            return {};
        auto intersection_set{first_doc_id_map};
        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &doc_id_map = retrieve(token);
            if (doc_id_map.empty())
                return {};
            doc_id_map_t temp_doc_id_map{};
            for (const auto &intersection_doc_id_map_pair : intersection_set)
            {
                const auto &doc_id = intersection_doc_id_map_pair.first;
                const auto &doc_id_map_iter = doc_id_map.find(doc_id);
                if (std::end(doc_id_map) == doc_id_map_iter)
                    continue;
                const auto &intersection_position_offset_vec = intersection_doc_id_map_pair.second;
                const auto &position_offset_vec = doc_id_map_iter->second;
                position_offset_vec_t temp_position_offset_vec{};
                for (const auto &intersection_position_offset : intersection_position_offset_vec)
                {
                    const auto &begin_position = intersection_position_offset.position;
                    const auto &end_position = begin_position + i;
                    for (const auto &position_offset : position_offset_vec)
                    {
                        if (position_offset.position != end_position)
                            continue;
                        const auto &offset_begin = intersection_position_offset.offset.begin;
                        const auto &offset_end = position_offset.offset.end;
                        position_offset_t temp_position_offset{begin_position, offset_t{offset_begin, offset_end}};
                        temp_position_offset_vec.push_back(temp_position_offset);
                    }
                }
                if (!temp_position_offset_vec.empty())
                    temp_doc_id_map[doc_id] = temp_position_offset_vec;
            }
            if (temp_doc_id_map.empty())
                return {};
            intersection_set = temp_doc_id_map;
        }
        return intersection_set;
    }
}