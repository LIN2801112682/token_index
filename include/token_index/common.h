#pragma once

#include "token_index/types.h"
#include <vector>
#include <iostream>
    
namespace ti
{
    
    std::vector<token_t> line_to_token_vec(const line_t &line);
    std::vector<query_t> load_query_vec(const path_t &path);
}
