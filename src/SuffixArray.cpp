#include "../include/SuffixArray.h"

#include <algorithm>
#include <cctype>
#include <iostream>


SuffixArray::SuffixArray()
{
}


SuffixArray::SuffixArray(
    const std::string& input
)
{
    build(input);
}


void SuffixArray::build(
    const std::string& input
)
{
    text = input;

    suffixArray.clear();

    if (text.empty())
    {
        return;
    }


    // Create one suffix starting position
    // for every character.
    for (
        int i = 0;
        i < static_cast<int>(text.size());
        i++
    )
    {
        suffixArray.push_back(i);
    }


    // Sort suffix starting positions
    // according to their suffix strings.
    std::sort(
        suffixArray.begin(),
        suffixArray.end(),
        [this](int a, int b)
        {
            return text.substr(a)
                 < text.substr(b);
        }
    );
}


int SuffixArray::compareSuffixWithPattern(
    int suffixPosition,
    const std::string& pattern
) const
{
    int i = suffixPosition;

    int j = 0;

    while (
        i < static_cast<int>(text.size()) &&
        j < static_cast<int>(pattern.size())
    )
    {
        char textChar =
            static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(
                        text[i]
                    )
                )
            );

        char patternChar =
            static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(
                        pattern[j]
                    )
                )
            );


        if (textChar < patternChar)
        {
            return -1;
        }

        if (textChar > patternChar)
        {
            return 1;
        }

        i++;
        j++;
    }


    // Pattern is completely matched.
    if (
        j ==
        static_cast<int>(pattern.size())
    )
    {
        return 0;
    }


    // Suffix ended before pattern.
    return -1;
}


std::vector<int> SuffixArray::search(
    const std::string& pattern
) const
{
    std::vector<int> results;

    if (
        pattern.empty() ||
        suffixArray.empty()
    )
    {
        return results;
    }


    // ----------------------------------------
    // Find first suffix that can match pattern
    // ----------------------------------------

    int left = 0;

    int right =
        static_cast<int>(
            suffixArray.size()
        ) - 1;


    int first = -1;


    while (left <= right)
    {
        int mid =
            left +
            (right - left) / 2;


        int comparison =
            compareSuffixWithPattern(
                suffixArray[mid],
                pattern
            );


        if (comparison >= 0)
        {
            first = mid;
            right = mid - 1;
        }
        else
        {
            left = mid + 1;
        }
    }


    if (first == -1)
    {
        return results;
    }


    // ----------------------------------------
    // Collect all matching suffixes
    // ----------------------------------------

    for (
        int i = first;
        i < static_cast<int>(suffixArray.size());
        i++
    )
    {
        int position =
            suffixArray[i];


        int comparison =
            compareSuffixWithPattern(
                position,
                pattern
            );


        if (comparison != 0)
        {
            break;
        }


        results.push_back(position);
    }


    // Return positions in document order.
    std::sort(
        results.begin(),
        results.end()
    );


    return results;
}


const std::string&
SuffixArray::getText() const
{
    return text;
}


const std::vector<int>&
SuffixArray::getSuffixArray() const
{
    return suffixArray;
}


void SuffixArray::display() const
{
    std::cout
        << "\n============================================\n";

    std::cout
        << "              SUFFIX ARRAY\n";

    std::cout
        << "============================================\n";


    for (
        std::size_t i = 0;
        i < suffixArray.size();
        i++
    )
    {
        int position =
            suffixArray[i];


        std::cout
            << i
            << " -> position "
            << position
            << " -> ";


        std::cout
            << text.substr(position)
            << "\n";
    }
}