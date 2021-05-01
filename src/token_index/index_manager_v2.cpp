#include "token_index/index_manager_v2.h"
#include "token_index/types.h"
#include "token_index/common.h"
#include "bm/bm.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

namespace ti
{
    index_manager_v2::index_manager_v2()
        : _inverted_index{} {}

    void
    index_manager_v2::push_col_file(const path_t &col_file_path)
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
    index_manager_v2::push_doc_line(const str_t &doc_line)
    {
        auto [doc_id, doc, new_doc_line] = line_to_doc_id_and_doc(doc_line);

        std::map<str_t, std::vector<position_t>> token_position_vec_map{};
        for (position_t position{0}; position < doc.size(); ++position)
        {
            const auto &token = doc[position];
            if (token_position_vec_map.count(token) == 0)
                token_position_vec_map.emplace(token, std::vector<position_t>{});
            token_position_vec_map[token].emplace_back(position);
        }

        for (const auto &token_position_vec_pair : token_position_vec_map)
        {
            const auto &token = token_position_vec_pair.first;
            const auto &offset_begin_vec = bm::BoyerMoore(new_doc_line, token);
            const auto &token_size = token.size();

            if (_inverted_index.count(token) == 0)
                _inverted_index.emplace(token, doc_id_position_offset_vec_t{});
            auto &doc_id_position_offset_vec = _inverted_index[token];

            for (std::size_t i{0}; i < token_position_vec_pair.second.size(); ++i)
            {
                doc_id_position_offset_vec.emplace_back(
                    doc_id_position_offset_t{
                        doc_id,
                        token_position_vec_pair.second[i],
                        offset_t{
                            offset_begin_vec[i],
                            offset_begin_vec[i] + token_size,
                        }});
            }
        }
    }

    void
    index_manager_v2::print_inverted_index() const
    {
        std::cout << _inverted_index;
    }

    const frequency_t
    index_manager_v2::calc_frequency(const str_t &token) const
    {
        auto inverted_index_iter = _inverted_index.find(token);
        if (std::end(_inverted_index) == inverted_index_iter)
            return 0;
        return inverted_index_iter->second.size();
    }

    const result_union_set_t
    index_manager_v2::retrieve_union(const query_t &query) const
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

    const result_intersection_set_t
    index_manager_v2::retrieve_intersection(const query_t &query) const
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
            decltype(intersection_doc_id_position_offset_vec) temp_doc_id_position_offset_vec{};

            for (const auto &intersection_doc_id_position_offset : intersection_doc_id_position_offset_vec)
            {
                const auto &doc_id = intersection_doc_id_position_offset.doc_id;
                const auto &position = intersection_doc_id_position_offset.position;
                for (const auto &doc_id_position_offset : doc_id_position_offset_vec)
                {
                    if (doc_id != doc_id_position_offset.doc_id)
                        continue;
                    if (position + i != doc_id_position_offset.position)
                        continue;
                    temp_doc_id_position_offset_vec.emplace_back(
                        doc_id_position_offset_t{
                            doc_id,
                            position,
                            offset_t{
                                intersection_doc_id_position_offset.offset.begin,
                                doc_id_position_offset.offset.end,
                            }});
                }
            }
            if (temp_doc_id_position_offset_vec.empty())
                return {};
            intersection_doc_id_position_offset_vec = temp_doc_id_position_offset_vec;
        }
        return intersection_doc_id_position_offset_vec;
    }
}