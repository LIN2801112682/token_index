#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <iostream>

namespace ti
{
    using path_t = std::string;
    using str_t = std::string; //str = string
    using doc_t = std::vector<str_t>; //doc = document
    using col_t = std::vector<doc_t>; // col = collection

    using doc_id_t = col_t::size_type;
    using position_t = doc_t::size_type;
    using str_idx_t = str_t::size_type; 
    struct offset_t
    {
        str_idx_t begin;
        str_idx_t end;
    };

    struct position_offset_t
    {
        position_t position;
        offset_t offset;
    };
    using position_offset_vec_t = std::vector<position_offset_t>;
    using doc_id_map_t = std::map<doc_id_t, position_offset_vec_t>;
    using inverted_index_v1_t = std::unordered_map<str_t, doc_id_map_t>;

    struct doc_id_position_offset_t
    {
        doc_id_t doc_id;
        position_t position;
        offset_t offset;
    };
    using doc_id_position_offset_vec_t = std::vector<doc_id_position_offset_t>;
    using inverted_index_v2_t = std::unordered_map<str_t, doc_id_position_offset_vec_t>;

    using position_uset_t = std::unordered_set<position_t>;
    using doc_id_umap_t = std::unordered_map<doc_id_t, position_uset_t>;
    using inverted_index_v3_t = std::unordered_map<str_t, doc_id_umap_t>;

    using frequency_t = doc_t::size_type;
    using query_t = std::vector<str_t>;
    using result_union_set_t = std::unordered_set<doc_id_t>;
    using result_intersection_set_t = std::vector<doc_id_position_offset_t>;
    struct token_relative_position_frequency_t
    {
        str_t token;
        position_t relative_position;
        frequency_t frequency;
    };

    result_intersection_set_t to_result_intersection_set_t(const doc_id_map_t &);
    result_intersection_set_t to_result_intersection_set_t(const doc_id_umap_t &);
}

std::ostream &operator<<(std::ostream &os, const ti::offset_t &);
std::ostream &operator<<(std::ostream &os, const ti::position_offset_t &);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_t &);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &);

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<T> &);

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os, const std::map<K, V> &);

template <typename K, typename V>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<K, V> &);
