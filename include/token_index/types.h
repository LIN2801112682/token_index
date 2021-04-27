#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

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
    using frequency_t = int;
    using position_t = doc_t::size_type;
    struct offset_t {
        token_t::size_type begin;
        token_t::size_type end;
    };

    using position_map_t = std::unordered_map<position_t, offset_t>;
    using doc_map_t = std::unordered_map<doc_id_t, position_map_t>;
    using inverted_index_t = std::unordered_map<token_t, doc_map_t>;

    using query_t = std::vector<token_t>;
    using query_vec_t = std::vector<query_t>;
    using index_set_t = std::unordered_set<doc_id_t>;
}