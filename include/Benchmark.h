#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>

struct BenchmarkResult
{
    std::string operation;

    double timeMilliseconds;

    long long inputSize;

    long long outputSize;

    double compressionPercentage;
};

class Benchmark
{
public:

    BenchmarkResult measureCompression(
        const std::string& inputFile,
        const std::string& outputFile
    ) const;

    BenchmarkResult measureSearch(
        const std::string& query,
        int documentCount
    ) const;

    void displayResult(
        const BenchmarkResult& result
    ) const;
};

#endif