#pragma once

#include <vector>

namespace bm
{
    std::vector<int> bm(const char query[], const char document[]);
    int dist(const char query[], const char ch);
}
