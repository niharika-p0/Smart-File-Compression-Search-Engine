#include "../include/BWT.h"

#include <algorithm>
#include <vector>

BWTResult BWT::transform(
    const std::string& text
) const
{
    BWTResult result;

    result.transformed = "";
    result.primaryIndex = 0;

    if (text.empty())
    {
        return result;
    }

    std::string input = text;

    input += '\0';

    int n = static_cast<int>(
        input.size()
    );

    std::vector<int> suffixes(n);

    for (int i = 0; i < n; ++i)
    {
        suffixes[i] = i;
    }

    std::sort(
        suffixes.begin(),
        suffixes.end(),
        [&input, n](int a, int b)
        {
            for (int k = 0; k < n; ++k)
            {
                char ca =
                    input[(a + k) % n];

                char cb =
                    input[(b + k) % n];

                if (ca != cb)
                {
                    return ca < cb;
                }
            }

            return false;
        }
    );

    result.transformed.reserve(n);

    for (int i = 0; i < n; ++i)
    {
        int position = suffixes[i];

        int previous =
            (position - 1 + n) % n;

        result.transformed +=
            input[previous];

        if (position == 0)
        {
            result.primaryIndex = i;
        }
    }

    return result;
}

std::string BWT::inverseTransform(
    const std::string& transformed,
    int primaryIndex
) const
{
    if (transformed.empty())
    {
        return "";
    }

    int n = static_cast<int>(
        transformed.size()
    );

    std::vector<std::string> table(n);

    for (int iteration = 0;
         iteration < n;
         ++iteration)
    {
        for (int i = 0; i < n; ++i)
        {
            table[i] =
                transformed[i] +
                table[i];
        }

        std::sort(
            table.begin(),
            table.end()
        );
    }

    if (primaryIndex < 0 ||
        primaryIndex >= n)
    {
        return "";
    }

    std::string result =
        table[primaryIndex];

    if (!result.empty() &&
        result.back() == '\0')
    {
        result.pop_back();
    }

    return result;
}