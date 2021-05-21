#include "token_index/ti_trie_tree.h"

namespace ti
{
    ti_trie_tree::ti_trie_tree()
        : la::trie_tree{dynamic_cast<la::trie_node *>(new ti_trie_node{' ', ""})} {}
}