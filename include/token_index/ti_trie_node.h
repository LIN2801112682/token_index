#pragma once

#include "levenshtein_automaton/trie_node.h"
#include "token_index/types.h"

namespace ti
{
    class ti_trie_node
        : public la::trie_node
    {
    public:
        ti_trie_node(const char &key, std::string &&value);
        ~ti_trie_node() = default;
        doc_id_map_t doc_id_map;
    };
}