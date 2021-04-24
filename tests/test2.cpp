#include "token_index/header.h"
#include "token_index/token_manager.h"

int dist(const char t[], const char ch)
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

std::vector<int> BM(const char s[], const char t[])
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

static const token_index::path_t file_path{"../resource/depattern_doc.txt"};
static const token_index::path_t query_path{"../resource/query.txt"};

int main()
{
    std::ifstream ifs;
    token_index::line_t line;

    token_index::index_manager manager;
    manager.push_file(file_path);

    token_index::line_vec_t documents;
    ifs.open(file_path, std::ifstream::in);
    while (getline(ifs, line))
        documents.push_back(line);
    ifs.close();

    token_index::query_vec_t query_vec = token_index::index_manager::load_query(query_path);
    
    token_index::line_vec_t querys;
    ifs.open(query_path, std::ifstream::in);
    while (getline(ifs, line))
        querys.push_back(line);
    ifs.close();

    for (std::size_t i = 0; i < query_vec.size(); ++i)
    {
        token_index::query_t query = query_vec[i];
        token_index::line_t query_line = querys[i];
        token_index::index_set_t intersection_set = manager.retrieve_intersection(query);
        for (const auto &token : query)
        {
            token_index::index_set_t index_set = manager.retrieve(token);
            if (index_set.size() > 1000)
                continue;
            //std::cout << token << std::endl;
            for (const auto &index : index_set)
            {
                token_index::line_t document_line = documents[index]; 
                auto result = BM(document_line.c_str(), query_line.c_str());
                if (intersection_set.find(index) != std::end(intersection_set))
                {
                    std::cout << "yes:" << std::endl;
                    std::cout << "  token:" << token << "," << std::endl;
                    std::cout << "  query:" << query_line << "," << std::endl;
                    std::cout << "  document:" << document_line << "," << std::endl;
                    std::cout << "  BM:";
                    for (const auto &num : result)
                        std::cout << num << ',';
                    std::cout << std::endl;
                }
            }
        }
    }

}