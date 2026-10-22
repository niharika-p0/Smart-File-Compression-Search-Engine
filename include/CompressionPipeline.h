#ifndef COMPRESSIONPIPELINE_H
#define COMPRESSIONPIPELINE_H

#include <string>

class CompressionPipeline
{
public:

    std::string compress(
        const std::string& input
    ) const;

    std::string decompress(
        const std::string& input
    ) const;
};

#endif