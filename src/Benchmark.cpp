#include "../include/Benchmark.h"
#include "../include/CompressionFileManager.h"
#include "../include/FileManager.h"
#include "../include/SearchEngine.h"

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>

namespace fs = std::filesystem;

BenchmarkResult
Benchmark::measureCompression(
    const std::string& inputFile,
    const std::string& outputFile
) const
{
    BenchmarkResult result;

    result.operation = "Compression";
    result.timeMilliseconds = 0.0;
    result.inputSize = 0;
    result.outputSize = 0;
    result.compressionPercentage = 0.0;

    if (!fs::exists(inputFile))
    {
        return result;
    }

    result.inputSize =
        static_cast<long long>(
            fs::file_size(inputFile)
        );

    CompressionFileManager manager;

    auto start =
        std::chrono::high_resolution_clock::now();

    CompressionFileResult compression =
        manager.compressFile(
            inputFile,
            outputFile
        );

    auto end =
        std::chrono::high_resolution_clock::now();

    result.timeMilliseconds =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    if (compression.success)
    {
        result.outputSize =
            compression.compressedSize;

        result.compressionPercentage =
            compression.compressionPercentage;
    }

    return result;
}

BenchmarkResult
Benchmark::measureSearch(
    const std::string& query,
    int documentCount
) const
{
    BenchmarkResult result;

    result.operation = "Search";
    result.timeMilliseconds = 0.0;
    result.inputSize = documentCount;
    result.outputSize = 0;
    result.compressionPercentage = 0.0;

    FileManager fileManager(
        "data/documents"
    );

    std::vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        return result;
    }

    SearchEngine engine(5);

    engine.build(documents);

    auto start =
        std::chrono::high_resolution_clock::now();

    std::vector<SearchResult> results =
        engine.search(query);

    auto end =
        std::chrono::high_resolution_clock::now();

    result.timeMilliseconds =
        std::chrono::duration<double, std::milli>(
            end - start
        ).count();

    result.outputSize =
        static_cast<long long>(
            results.size()
        );

    return result;
}

void Benchmark::displayResult(
    const BenchmarkResult& result
) const
{
    std::cout
        << "\nBENCHMARK RESULT\n";

    std::cout
        << "----------------\n";

    std::cout
        << "Operation       : "
        << result.operation
        << "\n";

    std::cout
        << "Time            : "
        << std::fixed
        << std::setprecision(4)
        << result.timeMilliseconds
        << " ms\n";

    std::cout
        << "Input Size      : "
        << result.inputSize
        << "\n";

    std::cout
        << "Output Size     : "
        << result.outputSize
        << "\n";

    if (result.operation ==
        "Compression")
    {
        std::cout
            << "Compression     : "
            << std::setprecision(2)
            << result.compressionPercentage
            << "%\n";
    }
}