#include "../include/MTF.h"

#include <algorithm>
#include <vector>

std::string MTF::encode(
    const std::string& input
) const
{
    std::vector<unsigned char> table(256);

    for (int i = 0; i < 256; ++i)
    {
        table[i] =
            static_cast<unsigned char>(i);
    }

    std::string output;

    output.reserve(input.size());

    for (unsigned char ch : input)
    {
        auto it = std::find(
            table.begin(),
            table.end(),
            ch
        );

        int position =
            static_cast<int>(
                it - table.begin()
            );

        output += static_cast<char>(
            position
        );

        unsigned char value =
            table[position];

        for (int i = position;
             i > 0;
             --i)
        {
            table[i] =
                table[i - 1];
        }

        table[0] = value;
    }

    return output;
}

std::string MTF::decode(
    const std::string& input
) const
{
    std::vector<unsigned char> table(256);

    for (int i = 0; i < 256; ++i)
    {
        table[i] =
            static_cast<unsigned char>(i);
    }

    std::string output;

    output.reserve(input.size());

    for (unsigned char encoded :
         input)
    {
        int position =
            static_cast<int>(encoded);

        unsigned char value =
            table[position];

        output += static_cast<char>(
            value
        );

        for (int i = position;
             i > 0;
             --i)
        {
            table[i] =
                table[i - 1];
        }

        table[0] = value;
    }

    return output;
}