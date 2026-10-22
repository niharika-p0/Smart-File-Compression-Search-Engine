#include <iostream>
#include <filesystem>
#include <iomanip>

#include "../include/FileManager.h"
#include "../include/SmartCompressor.h"

namespace fs = std::filesystem;

int main()
{
    std::cout
        << "\n========================================\n"
        << " COMPRESSION PIPELINE DEBUG TEST\n"
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

    SmartCompressor compressor;

    fs::create_directories(
        "data/compressed_debug"
    );

    int tested = 0;

    for (const auto& document : documents)
    {
        if (tested >= 5)
        {
            break;
        }

        std::string output =
            "data/compressed_debug/" +
            document.fileName +
            ".huff";

        std::cout
            << "\n----------------------------------------\n";

        std::cout
            << "File: "
            << document.fileName
            << "\n";

        CompressionResult result =
            compressor.compressFile(
                document.filePath,
                output
            );

        std::cout
            << "Original Size   : "
            << result.originalSize
            << " bytes\n";

        std::cout
            << "Compressed Size : "
            << result.compressedSize
            << " bytes\n";

        if (result.originalSize > 0)
        {
            double saving =
                (
                    1.0 -
                    static_cast<double>(
                        result.compressedSize
                    ) /
                    static_cast<double>(
                        result.originalSize
                    )
                ) * 100.0;

            std::cout
                << "Compression     : "
                << std::fixed
                << std::setprecision(2)
                << saving
                << "%\n";
        }

        if (result.compressedSize <
            result.originalSize)
        {
            std::cout
                << "Decision        : COMPRESSED\n";
        }
        else
        {
            std::cout
                << "Decision        : ORIGINAL\n";
        }

        tested++;
    }

    std::cout
        << "\n========================================\n"
        << " DEBUG TEST COMPLETED\n"
        << "========================================\n";

    return 0;
}