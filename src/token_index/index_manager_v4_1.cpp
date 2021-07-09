#include "token_index/index_manager_v4_1.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "common/scope_exit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace ti
{
    index_manager_v4_1::index_manager_v4_1()
        : _inverted_index{} {}

    void
    index_manager_v4_1::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        SCOPE_GUARD
        {
            ifs.close();
        };
        str_t doc_line;
        while (getline(ifs, doc_line))
        {
            std::transform(std::begin(doc_line), std::end(doc_line), std::begin(doc_line), tolower);
            push_doc_line(doc_line);
        }
    }

    void
    index_manager_v4_1::push_doc_line(const str_t &doc_line)
    {
        str_t token;
        std::istringstream iss{doc_line};
        iss >> token;
        doc_id_t doc_id{stoul(token)};
        str_t new_line{doc_line.substr(token.size() + 1)};

        position_t position{0};
        str_idx_t begin, end;
        bool is_find_begin{false};
        for (str_idx_t i{0}; i < new_line.size(); ++i)
        {
            const ch_t &ch = new_line[i];
            if (ch != ' ')
            {
                if (!is_find_begin)
                {
                    is_find_begin = true;
                    begin = i;
                }
                end = i;
            }
            else
            {
                if (is_find_begin)
                {
                    is_find_begin = false;
                    token = new_line.substr(begin, end - begin + 1);

                    auto &doc_id_position_offset_vec{_inverted_index[token]};

                    doc_id_position_offset_vec.emplace_back(
                        doc_id_position_offset_t{
                            doc_id,
                            position++,
                            offset_t{
                                begin,
                                end}});
                }
            }
        }
    }

    bool
    index_manager_v4_1::del_doc_by_id(const doc_id_t &doc_id)
    {
        bool has_deleted{false};
        for (auto inverted_index_iter{std::begin(_inverted_index)};
             inverted_index_iter != std::end(_inverted_index);)
        {
            auto &doc_id_position_offset_vec{inverted_index_iter->second};
            for (auto doc_id_position_offset_vec_iter{std::begin(doc_id_position_offset_vec)};
                 doc_id_position_offset_vec_iter != std::end(doc_id_position_offset_vec);)
            {
                if (doc_id_position_offset_vec_iter->doc_id != doc_id)
                {
                    ++doc_id_position_offset_vec_iter;
                    continue;
                }
                has_deleted = true;
                doc_id_position_offset_vec_iter = doc_id_position_offset_vec.erase(doc_id_position_offset_vec_iter);
            }
            if (!doc_id_position_offset_vec.empty())
            {
                ++inverted_index_iter;
                continue;
            }
            inverted_index_iter = _inverted_index.erase(inverted_index_iter);
        }
        return has_deleted;
    }

    void
    index_manager_v4_1::print_inverted_index() const noexcept
    {
        std::cout << _inverted_index << std::endl;
    }

    frequency_t
    index_manager_v4_1::calc_frequency(const str_t &token) const noexcept
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
            return 0;
        return inverted_index_iter->second.size();
    }

    result_union_set_t
    index_manager_v4_1::retrieve_union(const query_t &query) const noexcept
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                continue;

            for (const auto &doc_id_position_offset : inverted_index_iter->second)
                union_set.insert(doc_id_position_offset.doc_id);
        }
        return union_set;
    }

    result_intersection_set_t
    index_manager_v4_1::retrieve_intersection(const query_t &query, const str_t &) const noexcept
    {
        const auto &first_token = query[0];
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        auto intersection_doc_id_position_offset_vec{intersection_inverted_index_iter->second};

        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {};
            const auto &doc_id_position_offset_vec{inverted_index_iter->second};

            std::size_t quick{0}, slow{0};
            for (; quick < intersection_doc_id_position_offset_vec.size(); ++quick)
            {
                const auto &intersection_doc_id_position_offset = intersection_doc_id_position_offset_vec[quick];
                const auto &doc_id = intersection_doc_id_position_offset.doc_id;
                const auto &position = intersection_doc_id_position_offset.position;
                for (const auto &doc_id_position_offset : doc_id_position_offset_vec)
                {
                    if (doc_id == doc_id_position_offset.doc_id && position + i == doc_id_position_offset.position)
                    {
                        auto &temp_doc_id_position_offset = intersection_doc_id_position_offset_vec[slow];
                        temp_doc_id_position_offset.doc_id = doc_id;
                        temp_doc_id_position_offset.position = position;
                        temp_doc_id_position_offset.offset.begin = intersection_doc_id_position_offset.offset.begin;
                        temp_doc_id_position_offset.offset.end = doc_id_position_offset.offset.end;
                        ++slow;
                        break;
                    }
                }
            }
            if (slow == 0)
                return {};
            else
                intersection_doc_id_position_offset_vec.resize(slow);
        }
        return to_result_intersection_set_t(std::move(intersection_doc_id_position_offset_vec));
    }

    result_regex_t
    index_manager_v4_1::retrieve_regex(const std::regex &pattern) const noexcept
    {
        result_regex_t result_regex;
        std::smatch result;
        for (const auto &[token, doc_id_position_offset_vec]: _inverted_index)
            if (regex_match(token, result, pattern))
                for (const auto &doc_id_position_offset : doc_id_position_offset_vec)
                    result_regex.emplace(doc_id_position_offset.doc_id);
        return result_regex;
    }
}