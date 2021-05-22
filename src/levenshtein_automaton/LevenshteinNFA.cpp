#include <iostream>
#include <queue>
#include "levenshtein_automaton/LevenshteinNFA.h"

namespace la
{
    LevenshteinNFA::LevenshteinNFA(std::size_t size, int initialstate, std::list<int> &&finalStates)
        : _initialState{initialstate},
          _size{size},
          _finalStates{std::move(finalStates)},
          _transTable(size * size, '\0') {}

    LevenshteinNFA LevenshteinNFA::ConstructNFA(const std::string &str, int maxDist)
    {
        auto width{str.size() + 1};
        auto height{maxDist + 1};
        auto size{width * height};
        std::list<int> finalStates;
        for (decltype(height) i = 1; i <= height; ++i)
            finalStates.emplace_back(i * width - 1);
        LevenshteinNFA nfa{size, 0, std::move(finalStates)};
        for (decltype(height) e = 0; e < height; ++e)
        {
            for (decltype(width) i = 0; i < width - 1; ++i)
            {
                nfa.AddTransition(e * width + i, e * width + i + 1, str[i]);
                if (e < height - 1)
                {
                    nfa.AddTransition(e * width + i, (e + 1) * width + i, (char)Constants::Insertion);
                    nfa.AddTransition(e * width + i, (e + 1) * width + i + 1, (char)Constants::Deletion);
                }
            }
        }
        for (decltype(height) j = 1; j < height; ++j)
            nfa.AddTransition(j * width - 1, (j + 1) * width - 1, (char)Constants::Insertion);
        return nfa;
    }

    void LevenshteinNFA::AddTransition(int from, int to, char input)
    {
        _transTable.at((from * _size) + to) = input;
        _inputs.emplace_front(input);
        _inputs.sort();
        _inputs.unique();
    }

    std::list<int> LevenshteinNFA::Move(const std::list<int> &states, char inp) const
    {
        std::list<int> result;
        std::queue<int> tmp;
        bool needNormalLetter = false;
        bool findNormalLetter = false;
        if (inp != (char)LevenshteinNFA::Constants::Insertion &&
            inp != (char)LevenshteinNFA::Constants::Deletion)
            needNormalLetter = true;
        for (auto it = std::begin(states); it != std::end(states); ++it)
        {
            for (decltype(_size) j = 0; j < _size; j++)
            {
                char c = _transTable.at((*it * _size) + j);
                if (c == inp || c == (char)LevenshteinNFA::Constants::Insertion || c == (char)LevenshteinNFA::Constants::Deletion)
                {
                    if (needNormalLetter && c == inp)
                        findNormalLetter = true;
                    result.push_back(j);
                }
            }
        }
        while (!tmp.empty())
        {
            int now = tmp.front();
            tmp.pop();
            for (int j = 0; j < _size; j++)
            {
                char c = _transTable.at((now * _size) + j);
                if (c == (char)LevenshteinNFA::Constants::Deletion)
                {
                    tmp.push(j);
                    result.push_back(j);
                }
            }
        }
        if (needNormalLetter && !findNormalLetter)
            result.clear();
        result.sort();
        result.unique();
        return result;
    }

    void LevenshteinNFA::Show()
    {
        std::cout << "This NFA has " << _size << "size states: 0 - " << _size - 1 << '\n';
        std::cout << "The initial state is " << _initialState << '\n';
        //std::cout << "The final state is "<< finalStates << '\n';
        for (int from = 0; from < _size; ++from)
        {
            for (int to = 0; to < _size; ++to)
            {
                char in = _transTable.at((from * _size) + to);

                if (in != (char)LevenshteinNFA::Constants::Dead)
                {
                    std::cout << "Transition from " << from << " to " << to << " on input ";

                    if (in == (char)LevenshteinNFA::Constants::Deletion)
                        std::cout << "Deletion\n";
                    else if (in == (char)LevenshteinNFA::Constants::Insertion)
                        std::cout << "Insertion\n";
                    else
                        std::cout << in << " \n";
                }
            }
        }
        std::cout << "\n\n";
    }
}