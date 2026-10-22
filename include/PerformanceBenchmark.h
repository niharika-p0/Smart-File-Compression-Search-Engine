#ifndef PERFORMANCEBENCHMARK_H
#define PERFORMANCEBENCHMARK_H

#include <string>
#include <vector>

#include "FileManager.h"
#include "SearchEngine.h"

struct BenchmarkRecord
{
    std::string operation;
    std::string query;

    double timeMilliseconds;

    std::size_t resultCount;
};

class PerformanceBenchmark
{
private:

    std::vector<Document> documents;

    SearchEngine searchEngine;

public:

    PerformanceBenchmark(
        const std::vector<Document>& docs,
        std::size_t cacheCapacity = 5
    );

    BenchmarkRecord benchmarkSearch(
        const std::string& query
    );

    std::vector<BenchmarkRecord>
    benchmarkQueries(
        const std::vector<std::string>& queries
    );

    void displayResult(
        const BenchmarkRecord& result
    ) const;

    void displayAll(
        const std::vector<BenchmarkRecord>& results
    ) const;
};

#endif