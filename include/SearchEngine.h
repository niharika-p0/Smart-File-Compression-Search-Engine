#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "BM25.h"
#include "Document.h"
#include "InvertedIndex.h"
#include "LRUCache.h"
#include "SuffixArray.h"
#include "Trie.h"

struct SearchResult
{
    int documentID;
    std::string fileName;
    double score;
};

class SearchEngine
{
private:

    std::vector<Document> documents;

    InvertedIndex invertedIndex;

    Trie trie;

    std::unordered_map<
        int,
        SuffixArray
    > suffixArrays;

    std::unique_ptr<BM25> bm25;

    LRUCache<
        std::string,
        std::vector<SearchResult>
    > cache;

    std::string normalize(
        const std::string& text
    ) const;

    std::vector<std::string> tokenize(
        const std::string& text
    ) const;

public:

    explicit SearchEngine(
        std::size_t cacheCapacity = 5
    );

    void build(
        const std::vector<Document>& docs
    );

    std::vector<SearchResult> search(
        const std::string& query
    );

    std::vector<SearchResult> substringSearch(
        const std::string& pattern
    ) const;

    std::vector<std::string> autocomplete(
        const std::string& prefix,
        int limit = 5
    ) const;

    void displayResults(
        const std::vector<SearchResult>& results
    ) const;

    std::size_t documentCount() const;
};

#endif