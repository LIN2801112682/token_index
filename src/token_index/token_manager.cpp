#include "token_index/token_manager.h"
#include "token_index/types.h"
#include "token_index/common.h"
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
    index_manager::push_file(const path_t &path)
    {
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
            push_line(line);
        ifs.close();
    }

    void
    index_manager::push_line(const line_t &line)
    {
        doc_t document{line_to_token_vec(line)};
        push_document(document);
    }

    void
    index_manager::push_document(const doc_t &document)
    {
        _collection.push_back(document);
        doc_id_t index{_collection.size() - 1};
        for (position_t offset = 0; offset < document.size(); ++offset)
        {
            auto token = document[offset];
            auto iter1 = _inverted_index.find(token);
            if (iter1 == std::end(_inverted_index))
            {
                index_map_t index_map;
                offset_set_t offset_set;
                offset_set.insert(offset);
                index_map[index] = offset_set; 
                _inverted_index[token] = index_map;
            }
            else
            {
                auto &index_map = iter1->second;
                auto iter2 = index_map.find(index);
                if (iter2 == std::end(index_map))
                {
                    offset_set_t offset_set;
                    offset_set.insert(offset);
                    index_map[index] = offset_set;
                }
                else
                {
                    auto &offset_set = iter2->second;
                    offset_set.insert(offset);
                }
            }
        }
    }


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
            offset_set_t::size_type sum = 0;
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
            index_map_t index_map;
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
                offset_set_t offset_set;
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

    index_map_t
    index_manager::retrieve(const token_t &token) const
    {
        auto iter = _inverted_index.find(token);
        if (iter != std::end(_inverted_index))
        {
            return iter->second;
        }
        return index_map_t{};
    }

    index_set_t
    index_manager::retrieve_union(const query_t &query) const
    {
        index_set_t union_set{};
        for (const auto &token : query)
        {
            const auto &index_map = retrieve(token);
            for (const auto &pair : index_map)
                union_set.insert(pair.first);
        }
        return union_set;
    }

    index_map_t
    index_manager::retrieve_intersection(const query_t &query) const
    {
        const auto &first_token = query[0];
        const auto &first_index_map = retrieve(first_token);
        if (first_index_map.empty())
            return index_map_t{};
        auto intersection_set{first_index_map};
        for (size_t i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &index_map = retrieve(token);
            if (index_map.empty())
                return index_map_t{};
            decltype(intersection_set) temp_set;
            for (const auto &pair : intersection_set)
            {
                const auto &index = pair.first; 
                const auto &iter = index_map.find(index);
                if (iter == std::end(index_map))
                    continue;
                const auto &first_offset_set = pair.second;
                const auto &offset_set = iter->second;
                offset_set_t temp_offset_set;
                for (const auto &offset : first_offset_set)
                {
                    if (offset_set.find(offset + i) != std::end(offset_set))
                        temp_offset_set.insert(offset);
                }
                if (!temp_offset_set.empty())
                    temp_set[index] = temp_offset_set;
            }
            if (temp_set.empty())
                return index_map_t{};
            intersection_set = temp_set;
        }
        return intersection_set;
    }
}