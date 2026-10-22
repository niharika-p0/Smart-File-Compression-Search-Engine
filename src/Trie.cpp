#include "../include/Trie.h"

#include <algorithm>
#include <cctype>


TrieNode::TrieNode()
{
    for (int i = 0; i < 26; i++)
    {
        children[i] = nullptr;
    }

    isEndOfWord = false;

    frequency = 0;
}


Trie::Trie()
{
    root = new TrieNode();
}


Trie::~Trie()
{
    destroy(root);
}


void Trie::destroy(
    TrieNode* node
)
{
    if (node == nullptr)
    {
        return;
    }

    for (int i = 0; i < 26; i++)
    {
        destroy(node->children[i]);
    }

    delete node;
}


bool Trie::isValidWord(
    const std::string& word
) const
{
    if (word.empty())
    {
        return false;
    }

    for (char ch : word)
    {
        if (ch < 'a' || ch > 'z')
        {
            return false;
        }
    }

    return true;
}


void Trie::insert(
    const std::string& word
)
{
    if (!isValidWord(word))
    {
        return;
    }

    TrieNode* current = root;

    for (char ch : word)
    {
        int index = ch - 'a';

        if (current->children[index] == nullptr)
        {
            current->children[index] =
                new TrieNode();
        }

        current =
            current->children[index];
    }

    current->isEndOfWord = true;

    current->frequency++;
}


bool Trie::search(
    const std::string& word
) const
{
    if (!isValidWord(word))
    {
        return false;
    }

    TrieNode* current = root;

    for (char ch : word)
    {
        int index = ch - 'a';

        if (current->children[index] == nullptr)
        {
            return false;
        }

        current =
            current->children[index];
    }

    return current->isEndOfWord;
}


int Trie::getFrequency(
    const std::string& word
) const
{
    if (!isValidWord(word))
    {
        return 0;
    }

    TrieNode* current = root;

    for (char ch : word)
    {
        int index = ch - 'a';

        if (current->children[index] == nullptr)
        {
            return 0;
        }

        current =
            current->children[index];
    }

    if (!current->isEndOfWord)
    {
        return 0;
    }

    return current->frequency;
}


void Trie::collectWords(
    TrieNode* node,
    const std::string& currentWord,
    std::vector<std::pair<std::string, int>>& results
) const
{
    if (node == nullptr)
    {
        return;
    }

    if (node->isEndOfWord)
    {
        results.push_back(
            {
                currentWord,
                node->frequency
            }
        );
    }

    for (int i = 0; i < 26; i++)
    {
        if (node->children[i] != nullptr)
        {
            char nextChar =
                static_cast<char>('a' + i);

            collectWords(
                node->children[i],
                currentWord + nextChar,
                results
            );
        }
    }
}


std::vector<std::pair<std::string, int>>
Trie::autocomplete(
    const std::string& prefix,
    int maxResults
) const
{
    std::vector<std::pair<std::string, int>>
        results;

    if (!isValidWord(prefix))
    {
        return results;
    }

    TrieNode* current = root;

    for (char ch : prefix)
    {
        int index = ch - 'a';

        if (current->children[index] == nullptr)
        {
            return results;
        }

        current =
            current->children[index];
    }

    collectWords(
        current,
        prefix,
        results
    );

    std::sort(
        results.begin(),
        results.end(),
        [](const auto& a, const auto& b)
        {
            if (a.second != b.second)
            {
                return a.second > b.second;
            }

            return a.first < b.first;
        }
    );

    if (
        static_cast<int>(results.size())
        > maxResults
    )
    {
        results.resize(maxResults);
    }

    return results;
}