#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <utility>

struct TrieNode
{
    TrieNode* children[26];

    bool isEndOfWord;

    int frequency;

    TrieNode();
};


class Trie
{
private:

    TrieNode* root;

    void collectWords(
        TrieNode* node,
        const std::string& currentWord,
        std::vector<std::pair<std::string, int>>& results
    ) const;

    void destroy(
        TrieNode* node
    );

    bool isValidWord(
        const std::string& word
    ) const;

public:

    Trie();

    ~Trie();

    void insert(
        const std::string& word
    );

    bool search(
        const std::string& word
    ) const;

    int getFrequency(
        const std::string& word
    ) const;

    std::vector<std::pair<std::string, int>>
    autocomplete(
        const std::string& prefix,
        int maxResults = 10
    ) const;
};

#endif