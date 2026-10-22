#include <iostream>
#include <string>
#include <vector>

#include "../include/FileManager.h"
#include "../include/SearchEngine.h"
#include "../include/CompressedStorage.h"
#include "../include/PerformanceBenchmark.h"
#include "../include/Graph.h"

void printLine()
{
    std::cout
        << "\n========================================\n";
}

int main()
{
    printLine();

    std::cout
        << "SMART FILE SEARCH ENGINE\n"
        << "FULL SYSTEM TEST\n";

    printLine();

    // =========================================
    // 1. LOAD DOCUMENTS
    // =========================================

    FileManager fileManager(
        "data/documents"
    );

    std::vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        std::cout
            << "FAIL: No documents found.\n";

        return 1;
    }

    std::cout
        << "PASS: Documents loaded\n";

    std::cout
        << "Document count: "
        << documents.size()
        << "\n";


    // =========================================
    // 2. BUILD SEARCH ENGINE
    // =========================================

    SearchEngine searchEngine(5);

    searchEngine.build(
        documents
    );

    std::cout
        << "PASS: Search engine built\n";


    // =========================================
    // 3. BM25 SEARCH
    // =========================================

    std::cout
        << "\nBM25 SEARCH TEST\n";

    std::vector<SearchResult> searchResults =
        searchEngine.search(
            "machine learning"
        );

    if (!searchResults.empty())
    {
        std::cout
            << "PASS: Search returned "
            << searchResults.size()
            << " results\n";

        searchEngine.displayResults(
            searchResults
        );
    }
    else
    {
        std::cout
            << "FAIL: Search returned no results\n";
    }


    // =========================================
    // 4. LRU CACHE TEST
    // =========================================

    std::cout
        << "\nLRU CACHE TEST\n";

    std::vector<SearchResult> firstSearch =
        searchEngine.search(
            "python"
        );

    std::vector<SearchResult> secondSearch =
        searchEngine.search(
            "python"
        );

    if (firstSearch.size() ==
        secondSearch.size())
    {
        std::cout
            << "PASS: Repeated query handled correctly\n";
    }
    else
    {
        std::cout
            << "FAIL: Cache test failed\n";
    }


    // =========================================
    // 5. AUTOCOMPLETE TEST
    // =========================================

    std::cout
        << "\nTRIE AUTOCOMPLETE TEST\n";

    std::vector<std::string> suggestions =
        searchEngine.autocomplete(
            "data",
            5
        );

    if (!suggestions.empty())
    {
        std::cout
            << "PASS: Autocomplete returned "
            << suggestions.size()
            << " suggestions\n";

        for (const auto& word :
             suggestions)
        {
            std::cout
                << "  "
                << word
                << "\n";
        }
    }
    else
    {
        std::cout
            << "FAIL: No autocomplete suggestions\n";
    }


    // =========================================
    // 6. SUFFIX ARRAY SUBSTRING SEARCH
    // =========================================

    std::cout
        << "\nSUFFIX ARRAY TEST\n";

    std::vector<SearchResult>
        substringResults =
            searchEngine.substringSearch(
                "data"
            );

    if (!substringResults.empty())
    {
        std::cout
            << "PASS: Substring search returned "
            << substringResults.size()
            << " documents\n";

        for (const auto& result :
             substringResults)
        {
            std::cout
                << "  "
                << result.fileName
                << " | Matches: "
                << result.score
                << "\n";
        }
    }
    else
    {
        std::cout
            << "FAIL: Substring search failed\n";
    }


    // =========================================
    // 7. COMPRESSED STORAGE
    // =========================================

    std::cout
        << "\nCOMPRESSED STORAGE TEST\n";

    CompressedStorage storage(
        "data/compressed"
    );

    if (storage.storeDocuments(
            documents))
    {
        std::cout
            << "PASS: Documents stored\n";
    }
    else
    {
        std::cout
            << "FAIL: Compression storage failed\n";
    }

    storage.displayStorage();


    // =========================================
    // 8. RESTORE TEST
    // =========================================

    if (!documents.empty())
    {
        int documentID =
            documents[0].id;

        bool restored =
            storage.restoreDocument(
                documentID,
                "data/restored/test_restore.txt"
            );

        if (restored)
        {
            std::cout
                << "PASS: Document restored\n";
        }
        else
        {
            std::cout
                << "FAIL: Document restore failed\n";
        }
    }


    // =========================================
    // 9. GRAPH TEST
    // =========================================

    std::cout
        << "\nGRAPH TEST\n";

    Graph graph(
        static_cast<int>(
            documents.size()
        )
    );

    graph.buildGraph(
        documents,
        0.20
    );

    graph.displayGraph(
        documents
    );

    if (!documents.empty())
    {
        std::vector<
            std::pair<int, double>
        > recommendations =
            graph.recommend(
                documents[0].id,
                5
            );

        std::cout
            << "\nRecommendations: "
            << recommendations.size()
            << "\n";

        if (!recommendations.empty())
        {
            std::cout
                << "PASS: Recommendation system working\n";
        }
        else
        {
            std::cout
                << "INFO: No similar documents found\n";
        }
    }


    // =========================================
    // 10. PERFORMANCE BENCHMARK
    // =========================================

    std::cout
        << "\nPERFORMANCE BENCHMARK TEST\n";

    PerformanceBenchmark benchmark(
        documents,
        5
    );

    std::vector<std::string> queries =
    {
        "python",
        "database",
        "machine learning",
        "data",
        "algorithm"
    };

    std::vector<BenchmarkRecord>
        benchmarkResults =
            benchmark.benchmarkQueries(
                queries
            );

    benchmark.displayAll(
        benchmarkResults
    );


    // =========================================
    // FINAL RESULT
    // =========================================

    printLine();

    std::cout
        << "FULL SYSTEM TEST COMPLETED\n";

    std::cout
        << "========================================\n";

    return 0;
}