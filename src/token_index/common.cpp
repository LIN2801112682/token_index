#include "token_index/common.h"
#include "token_index/types.h"
#include <sstream>
#include <fstream>

namespace ti
{
    std::vector<token_t>
    line_to_token_vec(const line_t &line)
    {
        std::vector<token_t> token_vec;
        token_t token;
        std::istringstream iss{line};
        while (iss >> token)
            token_vec.push_back(token);
        return token_vec;
    }
    
    query_vec_t
    load_query_vec(const path_t &path)
    {
        query_vec_t query_vec;
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
            query_vec.push_back(line_to_token_vec(line));
        ifs.close();
        return query_vec;
    }
}