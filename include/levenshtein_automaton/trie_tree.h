#pragma once

#include "levenshtein_automaton/trie_node.h"

#include <string>
#include <array>

namespace la
{
    extern int START;

    class trie_tree
    {
    private:
        trie_tree(const trie_tree &) = delete;
        trie_tree(trie_tree &&) = delete;
        trie_tree &operator=(const trie_tree &) = delete;
        trie_tree &operator=(trie_tree &&) = delete;

    public:
        trie_tree(trie_node *root_node);
        trie_tree();
        ~trie_tree();
        trie_node *_root_node;
        void insert(std::string &&key);
        bool search(const std::string &key);
    };
}