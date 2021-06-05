#include "token_index/index_manager_v4_0_2.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "levenshtein_automaton/LevenshteinNFA.h"
#include "levenshtein_automaton/LevenshteinDFA.h"
#include "bm/bm.h"
#include "common/scope_exit.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace ti
{
    index_manager_v4_0_2::index_manager_v4_0_2()
        : _inverted_index{} {}

    void
    index_manager_v4_0_2::push_col_file(const path_t &col_file_path)
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
    index_manager_v4_0_2::push_doc_line(const str_t &doc_line)
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
            else
            {
                if (is_find_begin)
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
    }

    void
    index_manager_v4_0_2::print_inverted_index() const noexcept
    {
        std::cout << _inverted_index << std::endl;
    }

    static constexpr int maxDist = 2;

    std::vector<la::trie_node *>
    index_manager_v4_0_2::search(const str_t &token) const noexcept
    {
        std::vector<la::trie_node *> result;
        auto nfa{la::LevenshteinNFA::ConstructNFA(token, maxDist)};
        auto dfa{la::LevenshteinDFA::SubsetConstruct(nfa)};
        dfa.Search(_inverted_index, dfa._start, _inverted_index._root_node, result);
        return result;
    }

    frequency_t
    index_manager_v4_0_2::calc_frequency(const str_t &token) const noexcept
    {
        frequency_t frequency{0};
        auto search_result{search(token)};
        for (const auto &node : search_result)
            for (const auto &doc_id_map_pair : node->_doc_id_map)
                frequency += doc_id_map_pair.second.size();
        return frequency;
    }

    result_union_set_t
    index_manager_v4_0_2::retrieve_union(const query_t &query) const noexcept
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            auto search_result{search(token)};
            for (const auto &node : search_result)
                for (const auto &doc_id_map_pair : node->_doc_id_map)
                    union_set.insert(doc_id_map_pair.first);
        }
        return union_set;
    }

    result_union_set_t
    index_manager_v4_0_2::retrieve_union_test(const query_t &query) const noexcept
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            std::vector<la::trie_node *> search_result;
            auto nfa{la::LevenshteinNFA::ConstructNFA(token, maxDist)};
            auto dfa{la::LevenshteinDFA::SubsetConstruct(nfa)};
            dfa.Search(_inverted_index, dfa._start, _inverted_index._root_node, search_result);

            for (const auto &node : search_result)
                for (const auto &doc_id_map_pair : node->_doc_id_map)
                    union_set.insert(doc_id_map_pair.first);
        }
        return union_set;
    }

    result_union_set_with_position_t
    index_manager_v4_0_2::retrieve_union_test_with_position(const query_t &query) const noexcept
    {
        result_union_set_with_position_t union_set{};
        for (const auto &token : query)
        {
            std::vector<la::trie_node *> search_result;
            auto nfa{la::LevenshteinNFA::ConstructNFA(token, maxDist)};
            auto dfa{la::LevenshteinDFA::SubsetConstruct(nfa)};
            dfa.Search(_inverted_index, dfa._start, _inverted_index._root_node, search_result);

            for (const auto &node : search_result)
                for (const auto &doc_id_map_pair : node->_doc_id_map)
                {
                    auto &position_set{union_set[doc_id_map_pair.first]};
                    for (const auto &position_offset : doc_id_map_pair.second)
                        position_set.emplace(position_offset.position);
                }
        }
        return union_set;
    }

    std::vector<token_relative_position_frequency_t>
    index_manager_v4_0_2::gen_token_relative_position_frequency_vec(const query_t &query) const noexcept
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
                  [](const auto &lhs, const auto &rhs)
                  {
                      if (lhs.frequency < rhs.frequency)
                          return true;
                      if (lhs.relative_position < rhs.relative_position)
                          return true;
                      return false;
                  });
        return token_relative_position_frequency_vec;
    }

    result_intersection_set_t
    index_manager_v4_0_2::not_low_frequency_retrieve_intersection(
        std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec) const noexcept
    {
        return {};
        /* has bug, need fix;
        const auto &first_token_relative_position_frequency = token_relative_position_frequency_vec[0];
        const auto &first_token = first_token_relative_position_frequency.token;
        const auto &first_relative_position = first_token_relative_position_frequency.relative_position;

        doc_id_map_t intersection_doc_id_map{};
        auto first_search_result{search(first_token)};
        for (const auto &node : first_search_result)
            for (const auto &doc_id_map_pair : node->_doc_id_map)
            {
                auto &intersection_position_offset_vec{intersection_doc_id_map[doc_id_map_pair.first]};
                for (const auto &position_offset : doc_id_map_pair.second)
                    intersection_position_offset_vec.emplace_back(position_offset);
            }

       for (std::size_t i{1}; i < token_relative_position_frequency_vec.size(); ++i)
        {
            const auto &token_relative_position_frequency = token_relative_position_frequency_vec[i];
            const auto &token = token_relative_position_frequency.token;
            const auto &relative_position = token_relative_position_frequency.relative_position;

            doc_id_map_t doc_id_map{};
            const auto &search_result{search(token)};
            for (const auto &node : search_result)
                for (const auto &doc_id_map_pair : node->_doc_id_map)
                {
                    auto &position_offset_vec{intersection_doc_id_map[doc_id_map_pair.first]};
                    for (const auto &position_offset : doc_id_map_pair.second)
                        position_offset_vec.emplace_back(position_offset);
                }

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
        */
    }

    result_intersection_set_t
    index_manager_v4_0_2::low_frequency_retrieve_intersection(
        std::vector<token_relative_position_frequency_t> &&token_relative_position_frequency_vec, const str_t &query_line) const noexcept
    {
        const auto &first_token_relative_position_frequency = token_relative_position_frequency_vec[0];
        const auto &first_token = first_token_relative_position_frequency.token;
        const auto &first_relative_position = first_token_relative_position_frequency.relative_position;

        std::unordered_set<doc_id_t> intersection_doc_id_set{};
        auto first_search_result{search(first_token)};
        for (const auto &node : first_search_result)
            for (const auto &pair : node->_doc_id_map)
                intersection_doc_id_set.emplace(pair.first);

        for (std::size_t i{1}; i < token_relative_position_frequency_vec.size(); ++i)
        {
            const auto &token_relative_position_frequency = token_relative_position_frequency_vec[i];
            const auto &token = token_relative_position_frequency.token;

            decltype(intersection_doc_id_set) doc_id_set{};
            auto search_result{search(token)};
            for (const auto &node : search_result)
                for (const auto &pair : node->_doc_id_map)
                    doc_id_set.emplace(pair.first);

            for (auto intersection_doc_id_set_iter{std::begin(intersection_doc_id_set)};
                 intersection_doc_id_set_iter != std::end(intersection_doc_id_set);)
            {
                const auto &doc_id{*intersection_doc_id_set_iter};
                if (doc_id_set.count(doc_id) == 0)
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
    index_manager_v4_0_2::retrieve_intersection(const query_t &query, const str_t &query_line) const noexcept
    {
        std::vector<token_relative_position_frequency_t> token_relative_position_frequency_vec{
            gen_token_relative_position_frequency_vec(query)};
        if (token_relative_position_frequency_vec[0].frequency <= low_frequency)
            return low_frequency_retrieve_intersection(std::move(token_relative_position_frequency_vec), query_line);
        else
            return not_low_frequency_retrieve_intersection(std::move(token_relative_position_frequency_vec));
    }

    result_regex_t
    index_manager_v4_0_2::retrieve_regex(const std::regex &pattern) const noexcept
    {
        std::smatch result;
        return _inverted_index.dfs([&result, &pattern](const std::string &token){
            return regex_match(token, result, pattern);
        });
    }
}