#include "bm/bm.h"
#include <cstring>

namespace bm
{
    std::vector<int>
    bm(const char query[], const char document[])
    {
        std::vector<int> result;
        auto query_len = strlen(query);
        auto document_len = strlen(document);
        auto query_index = query_len - 1;
        auto document_index = query_len - 1;
        while (0 <= query_index && document_index < document_len)
        {
            if (query[query_index] == document[document_index])
            {
                --query_index;
                --document_index;
            }
            else
            {
                query_index = query_len - 1;
                document_index += dist(query, document[document_index]);
            }
            if (query_index < 0)
            {
                query_index = query_len - 1;
                ++document_index;
                result.push_back(document_index);
                document_index += 2 * query_len - 2;
            }
        }
        return result;
    }

    int
    dist(const char query[], const char ch)
    {
        int query_len = strlen(query);
        int query_index = query_len - 1;
        if (query[query_index] == ch)
            return query_len;
        --query_index;
        while (0 <= query_index)
        {
            if (query[query_index] == ch)
                return query_len - query_index - 1;
            else
                --query_index;
        }
        return query_len;
    }
}