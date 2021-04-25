#pragma once

#include "token_index/types.h"
#include <vector>
    
namespace token_index
{
    token_vec_t line_to_token_vec(const line_t &line);
    query_vec_t load_query_vec(const path_t &path);
}