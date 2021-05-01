#pragma once

#include "token_index/types.h"
#include <vector>
#include <iostream>
#include <tuple>
    
namespace ti
{
    std::tuple<doc_id_t, doc_t, str_t> line_to_doc(const str_t &line);
    std::vector<query_t> load_query_vec(const path_t &path);
}
