#pragma once

#include "token_index/types.h"
#include <vector>
#include <iostream>
    
namespace ti
{
    
    std::vector<token_t> line_to_token_vec(const line_t &line);
    query_vec_t load_query_vec(const path_t &path);
}

std::ostream &operator<<(std::ostream &os, const ti::col_t &col);
std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v1_t &inverted_index);
std::ostream &operator<<(std::ostream &os, const ti::inverted_index_v2_t &inverted_index);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_set_t &doc_id_set);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_map_t &doc_id_map);
std::ostream &operator<<(std::ostream &os, const ti::index_info_vec_t &index_info_vec_t);