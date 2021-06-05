#include "token_index/common.h"
#include "token_index/types.h"
#include "common/scope_exit.h"
#include <sstream>
#include <fstream>
#include <tuple>
#include <algorithm>

namespace ti
{
    std::tuple<doc_id_t, doc_t, str_t>
    line_to_doc(const str_t &line)
    {
        str_t token;
        std::istringstream iss{line};

        iss >> token;
        str_t new_line{line.substr(token.size() + 1)};
        doc_id_t doc_id{stoul(token)};

        doc_t doc{};
        while (iss >> token)
            doc.push_back(token);

        return {doc_id, doc, new_line};
    }

    query_t
    line_to_query(const str_t &line)
    {
        query_t query{};
        str_t token;
        std::istringstream iss{line};
        while (iss >> token)
            query.push_back(token);
        return query;
    }

    std::tuple<std::vector<query_t>, std::vector<str_t>>
    load_query_vec(const path_t &path)
    {
        std::vector<query_t> query_vec;
        std::vector<str_t> query_line_vec;
        std::ifstream ifs{path, std::ifstream::in};
        SCOPE_GUARD
        {
            ifs.close();
        };
        str_t line;
        while (getline(ifs, line))
        {
            std::transform(std::begin(line), std::end(line), std::begin(line), tolower);
            query_vec.push_back(line_to_query(line));
            query_line_vec.push_back(line);
        }
        return {query_vec, query_line_vec};
    }

    std::vector<std::regex>
    load_regex_vec(const path_t &path)
    {
        std::vector<std::regex> regex_vec;
        std::ifstream ifs{path, std::ifstream::in};
        SCOPE_GUARD
        {
            ifs.close();
        };
        str_t line;
        while (getline(ifs, line))
        {
            regex_vec.emplace_back(std::regex{line});
        }
        return regex_vec;
    }
}