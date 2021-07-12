#include "token_index/types.h"
#include "token_index/common.h"
#include "token_index/field_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

void test_regex(const ti::field_manager &manager, const std::string &field, const std::vector<std::regex> &regex_vec,
                const bool &is_out, std::ostream &os = std::cout)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    for (const auto &pattern : regex_vec)
    {
        auto begin_time = std::chrono::high_resolution_clock::now();
        const auto regex_result = manager.retrieve_field_regex(field, pattern);
        if (is_out)
        {
            os << pattern.flags() << " : " << regex_result << std::endl;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
        auto program_times = elapsed_time.count();
        std::cout << "      Location time: " << program_times
                  << ", Result doc size: " << regex_result.size() << std::endl;
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Location time: " << program_times << std::endl;
    double avg_time;
    if (regex_vec.empty())
        avg_time = 0.0;
    else
        avg_time = double(program_times) / double(regex_vec.size());
    std::cout << "Average location time: " << avg_time << std::endl;
}

void test_regex_group(const ti::path_t &field_dir, const std::string &field, const ti::path_t &regex_path,
                const ti::path_t &regex_result_path)
{
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    // manager.print_field_inverted_index(field);
    auto regex_vec = ti::load_regex_vec(regex_path);
    std::ofstream ofs;

    std::cout << "field: " << field << ", regex, cout ----------" << std::endl;
    test_regex(manager, field, regex_vec, false);

    std::cout << "field: " << field << ", regex, ofs ----------" << std::endl;
    ofs.open(regex_result_path, std::ofstream::out);
    test_regex(manager, field, regex_vec, true, ofs);
    ofs.close();
}

static const ti::path_t field{"query.txt"};
static const ti::path_t field_dir{"../resources/field_dir"};
static const ti::path_t regex_path{"../resources/regex.txt"};
static const ti::path_t regex_result_path{"../resources/regex_result.txt"};

int main()
{
    test_regex_group(field_dir, field, regex_path, regex_result_path);
    return 0;
}