#include "../include/RLE.h"

#include <sstream>

std::string RLE::encode(
    const std::string& input
) const
{
    if (input.empty())
    {
        return "";
    }

    std::ostringstream output;

    int count = 1;

    for (std::size_t i = 1;
         i <= input.size();
         ++i)
    {
        if (i < input.size() &&
            input[i] == input[i - 1])
        {
            ++count;
        }
        else
        {
            output
                << count
                << "#"
                << static_cast<int>(
                    static_cast<unsigned char>(
                        input[i - 1]
                    )
                )
                << ";";

            count = 1;
        }
    }

    return output.str();
}

std::string RLE::decode(
    const std::string& input
) const
{
    if (input.empty())
    {
        return "";
    }

    std::string output;

    std::size_t position = 0;

    while (position < input.size())
    {
        std::size_t hash =
            input.find(
                '#',
                position
            );

        if (hash == std::string::npos)
        {
            break;
        }

        std::size_t semicolon =
            input.find(
                ';',
                hash
            );

        if (semicolon == std::string::npos)
        {
            break;
        }

        int count = std::stoi(
            input.substr(
                position,
                hash - position
            )
        );

        int value = std::stoi(
            input.substr(
                hash + 1,
                semicolon - hash - 1
            )
        );

        for (int i = 0;
             i < count;
             ++i)
        {
            output += static_cast<char>(
                value
            );
        }

        position =
            semicolon + 1;
    }

    return output;
}