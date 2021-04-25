#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace token_index
{
    using path_t = std::string;
    using line_t = std::string;
    using line_vec_t = std::vector<line_t>;

    using token_t = std::string;
    using token_vec_t = std::vector<token_t>;
    using document_t = token_vec_t;
    using collection_t = std::vector<document_t>;

    using index_t = collection_t::size_type;
    using offset_t = document_t::size_type;
    using offset_set_t = std::unordered_set<offset_t>;
    using index_map_t = std::unordered_map<index_t, offset_set_t>;
    using inverted_index_t = std::unordered_map<token_t, index_map_t>;

    using query_t = token_vec_t;
    using query_vec_t = std::vector<query_t>;
    using index_set_t = std::unordered_set<index_t>;
}