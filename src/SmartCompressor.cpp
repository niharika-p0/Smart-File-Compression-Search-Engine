#include "../include/SmartCompressor.h"
#include "../include/CompressionPipeline.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

CompressionResult
SmartCompressor::compressFile(
    const std::string& inputFile,
    const std::string& outputFile
)
{
    CompressionResult result;

    result.originalFile = inputFile;
    result.compressedFile = outputFile;
    result.originalSize = 0;
    result.compressedSize = 0;
    result.compressionPercentage = 0.0;
    result.compressed = false;

    // Check input file
    if (!fs::exists(inputFile))
    {
        std::cout
            << "Input file does not exist: "
            << inputFile
            << "\n";

        return result;
    }

    result.originalSize =
        static_cast<long long>(
            fs::file_size(inputFile)
        );

    // Read original file
    std::ifstream input(
        inputFile,
        std::ios::binary
    );

    if (!input)
    {
        std::cout
            << "Unable to open input file.\n";

        return result;
    }

    std::string content(
        (std::istreambuf_iterator<char>(input)),
        std::istreambuf_iterator<char>()
    );

    input.close();

    if (content.empty())
    {
        return result;
    }

    // Create output directory if required
    fs::path outputPath(outputFile);

    if (outputPath.has_parent_path())
    {
        fs::create_directories(
            outputPath.parent_path()
        );
    }

    // Compression pipeline:
    // BWT -> MTF -> RLE -> Huffman
    CompressionPipeline pipeline;

    std::string compressedData =
        pipeline.compress(content);

    if (compressedData.empty())
    {
        std::cout
            << "Compression failed.\n";

        return result;
    }

    // Write compressed binary data
    std::ofstream output(
        outputFile,
        std::ios::binary
    );

    if (!output)
    {
        std::cout
            << "Unable to create compressed file.\n";

        return result;
    }

    output.write(
        compressedData.data(),
        static_cast<std::streamsize>(
            compressedData.size()
        )
    );

    output.close();

    if (!fs::exists(outputFile))
    {
        return result;
    }

    // Actual compressed candidate size
    result.compressedSize =
        static_cast<long long>(
            fs::file_size(outputFile)
        );

    // Calculate compression percentage
    if (result.originalSize > 0)
    {
        result.compressionPercentage =
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

    // Smart compression decision
    result.compressed =
        shouldCompress(
            result.originalSize,
            result.compressedSize
        );

    // If compression does not save space,
    // remove compressed candidate.
    if (!result.compressed)
    {
        fs::remove(outputFile);
    }

    return result;
}

bool SmartCompressor::shouldCompress(
    long long originalSize,
    long long compressedSize
) const
{
    return compressedSize < originalSize;
}

void SmartCompressor::displayResult(
    const CompressionResult& result
) const
{
    std::cout
        << "\nCOMPRESSION RESULT\n";

    std::cout
        << "------------------\n";

    std::cout
        << "Original File   : "
        << result.originalFile
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
        << result.compressionPercentage
        << "%\n";

    if (result.compressed)
    {
        std::cout
            << "Decision        : STORE COMPRESSED\n";
    }
    else
    {
        std::cout
            << "Decision        : KEEP ORIGINAL\n";
    }
}