#include "token_index/BM.h"

namespace token_index
{
    int dist(char *t, char ch)
    {
        int len = strlen(t);
        int i = len - 1;
        if (ch == t[i])
            return len;
        --i;
        while (i >= 0)
        {
            if (ch == t[i])
                return len - 1 - i;
            else
                --i;
        }
        return len;
    }

    std::vector<int> BM(char *s, char *t)
    {
        std::vector<int> result;
        int n = strlen(s);
        int m = strlen(t);
        int i = m - 1;
        int j = m - 1;
        while (j >= 0 && i < n)
        {
            if (s[i] == t[j])
            {
                --i;
                --j;
            }
            else
            {
                i += dist(t, s[i]);
                j = m - 1;
            }
            if (j < 0)
            {
                ++i;
                result.push_back(i);
                i += 2 * m - 2;
                j = m - 1;
            }
        }
        return result;
    }
}
