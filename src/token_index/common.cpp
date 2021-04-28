#include "token_index/common.h"
#include "token_index/types.h"
#include <sstream>
#include <fstream>

namespace ti
{
    std::vector<token_t>
    line_to_token_vec(const line_t &line)
    {
        std::vector<token_t> token_vec{};
        token_t token;
        std::istringstream iss{line};
        while (iss >> token)
            if (token != " ")
                token_vec.push_back(token);
        return token_vec;
    }
    
    query_vec_t
    load_query_vec(const path_t &path)
    {
        query_vec_t query_vec;
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
            query_vec.push_back(line_to_token_vec(line));
        ifs.close();
        return query_vec;
    }
}

std::ostream &operator<<(std::ostream &os, const ti::col_t &col)
{
    for (ti::doc_id_t doc_id{0}; doc_id < col.size(); ++doc_id)
    {
        os << doc_id << ": ";
        for (const auto &token : col[doc_id])
            os << token << ", ";
        os << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::inverted_index_t &inverted_index)
{
    for (const auto &inverted_index_pair : inverted_index)
    {
        const auto &token = inverted_index_pair.first;
        os << token << ": ";
        const auto &doc_id_map = inverted_index_pair.second;
        os << doc_id_map << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_set_t &doc_id_set)
{
    for (const auto &doc_id : doc_id_set)
        os << doc_id << ", ";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_map_t &doc_id_map)
{
    for (const auto &doc_id_map_pair : doc_id_map)
    {
        const auto &doc_id = doc_id_map_pair.first;
        os << doc_id << "= ";
        const auto &position_offset_vec = doc_id_map_pair.second;
        for (const auto &position_offset : position_offset_vec)
        {
            const auto &position = position_offset.position;
            const auto &offset = position_offset.offset;
            os << position << "- " << offset.begin << " ~ " << offset.end << ", ";
        }
        os << "; ";
    }
    return os;
}