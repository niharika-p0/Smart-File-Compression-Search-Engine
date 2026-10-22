#include "../include/CompressionFileManager.h"
#include "../include/CompressionPipeline.h"
#include "../include/SmartCompressor.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace fs = std::filesystem;

CompressionFileResult
CompressionFileManager::compressFile(
    const std::string& inputFile,
    const std::string& outputFile
) const
{
    CompressionFileResult result;

    result.inputFile = inputFile;
    result.outputFile = outputFile;
    result.originalSize = 0;
    result.compressedSize = 0;
    result.compressionPercentage = 0.0;
    result.success = false;

    if (!fs::exists(inputFile))
    {
        return result;
    }

    try
    {
        result.originalSize =
            static_cast<long long>(
                fs::file_size(inputFile)
            );

        fs::path outputPath(outputFile);

        if (outputPath.has_parent_path())
        {
            fs::create_directories(
                outputPath.parent_path()
            );
        }

        /*
         * Use a temporary compressed file first.
         * SmartCompressor decides whether compression
         * is actually beneficial.
         */
        std::string tempCompressedFile =
            outputFile + ".tmp";

        SmartCompressor compressor;

        CompressionResult smartResult =
            compressor.compressFile(
                inputFile,
                tempCompressedFile
            );

        if (!smartResult.compressed)
        {
            /*
             * Compression is not beneficial.
             * Remove temporary compressed file.
             */
            if (fs::exists(tempCompressedFile))
            {
                fs::remove(tempCompressedFile);
            }

            result.compressedSize =
                result.originalSize;

            result.compressionPercentage = 0.0;

            /*
             * Keep the original file.
             */
            result.outputFile = inputFile;
            result.success = true;

            return result;
        }

        /*
         * Compression is beneficial.
         * Move temporary compressed file
         * to the requested output location.
         */
        if (fs::exists(outputFile))
        {
            fs::remove(outputFile);
        }

        fs::rename(
            tempCompressedFile,
            outputFile
        );

        result.compressedSize =
            static_cast<long long>(
                fs::file_size(outputFile)
            );

        if (result.originalSize > 0)
        {
            result.compressionPercentage =
                (
                    1.0 -
                    static_cast<double>(
                        result.compressedSize
                    )
                    /
                    static_cast<double>(
                        result.originalSize
                    )
                ) * 100.0;
        }

        result.success = true;

        return result;
    }
    catch (...)
    {
        return result;
    }
}


bool CompressionFileManager::decompressFile(
    const std::string& compressedFile,
    const std::string& outputFile
) const
{
    if (!fs::exists(compressedFile))
    {
        return false;
    }

    try
    {
        std::ifstream input(
            compressedFile,
            std::ios::binary
        );

        if (!input)
        {
            return false;
        }

        std::string compressed(
            (std::istreambuf_iterator<char>(input)),
            std::istreambuf_iterator<char>()
        );

        input.close();

        CompressionPipeline pipeline;

        std::string restored =
            pipeline.decompress(compressed);

        fs::path outputPath(outputFile);

        if (outputPath.has_parent_path())
        {
            fs::create_directories(
                outputPath.parent_path()
            );
        }

        std::ofstream output(
            outputFile,
            std::ios::binary
        );

        if (!output)
        {
            return false;
        }

        output.write(
            restored.data(),
            static_cast<std::streamsize>(
                restored.size()
            )
        );

        output.close();

        return true;
    }
    catch (...)
    {
        return false;
    }
}


void CompressionFileManager::displayResult(
    const CompressionFileResult& result
) const
{
    std::cout
        << "\nCOMPRESSION RESULT\n";

    std::cout
        << "------------------\n";

    std::cout
        << "Input File      : "
        << result.inputFile
        << "\n";

    std::cout
        << "Output File     : "
        << result.outputFile
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

    std::cout
        << "Status          : "
        << (
            result.success
                ? "SUCCESS"
                : "FAILED"
        )
        << "\n";
}