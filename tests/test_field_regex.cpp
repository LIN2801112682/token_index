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

void test_build_index(const ti::path_t &field_dir, const std::string &field)
{
    auto begin_time = std::chrono::high_resolution_clock::now();
    ti::field_manager manager{};
    manager.push_field_dir(field_dir);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    auto program_times = elapsed_time.count();
    std::cout << "Build time: " << program_times << std::endl;
    
    ti::str_t doc_line{"999999 neu"};
    begin_time = std::chrono::high_resolution_clock::now();
    manager.push_doc_line(field, doc_line);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Push time: " << program_times << std::endl;

    ti::doc_id_t doc_id{1};
    begin_time = std::chrono::high_resolution_clock::now();
    bool has_deleted{manager.del_doc_by_id(field, doc_id)};
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Delete time: " << program_times << " has_deleted: " << std::boolalpha << has_deleted << std::endl;

    doc_id = 999999;
    doc_line = "999999 happy";
    begin_time = std::chrono::high_resolution_clock::now();
    bool has_changed{manager.del_doc_by_id(field, doc_id)};
    if (has_changed)
        manager.push_doc_line(field, doc_line);
    end_time = std::chrono::high_resolution_clock::now();
    elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - begin_time);
    program_times = elapsed_time.count();
    std::cout << "Change time: " << program_times << " has_changed: " << std::boolalpha << has_changed << std::endl;
}

static const ti::path_t field{"query.txt"};
static const ti::path_t field_dir{"../resources/field_dir"};
static const ti::path_t regex_path{"../resources/regex.txt"};
static const ti::path_t regex_result_path{"../resources/regex_result.txt"};

int main()
{
    test_regex_group(field_dir, field, regex_path, regex_result_path);
    test_build_index(field_dir, field);
    return 0;
}