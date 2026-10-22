#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <cctype>

#include "../include/FileManager.h"
#include "../include/InvertedIndex.h"
#include "../include/Trie.h"
#include "../include/SuffixArray.h"
#include "../include/BM25.h"
#include "../include/LRUCache.h"

using namespace std;
using namespace chrono;


// ============================================================
// TOKENIZER
// ============================================================

vector<string> tokenize(const string& text)
{
    vector<string> tokens;
    string word;

    for (char ch : text)
    {
        if (isalnum(static_cast<unsigned char>(ch)))
        {
            word += static_cast<char>(
                tolower(static_cast<unsigned char>(ch))
            );
        }
        else
        {
            if (!word.empty())
            {
                tokens.push_back(word);
                word.clear();
            }
        }
    }

    if (!word.empty())
    {
        tokens.push_back(word);
    }

    return tokens;
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    cout << "\n========================================\n";
    cout << "       PERFORMANCE TEST\n";
    cout << "    SMART FILE SEARCH ENGINE\n";
    cout << "========================================\n";


    // ========================================================
    // LOAD DOCUMENTS
    // ========================================================

    FileManager fileManager(
        "data/documents"
    );

    vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        cout << "\nNo documents found.\n";
        return 1;
    }

    cout << "\nDocuments Loaded : "
         << documents.size()
         << "\n";


    // ========================================================
    // BUILD INVERTED INDEX + TRIE
    // ========================================================

    InvertedIndex invertedIndex;

    Trie trie;

    auto indexStart =
        high_resolution_clock::now();


    for (const auto& document : documents)
    {
        vector<string> tokens =
            tokenize(document.content);


        // Add document to inverted index
        invertedIndex.addDocument(
            document.id,
            tokens
        );


        // Add words to Trie
        for (const string& token : tokens)
        {
            trie.insert(token);
        }
    }


    auto indexEnd =
        high_resolution_clock::now();


    double indexTime =
        duration<double, milli>(
            indexEnd - indexStart
        ).count();


    cout << "\n========================================\n";
    cout << "       INDEX BUILD PERFORMANCE\n";
    cout << "========================================\n";

    cout << "Index Build Time : "
         << indexTime
         << " ms\n";

    cout << "Vocabulary Size  : "
         << invertedIndex.vocabularySize()
         << "\n";


    // ========================================================
    // BM25 SEARCH
    // ========================================================

    BM25 bm25(
        documents,
        invertedIndex
    );


    vector<string> queryTerms =
    {
        "artificial",
        "intelligence"
    };


    vector<BM25Result> results;

    double totalSearchTime = 0.0;


    // Run multiple times for a more stable measurement
    for (int i = 0; i < 10; i++)
    {
        auto searchStart =
            high_resolution_clock::now();


        results =
            bm25.rank(queryTerms);


        auto searchEnd =
            high_resolution_clock::now();


        totalSearchTime +=
            duration<double, micro>(
                searchEnd - searchStart
            ).count();
    }


    double averageSearchTime =
        totalSearchTime / 10.0;


    cout << "\n========================================\n";
    cout << "         SEARCH PERFORMANCE\n";
    cout << "========================================\n";

    cout << "Query            : "
         << "artificial intelligence\n";

    cout << "Average Search   : "
         << averageSearchTime
         << " us\n";

    cout << "Results          : "
         << results.size()
         << "\n";


    // ========================================================
    // LRU CACHE PERFORMANCE
    // ========================================================

    LRUCache<
        string,
        vector<BM25Result>
    > cache(5);


    vector<BM25Result> cachedResults;


    // First search
    auto firstStart =
        high_resolution_clock::now();


    cachedResults =
        bm25.rank(queryTerms);


    auto firstEnd =
        high_resolution_clock::now();


    double firstSearchTime =
        duration<double, micro>(
            firstEnd - firstStart
        ).count();


    // Store result in cache
    cache.put(
        "artificial intelligence",
        cachedResults
    );


    // Cached search
    auto cachedStart =
        high_resolution_clock::now();


    bool cacheHit =
        cache.get(
            "artificial intelligence",
            cachedResults
        );


    auto cachedEnd =
        high_resolution_clock::now();


    double cachedSearchTime =
        duration<double, micro>(
            cachedEnd - cachedStart
        ).count();


    cout << "\n========================================\n";
    cout << "          LRU CACHE PERFORMANCE\n";
    cout << "========================================\n";

    cout << "First Search     : "
         << firstSearchTime
         << " us\n";

    cout << "Cached Search    : "
         << cachedSearchTime
         << " us\n";

    cout << "Cache Hit        : "
         << (cacheHit ? "YES" : "NO")
         << "\n";

    cout << "Cache Result     : "
         << (
                cachedSearchTime < firstSearchTime
                ? "FASTER"
                : "SLOWER"
            )
         << "\n";


    // ========================================================
    // TRIE AUTOCOMPLETE PERFORMANCE
    // ========================================================

    auto trieStart =
        high_resolution_clock::now();


    // IMPORTANT:
    // autocomplete() returns vector<pair<string,int>>
    vector<pair<string, int>> suggestions =
        trie.autocomplete(
            "dat",
            5
        );


    auto trieEnd =
        high_resolution_clock::now();


    double trieTime =
        duration<double, micro>(
            trieEnd - trieStart
        ).count();


    cout << "\n========================================\n";
    cout << "       AUTOCOMPLETE PERFORMANCE\n";
    cout << "========================================\n";

    cout << "Prefix           : dat\n";

    cout << "Autocomplete Time: "
         << trieTime
         << " us\n";

    cout << "Suggestions      : "
         << suggestions.size()
         << "\n";


    // ========================================================
    // SUFFIX ARRAY PERFORMANCE
    // ========================================================

    double totalSuffixTime = 0.0;

    int documentsFound = 0;

    string pattern = "data";


    for (const auto& document : documents)
    {
        SuffixArray suffixArray;


        // Build suffix array
        suffixArray.build(
            document.content
        );


        auto suffixStart =
            high_resolution_clock::now();


        vector<int> positions =
            suffixArray.search(
                pattern
            );


        auto suffixEnd =
            high_resolution_clock::now();


        totalSuffixTime +=
            duration<double, micro>(
                suffixEnd - suffixStart
            ).count();


        if (!positions.empty())
        {
            documentsFound++;
        }
    }


    double averageSuffixTime =
        totalSuffixTime /
        static_cast<double>(
            documents.size()
        );


    cout << "\n========================================\n";
    cout << "       SUFFIX ARRAY PERFORMANCE\n";
    cout << "========================================\n";

    cout << "Pattern          : "
         << pattern
         << "\n";

    cout << "Average Search   : "
         << averageSuffixTime
         << " us\n";

    cout << "Documents Found  : "
         << documentsFound
         << "\n";


    // ========================================================
    // FINAL PERFORMANCE SUMMARY
    // ========================================================

    cout << "\n========================================\n";
    cout << "        PERFORMANCE SUMMARY\n";
    cout << "========================================\n";


    cout << "Documents        : "
         << documents.size()
         << "\n";


    cout << "Index Build      : "
         << indexTime
         << " ms\n";


    cout << "Average Search   : "
         << averageSearchTime
         << " us\n";


    cout << "Autocomplete     : "
         << trieTime
         << " us\n";


    cout << "Suffix Search    : "
         << averageSuffixTime
         << " us\n";


    cout << "First Cache      : "
         << firstSearchTime
         << " us\n";


    cout << "Cached Search    : "
         << cachedSearchTime
         << " us\n";


    cout << "========================================\n";

    cout << "PERFORMANCE TEST COMPLETED\n";

    cout << "========================================\n";


    return 0;
}