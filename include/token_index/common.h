#pragma once

#include "token_index/types.h"
#include <vector>
#include <iostream>
#include <tuple>
    
namespace ti
{
    
    std::vector<token_t> line_to_token_vec(const line_t &line);
    std::tuple<doc_id_t, doc_t, line_t> line_to_doc_id_and_doc(const line_t &line);
    std::vector<query_t> load_query_vec(const path_t &path);
}
