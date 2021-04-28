#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <map>

namespace ti
{
    enum class index_level
    {
        doc_id = 1,
        frequency,
        position,
        offset, 
    };

    using path_t = std::string;
    using line_t = std::string;
    using line_vec_t = std::vector<line_t>;

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
    using inverted_index_t = std::unordered_map<token_t, doc_id_map_t>;

    using frequency_t = position_offset_vec_t::size_type;

    using query_t = std::vector<token_t>;
    using query_vec_t = std::vector<query_t>;
    using doc_id_set_t = std::unordered_set<doc_id_t>;
}