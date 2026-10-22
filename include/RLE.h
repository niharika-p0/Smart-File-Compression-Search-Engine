#ifndef RLE_H
#define RLE_H

#include <string>

class RLE
{
public:

    std::string encode(
        const std::string& input
    ) const;

    std::string decode(
        const std::string& input
    ) const;
};

#endif