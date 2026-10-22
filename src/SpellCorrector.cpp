#include "../include/SpellCorrector.h"

#include <algorithm>
#include <climits>

int SpellCorrector::editDistance(
    const std::string& a,
    const std::string& b
) const
{
    int n =
        static_cast<int>(a.size());

    int m =
        static_cast<int>(b.size());

    std::vector<int> previous(m + 1);
    std::vector<int> current(m + 1);

    for (int j = 0; j <= m; ++j)
    {
        previous[j] = j;
    }

    for (int i = 1; i <= n; ++i)
    {
        current[0] = i;

        for (int j = 1; j <= m; ++j)
        {
            int cost =
                (a[i - 1] == b[j - 1])
                ? 0
                : 1;

            current[j] =
                std::min(
                    {
                        current[j - 1] + 1,
                        previous[j] + 1,
                        previous[j - 1] + cost
                    }
                );
        }

        previous.swap(current);
    }

    return previous[m];
}

void SpellCorrector::build(
    const std::vector<std::string>& words
)
{
    frequency.clear();

    for (const auto& word : words)
    {
        if (!word.empty())
        {
            ++frequency[word];
        }
    }
}

std::vector<std::string>
SpellCorrector::suggestions(
    const std::string& word,
    int maxDistance
) const
{
    std::vector<
        std::pair<std::string, int>
    > candidates;

    for (const auto& entry : frequency)
    {
        int distance =
            editDistance(
                word,
                entry.first
            );

        if (distance <= maxDistance)
        {
            candidates.push_back(
                {
                    entry.first,
                    distance
                }
            );
        }
    }

    std::sort(
        candidates.begin(),
        candidates.end(),
        [this](
            const auto& a,
            const auto& b
        )
        {
            int frequencyA =
                frequency.at(a.first);

            int frequencyB =
                frequency.at(b.first);

            if (a.second != b.second)
            {
                return a.second < b.second;
            }

            if (frequencyA != frequencyB)
            {
                return frequencyA > frequencyB;
            }

            return a.first < b.first;
        }
    );

    std::vector<std::string> result;

    for (const auto& candidate : candidates)
    {
        result.push_back(
            candidate.first
        );
    }

    return result;
}

std::string SpellCorrector::correct(
    const std::string& word
) const
{
    if (frequency.empty())
    {
        return word;
    }

    if (word.empty())
    {
        return word;
    }

    /*
        Exact word already exists.
    */

    if (
        frequency.find(word) !=
        frequency.end()
    )
    {
        return word;
    }

    /*
        Determine maximum allowed edit distance.

        1-3 characters  -> no correction
        4-5 characters  -> distance 1
        6-8 characters  -> distance 2
        9+ characters   -> distance 3
    */

    int wordLength =
        static_cast<int>(word.length());

    int maxAllowedDistance = 0;

    if (wordLength <= 3)
    {
        maxAllowedDistance = 0;
    }
    else if (wordLength <= 5)
    {
        maxAllowedDistance = 1;
    }
    else if (wordLength <= 8)
    {
        maxAllowedDistance = 2;
    }
    else
    {
        maxAllowedDistance = 3;
    }

    /*
        Find the closest valid word.
    */

    std::string bestWord = word;

    int bestDistance = INT_MAX;

    int bestFrequency = -1;

    for (const auto& entry : frequency)
    {
        /*
            Ignore very different word lengths.
        */

        int lengthDifference =
            std::abs(
                static_cast<int>(word.length()) -
                static_cast<int>(entry.first.length())
            );

        if (
            lengthDifference >
            maxAllowedDistance
        )
        {
            continue;
        }

        int distance =
            editDistance(
                word,
                entry.first
            );

        /*
            IMPORTANT:
            Do not accept distant matches.
        */

        if (distance > maxAllowedDistance)
        {
            continue;
        }

        if (distance < bestDistance)
        {
            bestDistance = distance;

            bestWord = entry.first;

            bestFrequency =
                entry.second;
        }
        else if (
            distance == bestDistance &&
            entry.second > bestFrequency
        )
        {
            bestWord = entry.first;

            bestFrequency =
                entry.second;
        }
    }

    /*
        No sufficiently close word found.
        Return the original query.
    */

    if (bestDistance == INT_MAX)
    {
        return word;
    }

    return bestWord;
}