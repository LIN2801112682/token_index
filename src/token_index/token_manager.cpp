#include "token_index/token_manager.h"

namespace token_index
{
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
        document_t document{line_to_document(line)};
        push_document(document);
    }

    void
    index_manager::push_document(const document_t &document)
    {
        _collection.push_back(document);
        index_t index{_collection.size() - 1};
        for (offset_t offset = 0; offset < document.size(); ++offset)
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

    document_t
    index_manager::line_to_document(const line_t &line)
    {
        document_t document;
        token_t token;
        std::istringstream iss{line};
        while (iss >> token)
            document.push_back(token);
        return document;
    }

    void
    index_manager::print_collection()
    {
        for (index_t index = 0; index < _collection.size(); ++index)
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
                index_t index{static_cast<index_t>(stoi(result2.str(1)))};
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

    query_vec_t
    index_manager::load_query(const path_t &path)
    {
        query_vec_t query_vec;
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
        {
            query_t query;
            std::istringstream iss{line};
            token_t token;
            while (iss >> token)
                query.push_back(token);
            query_vec.push_back(query);
        }
        ifs.close();
        return query_vec;
    }

    index_set_t
    index_manager::retrieve(const token_t &token) const
    {
        auto iter = _inverted_index.find(token);
        if (iter != std::end(_inverted_index))
        {
            index_set_t index_set;
            const auto &index_map = iter->second;
            for (const auto &pair : index_map)
            {
                index_set.insert(pair.first);
            }
            return index_set;
        }
        return index_set_t{};
    }

    index_set_t
    index_manager::retrieve_union(const query_t &query) const
    {
        index_set_t union_set{};
        for (const token_t &token : query)
        {
            index_set_t index_set = retrieve(token);
            union_set.insert(std::begin(index_set), std::end(index_set));
        }
        return union_set;
    }

    index_set_t
    index_manager::retrieve_intersection(const query_t &query) const
    {
        index_set_t intersection_set;
        bool inited{false};
        for (const token_t &token : query)
        {
            index_set_t index_set = retrieve(token);
            if (index_set.empty())
                return index_set_t{};
            if (!inited)
            {
                intersection_set = index_set;
                inited = true;
            }
            else
            {
                index_set_t temp_set;
                std::set_intersection(std::begin(intersection_set), std::end(intersection_set),
                        std::begin(index_set), std::end(index_set),
                        std::inserter(temp_set, std::begin(temp_set)));
                intersection_set = temp_set;
            }
        }
        return intersection_set;
    }
}