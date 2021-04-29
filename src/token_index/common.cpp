#include "token_index/common.h"
#include "token_index/types.h"
#include <sstream>
#include <fstream>
#include <tuple>

namespace ti
{
    std::vector<token_t>
    line_to_token_vec(const line_t &line)
    {
        std::vector<token_t> token_vec{};
        token_t token;
        std::istringstream iss{line};
        while (iss >> token)
            token_vec.push_back(token);
        return token_vec;
    }

    std::tuple<doc_id_t, doc_t, line_t>
    line_to_doc_id_and_doc(const line_t &line)
    {
        token_t token;
        std::istringstream iss{line};

        iss >> token;
        line_t new_line{line.substr(token.size() + 1)};
        doc_id_t doc_id{stoul(token)};

        doc_t doc{};
        while (iss >> token)
            doc.push_back(token);


        return {doc_id, doc, new_line}; 
    }
    
    std::vector<query_t>
    load_query_vec(const path_t &path)
    {
        std::vector<query_t> query_vec;
        std::ifstream ifs{path, std::ifstream::in};
        line_t line;
        while (getline(ifs, line))
        {
            std::transform(std::begin(line), std::end(line), std::begin(line), tolower);
            query_vec.push_back(line_to_token_vec(line));
        }
        ifs.close();
        return query_vec;
    }
}
