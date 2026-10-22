#ifndef SMARTCOMPRESSOR_H
#define SMARTCOMPRESSOR_H

#include <string>

struct CompressionResult
{
    std::string originalFile;
    std::string compressedFile;

    long long originalSize;
    long long compressedSize;

    double compressionPercentage;

    bool compressed;
};

class SmartCompressor
{
public:

    CompressionResult compressFile(
        const std::string& inputFile,
        const std::string& outputFile
    );

    bool shouldCompress(
        long long originalSize,
        long long compressedSize
    ) const;

    void displayResult(
        const CompressionResult& result
    ) const;
};

#endif