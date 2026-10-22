#include "../include/QueryProcessor.h"

#include <algorithm>
#include <cctype>
#include <sstream>

QueryProcessor::QueryProcessor(
    SearchEngine& engine,
    SpellCorrector& corrector
)
    : searchEngine(engine),
      spellCorrector(corrector)
{
}

std::string QueryProcessor::normalize(
    const std::string& query
) const
{
    std::string result;

    for (char ch : query)
    {
        if (
            std::isalnum(
                static_cast<unsigned char>(ch)
            )
        )
        {
            result += static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(ch)
                )
            );
        }
        else if (
            std::isspace(
                static_cast<unsigned char>(ch)
            )
        )
        {
            if (
                !result.empty() &&
                result.back() != ' '
            )
            {
                result += ' ';
            }
        }
    }

    if (
        !result.empty() &&
        result.back() == ' '
    )
    {
        result.pop_back();
    }

    return result;
}

std::vector<std::string>
QueryProcessor::tokenize(
    const std::string& query
) const
{
    std::vector<std::string> tokens;

    std::string normalized =
        normalize(query);

    std::stringstream stream(
        normalized
    );

    std::string word;

    while (stream >> word)
    {
        tokens.push_back(word);
    }

    return tokens;
}

bool QueryProcessor::isValidForSpellCorrection(
    const std::string& word
) const
{
    /*
        Avoid correcting completely random strings.

        Very short words are ignored because spell
        correction can easily produce unrelated words.

        Words containing digits are also ignored.
    */

    if (word.length() < 4)
    {
        return false;
    }

    for (char ch : word)
    {
        if (
            !std::isalpha(
                static_cast<unsigned char>(ch)
            )
        )
        {
            return false;
        }
    }

    return true;
}

std::string QueryProcessor::correctQuery(
    const std::string& query
) const
{
    std::vector<std::string> words =
        tokenize(query);

    if (words.empty())
    {
        return "";
    }

    std::string corrected;

    bool correctionMade = false;

    for (const auto& word : words)
    {
        std::string correctedWord = word;

        if (isValidForSpellCorrection(word))
        {
            std::string candidate =
                spellCorrector.correct(word);

            /*
                Only accept a correction if the
                spell corrector actually produced
                a different word.
            */
            if (
                !candidate.empty() &&
                candidate != word
            )
            {
                correctedWord = candidate;
                correctionMade = true;
            }
        }

        if (!corrected.empty())
        {
            corrected += " ";
        }

        corrected += correctedWord;
    }

    if (!correctionMade)
    {
        return query;
    }

    return corrected;
}

std::vector<SearchResult>
QueryProcessor::process(
    const std::string& query
)
{
    std::string normalized =
        normalize(query);

    if (normalized.empty())
    {
        return {};
    }

    /*
        ------------------------------------------------
        STEP 1
        Search the original query.
        ------------------------------------------------
    */

    std::vector<SearchResult> results =
        searchEngine.search(
            normalized
        );

    if (!results.empty())
    {
        return results;
    }

    /*
        ------------------------------------------------
        STEP 2
        Try spell correction only after the original
        query produced no results.
        ------------------------------------------------
    */

    std::string corrected =
        correctQuery(normalized);

    /*
        If no safe correction exists,
        return empty results.
    */
    if (
        corrected.empty() ||
        corrected == normalized
    )
    {
        return {};
    }

    /*
        ------------------------------------------------
        STEP 3
        Search corrected query.
        ------------------------------------------------
    */

    results =
        searchEngine.search(
            corrected
        );

    return results;
}

std::vector<std::string>
QueryProcessor::autocomplete(
    const std::string& prefix,
    int limit
)
{
    return searchEngine.autocomplete(
        normalize(prefix),
        limit
    );
}
