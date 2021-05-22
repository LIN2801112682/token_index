#pragma once

#include "levenshtein_automaton/trie_node.h"
#include "token_index/types.h"

#include <string>
#include <array>

namespace la
{
    class trie_tree
    {
    private:
        trie_tree(const trie_tree &) = delete;
        trie_tree(trie_tree &&) = delete;
        trie_tree &operator=(const trie_tree &) = delete;
        trie_tree &operator=(trie_tree &&) = delete;

    public:
        trie_tree();
        ~trie_tree();
        trie_node *_root_node;
        void insert(std::string &&key);
        bool search(const std::string &key);
        ti::doc_id_map_t& operator[](const std::string& key);
        ti::doc_id_map_t& operator[](std::string&& key);
    };
}