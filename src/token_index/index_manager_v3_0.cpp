#include "token_index/index_manager_v3_0.h"
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
    index_manager_v3_0::index_manager_v3_0()
        : _inverted_index{} {}

    void
    index_manager_v3_0::push_col_file(const path_t &col_file_path)
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
    index_manager_v3_0::push_doc_line(const str_t &doc_line)
    {
        auto [doc_id, doc, _] = line_to_doc(doc_line);

        for (position_t position{0}; position < doc.size(); ++position)
        {
            const auto &token{doc[position]};
            auto &doc_id_map{_inverted_index[token]};
            doc_id_map[doc_id].emplace(position);
        }
    }

    void
    index_manager_v3_0::print_inverted_index() const noexcept
    {
        std::cout << _inverted_index << std::endl;
    }

    frequency_t
    index_manager_v3_0::calc_frequency(const str_t &token) const noexcept
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
           return 0;

        frequency_t frequency{0};
        for (const auto & doc_id_umap_pair : inverted_index_iter->second)
            frequency += doc_id_umap_pair.second.size();
        return frequency;
    }

    result_union_set_t
    index_manager_v3_0::retrieve_union(const query_t &query) const noexcept
    {
        result_union_set_t union_set{};
        for (const auto &token : query)
        {
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                continue;

            for (const auto &doc_id_umap_pair : inverted_index_iter->second)
                union_set.insert(doc_id_umap_pair.first);
        }
        return union_set;
    }

    result_intersection_set_t
    index_manager_v3_0::retrieve_intersection(const query_t &query, const str_t &) const noexcept
    {
        const auto &first_token = query[0];
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        if (std::end(_inverted_index) == intersection_inverted_index_iter)
            return {};
        auto intersection_doc_id_umap{intersection_inverted_index_iter->second};

        for (query_t::size_type i{1}; i < query.size(); ++i)
        {
            const auto &token = query[i];
            const auto &inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                return {};
            const auto &doc_id_umap{inverted_index_iter->second};

            for (auto intersection_doc_id_umap_iter = std::begin(intersection_doc_id_umap);
                 intersection_doc_id_umap_iter != std::end(intersection_doc_id_umap);)
            {
                const auto &doc_id = intersection_doc_id_umap_iter->first;
                const auto &doc_id_umap_iter = doc_id_umap.find(doc_id);
                if (std::end(doc_id_umap) == doc_id_umap_iter)
                {
                    intersection_doc_id_umap_iter = intersection_doc_id_umap.erase(intersection_doc_id_umap_iter);
                    continue;
                }
                auto &intersection_position_uset{intersection_doc_id_umap_iter->second};
                const auto &position_uset{doc_id_umap_iter->second};

                for (auto intersection_position_uset_iter = std::begin(intersection_position_uset);
                     intersection_position_uset_iter != std::end(intersection_position_uset);)
                {
                    const auto &position = *intersection_position_uset_iter;
                    if (std::end(position_uset) == position_uset.find(position + i))
                    {
                        intersection_position_uset_iter = intersection_position_uset.erase(intersection_position_uset_iter);
                        continue;
                    }
                    ++intersection_position_uset_iter;
                }
                ++intersection_doc_id_umap_iter;
            }
            if (intersection_doc_id_umap.empty())
                return {};
        }
        return to_result_intersection_set_t(std::move(intersection_doc_id_umap));
    }

    result_regex_t
    index_manager_v3_0::retrieve_regex(const std::regex &pattern) const noexcept
    {
        result_regex_t result_regex;
        std::smatch result;
        for (const auto &[token, _] : _inverted_index)
            if (regex_match(token, result, pattern))
                result_regex.emplace_back(token);
        return result_regex;
    }
}