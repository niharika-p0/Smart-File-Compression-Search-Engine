#include "../include/CompressionPipeline.h"
#include "../include/BWT.h"
#include "../include/MTF.h"
#include "../include/RLE.h"
#include "../include/Huffman.h"

#include <sstream>

std::string CompressionPipeline::compress(
    const std::string& input
) const
{
    if (input.empty())
    {
        return "";
    }

    BWT bwt;
    MTF mtf;
    RLE rle;
    Huffman huffman;

    // BWT
    BWTResult bwtResult =
        bwt.transform(input);

    // MTF
    std::string mtfData =
        mtf.encode(
            bwtResult.transformed
        );

    // RLE
    std::string rleData =
        rle.encode(
            mtfData
        );

    // Huffman
    std::string huffmanData =
        huffman.encode(
            rleData
        );

    std::ostringstream output;

    output
        << bwtResult.primaryIndex
        << "|"
        << huffmanData;

    return output.str();
}

std::string CompressionPipeline::decompress(
    const std::string& input
) const
{
    if (input.empty())
    {
        return "";
    }

    std::size_t separator =
        input.find('|');

    if (separator == std::string::npos)
    {
        return "";
    }

    int primaryIndex =
        std::stoi(
            input.substr(
                0,
                separator
            )
        );

    std::string huffmanData =
        input.substr(
            separator + 1
        );

    Huffman huffman;
    RLE rle;
    MTF mtf;
    BWT bwt;

    // Huffman decode
    std::string rleData =
        huffman.decode(
            huffmanData
        );

    // RLE decode
    std::string mtfData =
        rle.decode(
            rleData
        );

    // MTF decode
    std::string bwtData =
        mtf.decode(
            mtfData
        );

    // BWT inverse
    return bwt.inverseTransform(
        bwtData,
        primaryIndex
    );
}