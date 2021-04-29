#include "token_index/types.h"

std::ostream &operator<<(std::ostream &os, const ti::doc_id_set_t &s)
{
    os << "[";
    for (const auto &e : s)
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