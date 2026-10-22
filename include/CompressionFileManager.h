#ifndef COMPRESSIONFILEMANAGER_H
#define COMPRESSIONFILEMANAGER_H

#include <string>

struct CompressionFileResult
{
    std::string inputFile;
    std::string outputFile;

    long long originalSize;
    long long compressedSize;

    double compressionPercentage;

    bool success;
};

class CompressionFileManager
{
public:

    CompressionFileResult compressFile(
        const std::string& inputFile,
        const std::string& outputFile
    ) const;

    bool decompressFile(
        const std::string& compressedFile,
        const std::string& outputFile
    ) const;

    void displayResult(
        const CompressionFileResult& result
    ) const;
};

#endif