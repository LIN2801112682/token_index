#pragma once

#include <vector>

namespace bm
{
    int calcBC(const char *subdir, int bc[]);
    int calcSuffix(const char *subdir, int suffix[]);
    int calcGS(const char *subdir, int gs[]);
    std::vector<int> BoyerMoore(const char *szText, int nTextLen, const char *szSubstr, int nStrLen);
}
