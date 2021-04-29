#include "token_index/types.h"

namespace ti
{
    result_intersection_set_t to_result_intersection_set_t(doc_id_map_t &doc_id_map)
    {
        result_intersection_set_t result_intersection_set{};
        for (auto &doc_id_map_pair : doc_id_map)
        {
            auto &doc_id = doc_id_map_pair.first;
            auto &position_offset_vec = doc_id_map_pair.second;
            for (auto &position_offset : position_offset_vec)
            {
                doc_id_position_offset_t doc_id_position_offset{
                    doc_id_map_pair.first,
                    position_offset.position,
                    position_offset.offset};
                result_intersection_set.push_back(doc_id_position_offset);
            }
        }
        return result_intersection_set;
    }
}

std::ostream &operator<<(std::ostream &os, const ti::result_union_set_t &us)
{
    os << "[";
    for (const auto &e : us)
        os << e << ", ";
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_t &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::col_t &v)
{
    os << "[" << std::endl;
    for (const auto &e : v)
        os << " " << e << "," << std::endl;
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::offset_t &s)
{
    os << "{begin = " << s.begin
       << ", end = " << s.end
       << ", }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::position_offset_t &s)
{
    os << "{position = " << s.position
       << ", offset = " << s.offset
       << ", }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::position_offset_vec_t &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_map_t &m)
{
    os << "[";
    for (const auto &p : m)
        os << "{" << p.first << " : " << p.second << "}, ";
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v1_t &um)
{
    os << "[" << std::endl;
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}," << std::endl;
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_t &s)
{
    os << "{doc_id = " << s.doc_id
       << ", position = " << s.position
       << ", offset = " << s.offset
       << ", }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_vec_t &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v2_t &um)
{
    os << "[" << std::endl;
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}," << std::endl;
    os << "]";
    return os;
}