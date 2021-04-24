#include <fstream>
#include <string>
#include <algorithm>

int main()
{
    std::ifstream ifs{"../resource/depattern_doc-origin.txt", std::ifstream::in};
    std::ofstream ofs{"../resource/depattern_doc.txt", std::ofstream::out};
    std::string line;
    while (getline(ifs, line))
    {
        replace_if(
            std::begin(line), std::end(line),
            [](char c) {
                if (isalnum(c))
                    return false;
                switch (c)
                {
                case '_':
                case '>':
                case '<':
                case '=':
                case ' ':
                case '\n':
                    return false;
                default:
                    return true;
                }
            },
            ' ');
        ofs << line << '\n';
    }
    ifs.close();
    ofs.close();
    return 0;
}