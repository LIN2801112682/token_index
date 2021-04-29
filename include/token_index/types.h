#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <iostream>

namespace ti
{
    using path_t = std::string;
    using line_t = std::string;
    using token_t = std::string;
    using doc_t = std::vector<token_t>; //doc = document
    using col_t = std::vector<doc_t>; // col = collection

    using doc_id_t = col_t::size_type;
    using position_t = doc_t::size_type;
    struct offset_t
    {
        line_t::size_type begin;
        line_t::size_type end;
    };

    struct position_offset_t
    {
        position_t position;
        offset_t offset;
    };
    using position_offset_vec_t = std::vector<position_offset_t>;
    using doc_id_map_t = std::map<doc_id_t, position_offset_vec_t>;
    using inverted_index_v1_t = std::unordered_map<token_t, doc_id_map_t>;

    struct doc_id_position_offset_t
    {
        doc_id_t doc_id;
        position_t position;
        offset_t offset;
    };
    using doc_id_position_offset_vec_t = std::vector<doc_id_position_offset_t>;
    using inverted_index_v2_t = std::unordered_map<token_t, doc_id_position_offset_vec_t>;

    using frequency_t = doc_t::size_type;
    using query_t = std::vector<token_t>;
    using result_union_set_t = std::unordered_set<doc_id_t>;
    using result_intersection_set_t = std::vector<doc_id_position_offset_t>;

    result_intersection_set_t to_result_intersection_set_t(doc_id_map_t &);
}

std::ostream &operator<<(std::ostream &os, const ti::result_union_set_t &);

std::ostream &operator<<(std::ostream &os, const ti::doc_t& );
std::ostream &operator<<(std::ostream &os, const ti::col_t &);

std::ostream &operator<<(std::ostream &os, const ti::offset_t &);

std::ostream &operator<<(std::ostream &os, const ti::position_offset_t &);
std::ostream &operator<<(std::ostream &os, const ti::position_offset_vec_t &);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_map_t &);
std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v1_t &);

std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_t &);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_position_offset_vec_t &);
std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v2_t &);