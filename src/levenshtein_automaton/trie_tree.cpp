#include "levenshtein_automaton/trie_tree.h"
#include "token_index/types.h"
#include <stack>

namespace la
{
    trie_node::trie_node(const char &key, std::string &&value)
        : _key{key},
        _value{std::move(value)},
        _is_end_word{false},
        _children{nullptr} {}
    
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
        std::stack<la::trie_node *> s{};
        s.push(_root_node);
        while (!s.empty())
        {
            la::trie_node *cur_node{s.top()};
            s.pop();
            for (const auto &child : cur_node->_children)
                if (child != nullptr)
                    s.push(child);
            delete cur_node;
        }
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

    std::vector<std::string>
    trie_tree::dfs(std::function<bool(const std::string &)> func) const
    {
        std::vector<std::string> result;
        std::stack<la::trie_node *> s{};
        s.push(_root_node);
        while (!s.empty())
        {
            la::trie_node *cur_node{s.top()};
            s.pop();
            if (func(cur_node->_value))
                result.emplace_back(cur_node->_value);
            for (const auto &child : cur_node->_children)
                if (child != nullptr)
                    s.push(child);
        }
        return result;
    }
}

std::ostream &operator<<(std::ostream &os, const la::trie_node *tn)
{
    if (tn->_is_end_word)
        // os << " {" << tn->_value << " : " << tn->_doc_id_map << "}," << std::endl;
        os << " {" << tn->_value << " : " << "}," << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const la::trie_tree &tt)
{
    os << "[" << std::endl;
    std::stack<la::trie_node *> s{};
    s.push(tt._root_node);
    while (!s.empty())
    {
        la::trie_node *cur_node{s.top()};
        s.pop();
        os << cur_node;
        for (const auto &child : cur_node->_children)
            if (child != nullptr)
                s.push(child);
    }
    os << "]";
    return os;
}