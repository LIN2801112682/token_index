#pragma once

#include "token_index/types.h"
#include <vector>
#include <iostream>
    
namespace ti
{
    
    std::vector<token_t> line_to_token_vec(const line_t &line);
    query_vec_t load_query_vec(const path_t &path);
}

std::ostream &operator<<(std::ostream &os, const ti::doc_id_set_t &doc_id_set);
std::ostream &operator<<(std::ostream &os, const ti::doc_id_map_t &doc_id_map);