#include "../include/PerformanceBenchmark.h"

#include <iostream>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace chrono;


// ============================================================
// CONSTRUCTOR
// ============================================================

PerformanceBenchmark::PerformanceBenchmark(
    const vector<Document>& docs,
    size_t cacheCapacity
)
    : documents(docs),
      searchEngine(cacheCapacity)
{
    searchEngine.build(documents);
}


// ============================================================
// BENCHMARK SINGLE SEARCH
// ============================================================

BenchmarkRecord
PerformanceBenchmark::benchmarkSearch(
    const string& query
)
{
    auto start =
        high_resolution_clock::now();

    vector<SearchResult> results =
        searchEngine.search(query);

    auto end =
        high_resolution_clock::now();

    double timeMilliseconds =
        duration<double, milli>(
            end - start
        ).count();

    BenchmarkRecord record;

    record.operation = "Search";
    record.query = query;
    record.timeMilliseconds =
        timeMilliseconds;
    record.resultCount =
        results.size();

    return record;
}


// ============================================================
// BENCHMARK MULTIPLE QUERIES
// ============================================================

vector<BenchmarkRecord>
PerformanceBenchmark::benchmarkQueries(
    const vector<string>& queries
)
{
    vector<BenchmarkRecord> results;

    for (const string& query : queries)
    {
        BenchmarkRecord record =
            benchmarkSearch(query);

        results.push_back(record);
    }

    return results;
}


// ============================================================
// DISPLAY SINGLE RESULT
// ============================================================

void PerformanceBenchmark::displayResult(
    const BenchmarkRecord& result
) const
{
    cout
        << "\nOperation : "
        << result.operation
        << "\n";

    cout
        << "Query     : "
        << result.query
        << "\n";

    cout
        << "Time      : "
        << fixed
        << setprecision(4)
        << result.timeMilliseconds
        << " ms\n";

    cout
        << "Results   : "
        << result.resultCount
        << "\n";
}


// ============================================================
// DISPLAY ALL RESULTS
// ============================================================

void PerformanceBenchmark::displayAll(
    const vector<BenchmarkRecord>& results
) const
{
    cout
        << "\n========================================\n";

    cout
        << "       PERFORMANCE BENCHMARK\n";

    cout
        << "========================================\n";

    double totalTime = 0.0;

    size_t totalResults = 0;

    for (const BenchmarkRecord& result : results)
    {
        displayResult(result);

        totalTime +=
            result.timeMilliseconds;

        totalResults +=
            result.resultCount;
    }

    cout
        << "\n----------------------------------------\n";

    cout
        << "Total Queries : "
        << results.size()
        << "\n";

    cout
        << "Total Time    : "
        << fixed
        << setprecision(4)
        << totalTime
        << " ms\n";

    if (!results.empty())
    {
        cout
            << "Average Time  : "
            << totalTime /
                   static_cast<double>(
                       results.size()
                   )
            << " ms\n";
    }

    cout
        << "Total Results : "
        << totalResults
        << "\n";

    cout
        << "========================================\n";
}