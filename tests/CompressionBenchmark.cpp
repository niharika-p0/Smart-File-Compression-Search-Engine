#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "../include/FileManager.h"
#include "../include/SmartCompressor.h"

namespace fs = std::filesystem;

int main()
{
    std::cout
        << "\n========================================\n"
        << " SMART FILE SEARCH ENGINE\n"
        << " COMPRESSION BENCHMARK\n"
        << "========================================\n";

    FileManager fileManager(
        "data/documents"
    );

    std::vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        std::cout
            << "No documents found.\n";

        return 1;
    }

    fs::create_directories(
        "data/compressed"
    );

    SmartCompressor compressor;

    long long totalOriginalSize = 0;
    long long totalCompressedSize = 0;

    int compressedCount = 0;
    int originalKeptCount = 0;

    double totalCompressionTime = 0.0;

    std::cout
        << "\nDOCUMENT COMPRESSION RESULTS\n"
        << "-----------------------------\n";

    for (const auto& document :
         documents)
    {
        std::string outputPath =
            "data/compressed/" +
            document.fileName +
            ".huff";

        auto start =
            std::chrono::high_resolution_clock::now();

        CompressionResult result =
            compressor.compressFile(
                document.filePath,
                outputPath
            );

        auto end =
            std::chrono::high_resolution_clock::now();

        double time =
            std::chrono::duration<double, std::milli>(
                end - start
            ).count();

        totalCompressionTime += time;

        totalOriginalSize +=
            result.originalSize;

        totalCompressedSize +=
            result.compressedSize;

        double percentage = 0.0;

        if (result.originalSize > 0)
        {
            percentage =
                (
                    1.0 -
                    static_cast<double>(
                        result.compressedSize
                    ) /
                    static_cast<double>(
                        result.originalSize
                    )
                ) * 100.0;
        }

        std::cout
            << "\nFile: "
            << document.fileName
            << "\n";

        std::cout
            << "Original Size   : "
            << result.originalSize
            << " bytes\n";

        std::cout
            << "Compressed Size : "
            << result.compressedSize
            << " bytes\n";

        std::cout
            << "Compression     : "
            << std::fixed
            << std::setprecision(2)
            << percentage
            << "%\n";

        std::cout
            << "Time            : "
            << time
            << " ms\n";

        if (result.compressedSize <
            result.originalSize)
        {
            compressedCount++;

            std::cout
                << "Decision        : STORE COMPRESSED\n";
        }
        else
        {
            originalKeptCount++;

            std::cout
                << "Decision        : KEEP ORIGINAL\n";

            if (fs::exists(outputPath))
            {
                fs::remove(outputPath);
            }
        }
    }

    double totalPercentage = 0.0;

    if (totalOriginalSize > 0)
    {
        totalPercentage =
            (
                1.0 -
                static_cast<double>(
                    totalCompressedSize
                ) /
                static_cast<double>(
                    totalOriginalSize
                )
            ) * 100.0;
    }

    double averageTime =
        totalCompressionTime /
        documents.size();

    std::cout
        << "\n========================================\n"
        << " COMPRESSION SUMMARY\n"
        << "========================================\n";

    std::cout
        << "Documents              : "
        << documents.size()
        << "\n";

    std::cout
        << "Total Original Size    : "
        << totalOriginalSize
        << " bytes\n";

    std::cout
        << "Total Compressed Size  : "
        << totalCompressedSize
        << " bytes\n";

    std::cout
        << "Overall Compression    : "
        << std::fixed
        << std::setprecision(2)
        << totalPercentage
        << "%\n";

    std::cout
        << "Stored Compressed      : "
        << compressedCount
        << "\n";

    std::cout
        << "Original Files Kept    : "
        << originalKeptCount
        << "\n";

    std::cout
        << "Total Compression Time : "
        << totalCompressionTime
        << " ms\n";

    std::cout
        << "Average Compression    : "
        << averageTime
        << " ms/file\n";

    std::cout
        << "========================================\n"
        << " COMPRESSION TEST COMPLETED\n"
        << "========================================\n";

    return 0;
}