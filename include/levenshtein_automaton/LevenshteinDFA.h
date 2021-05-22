#pragma once

#include "levenshtein_automaton/trie_node.h"
#include "levenshtein_automaton/trie_tree.h"
#include "levenshtein_automaton/LevenshteinNFA.h"

#include <list>
#include <map>
#include <unordered_map>

namespace la
{
    class LevenshteinDFA
    {
    public:
        LevenshteinDFA();
        static LevenshteinDFA SubsetConstruct(const LevenshteinNFA &nfa);
        static std::list<int> EpsilonClosure(const LevenshteinNFA &nfa, const std::list<int> &states);
        int _start;
        std::list<int> _finalStates;
        std::map<std::pair<int, char>, int> _transTable;
        std::unordered_map<int, int> _defaultTrans;
        std::list<char> _uniqueChars;
        void Search(const trie_tree &trie, int start, trie_node* node, std::list<std::string> &output);
    };
}