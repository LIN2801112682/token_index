#include "levenshtein_automaton/trie_tree.h"

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


    trie_tree::trie_tree()
        : _root_node{new trie_node{' ', ""}} {}

    trie_tree::~trie_tree()
    {
        delete _root_node;
    }

    void trie_tree::insert(std::string &&key)
    {
        auto cur_node{_root_node};
        for (decltype(key.size()) i{0}; i < key.size(); ++i)
        {
            const auto ci{trie_node::convertIndex(key[i])};
            if (cur_node->_children[ci] == nullptr)
                cur_node->_children[ci] = new trie_node{key[i], key.substr(0, i + 1)};
            cur_node = cur_node->_children[ci];
        }
        cur_node->_is_end_word = true;
    }

    bool trie_tree::search(const std::string &key)
    {
        auto cur_node{_root_node};
        for (decltype(key.size()) i{0}; i < key.size(); ++i)
        {
            const auto ci{trie_node::convertIndex(key[i])};
            if (cur_node->_children[ci] == nullptr)
                return false;
            cur_node = cur_node->_children[ci];
        }
        return (cur_node != nullptr && cur_node->_is_end_word);
    }

    ti::doc_id_map_t& 
    trie_tree::operator[](const std::string& key)
    {
        auto cur_node{_root_node};
        for (decltype(key.size()) i{0}; i < key.size(); ++i)
        {
            const auto ci{trie_node::convertIndex(key[i])};
            if (cur_node->_children[ci] == nullptr)
                cur_node->_children[ci] = new trie_node{key[i], key.substr(0, i + 1)};
            cur_node = cur_node->_children[ci];
        }
        cur_node->_is_end_word = true;
        return cur_node->_doc_id_map;
    }
}

std::ostream &operator<<(std::ostream &os, const la::trie_node *tn)
{
    if (tn->_is_end_word)
        os << tn->_value << std::endl;
    for (const auto &child : tn->_children)
        if (child != nullptr)
            os << child;
    return os;
}

std::ostream &operator<<(std::ostream &os, const la::trie_tree &tn)
{
    os << tn._root_node;
    return os;
}