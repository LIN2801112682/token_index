#pragma once

#include <string>
#include <array>

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
        ~trie_node();
        static std::size_t convertIndex(const char &c);

        std::array<trie_node *, ALPHABET_SIZE> _children;
        bool _is_end_word;
        char _key;
        std::string _value;
    };
}