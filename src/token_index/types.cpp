#include "token_index/types.h"

namespace ti
{
    result_intersection_set_t to_result_intersection_set_t(doc_id_map_t &&doc_id_map)
    {
        result_intersection_set_t result_intersection_set{};
        for (auto &&doc_id_map_pair : doc_id_map)
            result_intersection_set.emplace(doc_id_map_pair.first, std::move(doc_id_map_pair.second));
        return result_intersection_set;
    }

    result_intersection_set_t to_result_intersection_set_t(doc_id_position_offset_vec_t &&doc_id_position_offset_vec)
    {
        result_intersection_set_t result_intersection_set{};
        for (auto &&doc_id_position_offset : doc_id_position_offset_vec)
            result_intersection_set[doc_id_position_offset.doc_id].emplace_back(
                position_offset_t{
                    std::move(doc_id_position_offset.position),
                    std::move(doc_id_position_offset.offset),
                });
        return result_intersection_set;
    }

    result_intersection_set_t to_result_intersection_set_t(doc_id_umap_t &&doc_id_umap)
    {
        result_intersection_set_t result_intersection_set{};
        for (auto &&doc_id_umap_pair : doc_id_umap)
        {
            auto &position_offset_vec{result_intersection_set[doc_id_umap_pair.first]};
            for (auto &&position : doc_id_umap_pair.second)
                position_offset_vec.emplace_back(
                    position_offset_t{
                        std::move(position),
                        offset_t{0, 0},
                    });
        }
        return result_intersection_set;
    }
}

// ---- struct begin ---- 
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

std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_t &s)
{
    os << "{doc_id = " << s.doc_id
       << ", position = " << s.position
       << ", offset = " << s.offset
       << ", }";
    return os;
}
// ---- struct end ---- 

// ---- vector begin ---- 
template <>
std::ostream &operator<<(std::ostream &os, const std::vector<ti::str_t> &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::vector<ti::doc_id_t> &v)
{
    os << "[" << std::endl;
    for (const auto &e : v)
        os << " " << e << "," << std::endl;
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::vector<ti::position_offset_t> &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::vector<ti::doc_id_position_offset_t> &v)
{
    os << "[";
    for (const auto &e : v)
        os << e << ", ";
    os << "]";
    return os;
}
// ---- vector end ---- 

// ---- set begin ----
// ---- set end ----

// ---- unordered_set begin ----
template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<ti::doc_id_t> &us)
{
    os << "[";
    for (const auto &e : us)
        os << e << ", ";
    os << "]";
    return os;
}
// ---- unordered_set end ----

// ---- map begin ----
template <>
std::ostream &operator<<(std::ostream &os, const std::map<ti::doc_id_t, ti::position_offset_vec_t> &m)
{
    os << "[";
    for (const auto &p : m)
        os << "{" << p.first << " : " << p.second << "}, ";
    os << "]";
    return os;
}
// ---- map end ----

// ---- unordered_map begin ----
template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<ti::doc_id_t, ti::position_uset_t> &um)
{
    os << "[";
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}, ";
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<ti::doc_id_t, ti::position_offset_vec_t> &um)
{
    os << "[";
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}, ";
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<ti::str_t, ti::doc_id_map_t> &um)
{
    os << "[" << std::endl;
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}," << std::endl;
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<ti::str_t, ti::doc_id_position_offset_vec_t> &um)
{
    os << "[" << std::endl;
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}," << std::endl;
    os << "]";
    return os;
}

template <>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<ti::str_t, ti::doc_id_umap_t> &um)
{
    os << "[" << std::endl;
    for (const auto &p : um)
        os << " {" << p.first << " : " << p.second << "}," << std::endl;
    os << "]";
    return os;
}
// ---- unordered_map end ----