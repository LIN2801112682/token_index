#pragma once

#include <vector>
#include <string>

namespace bm
{
    int calcBC(const char *subdir, int bc[]);
    int calcSuffix(const char *subdir, int suffix[]);
    int calcGS(const char *subdir, int gs[]);
    std::vector<std::string::size_type> BoyerMoore(const std::string &doc_line, const std::string &query_line);
}
