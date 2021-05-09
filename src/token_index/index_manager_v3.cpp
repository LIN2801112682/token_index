#include "token_index/index_manager_v3.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>

namespace ti
{
    index_manager_v3::index_manager_v3()
        : _inverted_index{} {}

    void
    index_manager_v3::push_col_file(const path_t &col_file_path)
    {
        std::ifstream ifs{col_file_path, std::ifstream::in};
        str_t doc_line;
        while (getline(ifs, doc_line))
        {
            std::transform(std::begin(doc_line), std::end(doc_line), std::begin(doc_line), tolower);
            push_doc_line(doc_line);
        }
        ifs.close();
    }

    void
    index_manager_v3::push_doc_line(const str_t &doc_line)
    {
        auto [doc_id, doc, _] = line_to_doc(doc_line);

        for (position_t position{0}; position < doc.size(); ++position)
        {
            const auto &token = doc[position];

            if (_inverted_index.count(token) == 0)
                _inverted_index.emplace(token, doc_id_umap_t{});
            auto &doc_id_map = _inverted_index[token];

            if (doc_id_map.count(doc_id) == 0)
                doc_id_map.emplace(doc_id, position_uset_t{});
            doc_id_map[doc_id].emplace(position);
        }
    }

    void
    index_manager_v3::print_inverted_index() const
    {
        std::cout << _inverted_index;
    }

    const frequency_t
    index_manager_v3::calc_frequency(const str_t &token) const
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
           return 0;

        frequency_t frequency{0};
        for (const auto & doc_id_umap_pair : inverted_index_iter->second)
            frequency += doc_id_umap_pair.second.size();
        return frequency;
    }

    const result_union_set_t
    index_manager_v3::retrieve_union(const query_t &query) const //yitiao query
    {
        result_union_set_t union_set{};
        for (const auto &token : query)// yitiao query zhongde every token
        {
            auto inverted_index_iter = _inverted_index.find(token);
            if (std::end(_inverted_index) == inverted_index_iter)
                continue;

            for (const auto &doc_id_umap_pair : inverted_index_iter->second)
                union_set.insert(doc_id_umap_pair.first);
        }
        return union_set;
    }

    const result_intersection_set_t
    index_manager_v3::retrieve_intersection(const query_t &query) const //yitiao query:the fox did
    {
        const auto &first_token = query[0];
        const auto &intersection_inverted_index_iter = _inverted_index.find(first_token);
        const auto &intersection_inverted_index_iter_2 = intersection_inverted_index_iter->second;//
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
            //decltype(intersection_doc_id_umap) temp_doc_id_umap{};//copy

            /*for (const auto &intersection_doc_id_umap_pair : intersection_doc_id_umap)
            {
                const auto &doc_id = intersection_doc_id_umap_pair.first;
                const auto &doc_id_umap_iter = doc_id_umap.find(doc_id);
                if (std::end(doc_id_umap) == doc_id_umap_iter)
                    continue;

                const auto &intersection_position_uset = intersection_doc_id_umap_pair.second;
                const auto &position_uset = doc_id_umap_iter->second;
                for (const auto &position : intersection_position_uset)
                {
                    if (position_uset.find(position + i) == std::end(position_uset))
                        continue;

                    if (temp_doc_id_umap.count(doc_id) == 0)
                        temp_doc_id_umap.emplace(doc_id, position_uset_t{});
                    temp_doc_id_umap[doc_id].emplace(position);
                }
            }
            if (temp_doc_id_umap.empty())
                return {};*/
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

                auto &intersection_position_uset = intersection_doc_id_umap_iter->second;
                const auto &position_uset = doc_id_umap_iter->second;
                for (auto intersection_position_uset_iter = std::begin(intersection_position_uset);
                     intersection_position_uset_iter != std::end(intersection_position_uset);)
                {
                    const auto &position = *intersection_position_uset_iter;
                    if (position_uset.find(position + i)
                        == std::end(position_uset))
                    {
                        intersection_position_uset_iter = intersection_position_uset.erase(intersection_position_uset_iter);
                        continue;
                    }
                    ++intersection_position_uset_iter;
                }
                ++intersection_doc_id_umap_iter;//
            }


            if (intersection_doc_id_umap.empty())
                return {};
        }
        return to_result_intersection_set_t(intersection_doc_id_umap);
    }
}