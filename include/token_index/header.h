#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstdlib>

namespace token_index
{
    using path_t = std::string;
    using line_t = std::string;
    using token_t = std::string;
    using document_t = std::unordered_set<token_t>;
    using collection_t = std::vector<document_t>;
    using index_t = collection_t::size_type;
    using index_set_t = std::unordered_set<index_t>;
    using inverted_index_t = std::unordered_map<token_t, index_set_t>;
    using query_t = std::vector<token_t>;
    using query_vec_t = std::vector<query_t>;
}