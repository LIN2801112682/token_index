#pragma once

#include "token_index/types.h"

#include <string>
#include <array>
#include <iostream>

namespace la
{
    class trie_node
    {
    private:
        trie_node(const trie_node &) = delete;
        trie_node(trie_node &&) = delete;
        trie_node &operator=(const trie_node &) = delete;
        trie_node &operator=(trie_node &&) = delete;

    public:
        static constexpr int ALPHABET_SIZE{40};
        trie_node(const char &key, std::string &&value);
        static std::size_t convertIndex(const char &c);

        std::array<trie_node *, ALPHABET_SIZE> _children;
        bool _is_end_word;
        char _key;
        std::string _value;
        ti::doc_id_map_t _doc_id_map;
    };

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
    };
}

std::ostream &operator<<(std::ostream &os, const la::trie_node *tn);
std::ostream &operator<<(std::ostream &os, const la::trie_tree &tt);