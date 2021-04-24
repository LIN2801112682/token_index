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
        for (const token_t &token : document)
        {
            auto iter = _inverted_index.find(token);
            if (iter == std::end(_inverted_index))
            {
                index_set_t index_set;
                index_set.insert(index);
                _inverted_index[token] = index_set;
            }
            else
                iter->second.insert(index);
        }
    }

    document_t
    index_manager::line_to_document(const line_t &line)
    {
        document_t document;
        token_t token;
        std::istringstream iss{line};
        while (iss >> token)
            if (token.compare(" ") != 0)
                document.insert(token);
        return document;
    }

    void
    index_manager::print_collection()
    {
        for (index_t index = 0; index < _collection.size(); ++index)
        {
            std::cout << index << KEY_VALUE_DLM;
            for (const token_t &token : _collection[index])
                std::cout << token << SET_DLM;
            std::cout << std::endl;
        }
    }

    void
    index_manager::print_inverted_index()
    {
        for (const auto &pair : _inverted_index)
        {
            std::cout << pair.first << KEY_VALUE_DLM;
            for (const index_t &index : pair.second)
                std::cout << index << SET_DLM;
            std::cout << std::endl;
        }
    }

    void
    index_manager::print_token_frequency()
    {
        for (const auto &pair : _inverted_index)
            std::cout << pair.first << KEY_VALUE_DLM
                      << pair.first.size() << '-'
                      << pair.second.size() << std::endl;
    }

    void
    index_manager::save_inverted_index(const path_t &path)
    {
        std::ofstream ofs{path, std::ofstream::out};
        for (const auto &pair : _inverted_index)
        {
            ofs << pair.first << KEY_VALUE_DLM;
            for (const index_t &index : pair.second)
                ofs << index << SET_DLM;
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
            std::smatch result;
            std::regex_search(line, result, KEY_VALUE_REGEX);
            token_t token{result.str(1)};
            std::istringstream iss{result.str(2)};
            index_set_t index_set;
            std::string index_str;
            while (getline(iss, index_str, SET_DLM))
                index_set.insert(stoi(index_str));
            _inverted_index[token] = index_set;
        }
        inverted_index_build_collection();
    }

    void
    index_manager::inverted_index_build_collection()
    {
        _collection.clear();
        for (const auto &pair : _inverted_index)
        {
            token_t token = pair.first;
            index_set_t index_set = pair.second;
            for (const auto &index : index_set)
            {
                if (index + 1 > _collection.size())
                    _collection.resize(index + 1);
                _collection[index].insert(token);
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
            return iter->second;
        return index_set_t{};
    }

    index_set_t
    index_manager::retrieve_union(const query_t &query) const
    {
        index_set_t index_set;
        for (const token_t &token : query)
        {
            auto iter = _inverted_index.find(token);
            if (iter != std::end(_inverted_index))
                index_set.insert(std::begin(iter->second), std::end(iter->second));
        }
        return index_set;
    }

    index_set_t
    index_manager::retrieve_intersection(const query_t &query) const
    {
        index_set_t index_set;
        bool inited{false};
        for (const token_t &token : query)
        {
            auto iter = _inverted_index.find(token);
            if (iter != std::end(_inverted_index))
            {
                if (!inited)
                {
                    index_set.insert(std::begin(iter->second), std::end(iter->second));
                    inited = true;
                }
                else
                {
                    index_set_t intersection_set;
                    std::set_intersection(std::begin(index_set), std::end(index_set),
                        std::begin(iter->second), std::end(iter->second),
                        std::inserter(intersection_set, std::begin(intersection_set)));
                    index_set = intersection_set;
                }
            }
        }
        return index_set;
    }
}