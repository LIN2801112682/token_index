#include "bm/bm.h"
#include <cstring>
#include <algorithm>
#include <vector>
#include <string>

#define MAX_CHAR_LEN 256

namespace bm
{

    int calcBC(const char *subdir, int bc[])
    {
        int i = 0;
        int len = strlen(subdir);

        for (i = 0; i < MAX_CHAR_LEN; i++)
        {
            bc[i] = len;
        }
        for (i = 0; i < len - 1; i++)
        {
            bc[subdir[i]] = len - 1 - i;
        }

        return 0;
    }

    int calcSuffix(const char *subdir, int suffix[])
    {
        int i = 0;
        int j = 0;
        int k = 0;
        int count = 0;
        int len = strlen(subdir);

        suffix[len - 1] = len;
        for (i = len - 2; i >= 0; i--)
        {
            j = i;
            k = len - 1;
            count = 0;
            while (subdir[j--] == subdir[k--])
            {
                count++;
            }
            suffix[i] = count;
        }

        return 0;
    }

    int calcGS(const char *subdir, int gs[])
    {
        int i = 0;
        int j = 0;
        int len = strlen(subdir);

        int *suffix = (int *)malloc(sizeof(int) * len);
        if (!suffix)
            return 0;

        calcSuffix(subdir, suffix);

        for (i = 0; i < len; i++)
        {
            gs[i] = len;
        }

        j = 0;
        for (i = len - 2; i >= 0; --i)
        {
            if (suffix[i] == i + 1)
            {
                for (; j < len - 1; j++)
                {
                    if (gs[j] == len)
                    {
                        gs[j] = len - 1 - i;
                    }
                }
            }
        }

        for (i = 0; i <= len - 2; i++)
        {
            gs[len - 1 - suffix[i]] = len - 1 - i;
        }

        if (suffix)
        {
            free(suffix);
            suffix = NULL;
        }

        return 0;
    }

    std::vector<std::size_t> BoyerMoore(const std::string &doc_line, const std::string &query_line)
    {
        const char *szText = doc_line.c_str();
        const char *szSubstr = query_line.c_str();
        int nTextLen = doc_line.size();
        int nStrLen = query_line.size();

        std::vector<std::size_t> result{};

        int i = 0;
        int j = 0;

        int *bc = (int *)malloc(sizeof(int) * MAX_CHAR_LEN);
        int *gs = (int *)malloc(sizeof(int) * nStrLen);

        calcBC(szSubstr, bc);
        calcGS(szSubstr, gs);

        while (j <= nTextLen - nStrLen)
        {
            for (i = nStrLen - 1; i >= 0 && szSubstr[i] == szText[j + i]; i--)
                ;

            if (i < 0)
            {
                result.push_back(static_cast<size_t>(j));
                j += gs[0];
            }
            else
            {
                j += std::max(bc[szText[j + i]] - nStrLen + 1 + i, gs[i]);
            }
        }

        return result;
    }
}