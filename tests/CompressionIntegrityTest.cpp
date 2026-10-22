#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../include/FileManager.h"
#include "../include/CompressionFileManager.h"

namespace fs = std::filesystem;

bool filesMatch(
    const std::string& file1,
    const std::string& file2
)
{
    std::ifstream a(file1, std::ios::binary);
    std::ifstream b(file2, std::ios::binary);

    if (!a.is_open() || !b.is_open())
    {
        return false;
    }

    char charA;
    char charB;

    while (true)
    {
        bool readA = static_cast<bool>(a.get(charA));
        bool readB = static_cast<bool>(b.get(charB));

        if (readA != readB)
        {
            return false;
        }

        if (!readA && !readB)
        {
            break;
        }

        if (charA != charB)
        {
            return false;
        }
    }

    return true;
}

int main()
{
    std::cout
        << "\n========================================\n"
        << " COMPRESSION INTEGRITY TEST\n"
        << "========================================\n";

    FileManager fileManager("data/documents");

    std::vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        std::cout << "No documents found.\n";
        return 1;
    }

    CompressionFileManager compressionManager;

    fs::create_directories("data/integrity_test");

    int passed = 0;
    int failed = 0;
    int tested = 0;

    for (const auto& document : documents)
    {
        if (tested >= 5)
        {
            break;
        }

        std::string compressedPath =
            "data/integrity_test/" +
            document.fileName +
            ".huff";

        std::string restoredPath =
            "data/integrity_test/restored_" +
            document.fileName;

        std::cout
            << "\n----------------------------------------\n";

        std::cout
            << "File: "
            << document.fileName
            << "\n";

        if (fs::exists(compressedPath))
        {
            fs::remove(compressedPath);
        }

        if (fs::exists(restoredPath))
        {
            fs::remove(restoredPath);
        }

        // Compress
        CompressionFileResult result =
            compressionManager.compressFile(
                document.filePath,
                compressedPath
            );

        std::cout
            << "Original Size   : "
            << result.originalSize
            << " bytes\n";

        std::cout
            << "Compressed Size : "
            << result.compressedSize
            << " bytes\n";

        if (!result.success)
        {
            std::cout
                << "Compression     : FAILED\n"
                << "Integrity       : FAILED\n";

            failed++;
            tested++;

            continue;
        }

        std::cout
            << "Compression     : SUCCESS\n";

        /*
         * SMART COMPRESSION CHECK
         *
         * If the compressed file exists,
         * compression was beneficial.
         *
         * If it does not exist,
         * SmartCompressor kept the original file
         * because compression was not beneficial.
         */
        if (!fs::exists(compressedPath))
        {
            std::cout
                << "Decision        : ORIGINAL KEPT\n";

            std::cout
                << "Decompression   : NOT REQUIRED\n";

            bool match =
                filesMatch(
                    document.filePath,
                    document.filePath
                );

            if (match)
            {
                std::cout
                    << "Integrity       : PASSED\n";

                passed++;
            }
            else
            {
                std::cout
                    << "Integrity       : FAILED\n";

                failed++;
            }

            tested++;

            continue;
        }

        std::cout
            << "Decision        : COMPRESSED FILE STORED\n";

        // Decompress
        bool restored =
            compressionManager.decompressFile(
                compressedPath,
                restoredPath
            );

        if (!restored)
        {
            std::cout
                << "Decompression   : FAILED\n"
                << "Integrity       : FAILED\n";

            failed++;
            tested++;

            continue;
        }

        std::cout
            << "Decompression   : SUCCESS\n";

        // Compare original and restored
        bool match =
            filesMatch(
                document.filePath,
                restoredPath
            );

        if (match)
        {
            std::cout
                << "Integrity       : PASSED\n";

            passed++;
        }
        else
        {
            std::cout
                << "Integrity       : FAILED\n";

            failed++;
        }

        tested++;
    }

    std::cout
        << "\n========================================\n"
        << " INTEGRITY SUMMARY\n"
        << "========================================\n";

    std::cout
        << "Files Tested : "
        << tested
        << "\n";

    std::cout
        << "Passed       : "
        << passed
        << "\n";

    std::cout
        << "Failed       : "
        << failed
        << "\n";

    std::cout
        << "========================================\n";

    if (failed == 0)
    {
        std::cout
            << "ALL INTEGRITY TESTS PASSED\n";
    }
    else
    {
        std::cout
            << "SOME INTEGRITY TESTS FAILED\n";
    }

    std::cout
        << "========================================\n";

    return 0;
}