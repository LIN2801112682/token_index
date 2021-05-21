#include "token_index/ti_trie_node.h"

namespace ti
{
    ti_trie_node::ti_trie_node(const char &key, std::string &&value)
        : la::trie_node(key, std::move(value)) {}
}