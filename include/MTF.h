#ifndef MTF_H
#define MTF_H

#include <string>

class MTF
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