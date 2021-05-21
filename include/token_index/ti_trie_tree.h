#pragma once

#include "levenshtein_automaton/trie_tree.h"
#include "token_index/ti_trie_node.h"

namespace ti
{
    class ti_trie_tree
        : public la::trie_tree
    {
    public:
        ti_trie_tree();
        ~ti_trie_tree() = default;
    };
}