#ifndef BWT_H
#define BWT_H

#include <string>

struct BWTResult
{
    std::string transformed;
    int primaryIndex;
};

class BWT
{
public:

    BWTResult transform(
        const std::string& text
    ) const;

    std::string inverseTransform(
        const std::string& transformed,
        int primaryIndex
    ) const;
};

#endif