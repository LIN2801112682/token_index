#include "token_index/index_manager_v4_0_1.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include "common/scope_exit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace ti
{
    index_manager_v4_0_1::index_manager_v4_0_1()
        : _inverted_index{} {}

    void
    index_manager_v4_0_1::push_col_file(const path_t &col_file_path)
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
    index_manager_v4_0_1::push_doc_line(const str_t &doc_line)
    {
        str_t token;
        std::istringstream iss{doc_line};
        iss >> token;
        doc_id_t doc_id{stoul(token)};
        str_t new_line{doc_line.substr(token.size() + 1)};
        _doc_line_index.emplace(doc_id, new_line);

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
            if (ch == ' ' || i == new_line.size() - 1 && is_find_begin)
            {
                is_find_begin = false;
                token = new_line.substr(begin, end - begin + 1);

                auto &doc_id_map{_inverted_index[token]};
                auto &position_offset_vec{doc_id_map[doc_id]};

                position_offset_vec.emplace_back(
                        position_offset_t{
                                position++,
                                offset_t{
                                        begin,
                                        end,
                                }});
            }
        }
    }

    void
    index_manager_v4_0_1::push_doc_line_by_id(const doc_id_t &doc_id, const str_t &new_line)
    {
        str_t token;
        _doc_line_index.emplace(doc_id, new_line);

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
            if (ch == ' ' || i == new_line.size() - 1 && is_find_begin)
            {
                is_find_begin = false;
                token = new_line.substr(begin, end - begin + 1);

                auto &doc_id_map{_inverted_index[token]};
                auto &position_offset_vec{doc_id_map[doc_id]};

                position_offset_vec.emplace_back(
                        position_offset_t{
                                position++,
                                offset_t{
                                        begin,
                                        end,
                                }});
            }
        }
    }


    bool
    index_manager_v4_0_1::del_doc_by_id(const doc_id_t &doc_id)
    {
        bool has_deleted{false};
        for (auto inverted_index_iter{std::begin(_inverted_index)};
             inverted_index_iter != std::end(_inverted_index);)
        {
            auto &doc_id_map{inverted_index_iter->second};
            auto doc_id_map_iter = doc_id_map.find(doc_id);
            if (std::end(doc_id_map) == doc_id_map_iter)
            {
                ++inverted_index_iter;
                continue;
            }
            has_deleted = true;
            doc_id_map.erase(doc_id_map_iter);
            if (!doc_id_map.empty())
            {
                ++inverted_index_iter;
                continue;
            }
            inverted_index_iter = _inverted_index.erase(inverted_index_iter);
        }
        return has_deleted;
    }

    void
    index_manager_v4_0_1::print_inverted_index() const noexcept
    {
        std::cout << _inverted_index << std::endl;
    }

    frequency_t
    index_manager_v4_0_1::calc_frequency(const str_t &token) const noexcept
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
            return 0;

        frequency_t frequency{0};
        for (const auto &doc_id_map_pair : inverted_index_iter->second)
            frequency += doc_id_map_pair.second.size();
        return frequency;
    }

    result_union_set_t
    index_manager_v4_0_1::retrieve_union(const query_t &query) const noexcept
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                continue;

            for (const auto &doc_id_map_pair : inverted_index_iter->second)
                union_set.insert(doc_id_map_pair.first);
        }
        return union_set;
    }

    std::vector<token_relative_position_frequency_t>
    index_manager_v4_0_1::gen_token_relative_position_frequency_vec(const query_t &query) const noexcept
    {
        std::vector<token_relative_position_frequency_t> token_relative_position_frequency_vec;
        for (position_t relative_position{0}; relative_position < query.size(); ++relative_position)
        {
            const auto &token = query[relative_position];
            auto frequency = calc_frequency(token);
            token_relative_position_frequency_vec.emplace_back(
                    token_relative_position_frequency_t{
                            token, relative_position, frequency});
        }
        std::sort(std::begin(token_relative_position_frequency_vec),
                  std::end(token_relative_position_frequency_vec),
                  [](const auto &lhs, const auto &rhs) {
                      if (lhs.frequency < rhs.frequency)
                          return true;
                      if (lhs.relative_position < rhs.relative_position)
                          return true;
                      return false;
                  });
        return token_relative_position_frequency_vec;
    }

    result_intersection_set_t
    index_manager_v4_0_1::not_low_frequency_retrieve_intersection(
        std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec) const noexcept
    {
        const auto &first_token_relative_position_frequency = token_relative_position_frequency_vec[0];
        const auto &first_token = first_token_relative_position_frequency.token;
        const auto &first_relative_position = first_token_relative_position_frequency.relative_position;
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        auto intersection_doc_id_map{intersection_inverted_index_iter->second};

        for (std::size_t i{1}; i < token_relative_position_frequency_vec.size(); ++i)
        {
            const auto &token_relative_position_frequency = token_relative_position_frequency_vec[i];
            const auto &token = token_relative_position_frequency.token;
            const auto &relative_position = token_relative_position_frequency.relative_position;
            const auto &inverted_index_iter= _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {};
            const auto &doc_id_map{inverted_index_iter->second};

            for (auto intersection_doc_id_map_iter = std::begin(intersection_doc_id_map);
                 intersection_doc_id_map_iter != std::end(intersection_doc_id_map);)
            {
                const auto &doc_id = intersection_doc_id_map_iter->first;
                const auto &doc_id_map_iter = doc_id_map.find(doc_id);
                if (std::end(doc_id_map) == doc_id_map_iter)
                {
                    intersection_doc_id_map_iter = intersection_doc_id_map.erase(intersection_doc_id_map_iter);
                    continue;
                }
                auto &intersection_position_offset_vec{intersection_doc_id_map_iter->second};
                const auto &position_offset_vec{doc_id_map_iter->second};

                std::size_t quick{0}, slow{0};
                for (; quick < intersection_position_offset_vec.size(); ++quick)
                {
                    const auto &intersection_position_offset = intersection_position_offset_vec[quick];
                    const auto &position = intersection_position_offset.position;
                    for (const auto &position_offset : position_offset_vec)
                    {
                        if (position - first_relative_position + relative_position == position_offset.position)
                        {
                            auto &temp_position_offset = intersection_position_offset_vec[slow];
                            temp_position_offset.position = position;
                            if (intersection_position_offset.offset.begin < temp_position_offset.offset.begin)
                                temp_position_offset.offset.begin = intersection_position_offset.offset.begin;
                            if (intersection_position_offset.offset.end > temp_position_offset.offset.end)
                                temp_position_offset.offset.end = position_offset.offset.end;
                            ++slow;
                            break;
                        }
                    }
                }
                if (slow == 0)
                {
                    intersection_doc_id_map_iter = intersection_doc_id_map.erase(intersection_doc_id_map_iter);
                    continue;
                }
                else
                    intersection_position_offset_vec.resize(slow);
                ++intersection_doc_id_map_iter;
            }
            if (intersection_doc_id_map.empty())
                return {};
        }

        decltype(intersection_doc_id_map) temp_doc_id_map{};
        for (const auto &intersection_doc_id_map_pair : intersection_doc_id_map)
        {
            const auto &doc_id = intersection_doc_id_map_pair.first;
            auto &temp_position_offset_vec{temp_doc_id_map[doc_id]};

            const auto &intersection_position_offset_vec = intersection_doc_id_map_pair.second;
            for (const auto &position_offset : intersection_position_offset_vec)
                temp_position_offset_vec.emplace_back(position_offset_t{position_offset.position - first_relative_position, position_offset.offset});
        }
        intersection_doc_id_map = temp_doc_id_map;

        return to_result_intersection_set_t(std::move(intersection_doc_id_map));
    }

    result_intersection_set_t
    index_manager_v4_0_1::low_frequency_retrieve_intersection(
        std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec, const str_t &query_line) const noexcept
    {
        const auto &first_token_relative_position_frequency = token_relative_position_frequency_vec[0];
        const auto &first_token = first_token_relative_position_frequency.token;
        const auto &first_relative_position = first_token_relative_position_frequency.relative_position;
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        std::unordered_set<doc_id_t> intersection_doc_id_set{};
        for (const auto &pair : intersection_inverted_index_iter->second)
            intersection_doc_id_set.emplace(pair.first);

        for (std::size_t i{1}; i < token_relative_position_frequency_vec.size(); ++i)
        {
            const auto &token_relative_position_frequency = token_relative_position_frequency_vec[i];
            const auto &token = token_relative_position_frequency.token;
            const auto &inverted_index_iter= _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {};
            const auto &doc_id_map{inverted_index_iter->second};

            for (auto intersection_doc_id_set_iter{std::begin(intersection_doc_id_set)};
                 intersection_doc_id_set_iter != std::end(intersection_doc_id_set);)
            {
                const auto &doc_id{*intersection_doc_id_set_iter};
                if (doc_id_map.count(doc_id) == 0)
                {
                    intersection_doc_id_set_iter = intersection_doc_id_set.erase(intersection_doc_id_set_iter);
                    continue;
                }
                ++intersection_doc_id_set_iter;
            }
            if (intersection_doc_id_set.empty())
                return {};
        }

        result_intersection_set_t result_intersection_set{};
        auto query_line_size{query_line.size()};
        for (const doc_id_t &doc_id : intersection_doc_id_set)
        {
            const auto &doc_line{_doc_line_index.find(doc_id)->second};
            auto offset_begin_vec{bm::BoyerMoore(doc_line, query_line)};
            if (offset_begin_vec.empty())
                continue;
            auto &position_offset_vec{result_intersection_set[doc_id]};
            for (const auto &offset_begin : offset_begin_vec)
                position_offset_vec.emplace_back(
                    position_offset_t{
                        0,
                        offset_t{
                            offset_begin,
                            offset_begin + query_line_size,
                        },
                    });
        } 
        return result_intersection_set;
    }

    static constexpr frequency_t low_frequency{10};

    result_intersection_set_t
    index_manager_v4_0_1::retrieve_intersection(const query_t &query, const str_t &query_line) const noexcept
    {
        std::vector<token_relative_position_frequency_t> token_relative_position_frequency_vec{
            gen_token_relative_position_frequency_vec(query)
        };
        if (token_relative_position_frequency_vec[0].frequency <= low_frequency)
            return low_frequency_retrieve_intersection(std::move(token_relative_position_frequency_vec), query_line);
        else
            return not_low_frequency_retrieve_intersection(std::move(token_relative_position_frequency_vec));
    }

    result_regex_t
    index_manager_v4_0_1::retrieve_regex(const std::regex &pattern) const noexcept
    {
        result_regex_t result_regex;
        std::smatch result;
        for (const auto &[token, doc_id_map] : _inverted_index)
            if (regex_match(token, result, pattern))
                for (const auto &[doc_id, _] : doc_id_map)
                    result_regex.emplace(doc_id);
        return result_regex;
    }
}