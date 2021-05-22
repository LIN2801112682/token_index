#include "levenshtein_automaton/trie_node.h"
#include <cassert>

namespace la
{
    trie_node::trie_node(const char &key, std::string &&value)
        : _key{key},
        _value{std::move(value)},
        _is_end_word{false},
        _children{nullptr} {}

    trie_node::~trie_node()
    {
        for (auto &&child : _children)
            if(child != nullptr)
                delete child;
    }
    
    std::size_t
    trie_node::convertIndex(const char &c)
    {
        if ('a' <= c && c <= 'z')
            return c - 'a';
        if ('0' <= c && c <= '9')
            return c - '0' + 26;
        if (c == '_')
            return 26 + 10 + 1;
        if (c == ',')
            return 26 + 10 + 2;
        if (c == '.')
            return 26 + 10 + 3;
        assert(false);
        return 0;
    }
}