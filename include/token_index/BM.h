#pragma once

#include "token_index/header.h"

namespace token_index
{
    int Dist(char *t, char ch);
    std::vector<int> BM(char *s, char *t);
}