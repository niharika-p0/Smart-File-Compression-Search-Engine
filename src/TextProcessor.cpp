#include "../include/TextProcessor.h"

#include <algorithm>
#include <cctype>

std::string TextProcessor::toLower(
    const std::string& text
) const
{
    std::string result = text;

    std::transform(
        result.begin(),
        result.end(),
        result.begin(),
        [](unsigned char ch)
        {
            return static_cast<char>(
                std::tolower(ch)
            );
        }
    );

    return result;
}

std::vector<std::string>
TextProcessor::tokenize(
    const std::string& text
) const
{
    std::vector<std::string> tokens;

    std::string current;

    for (char ch : text)
    {
        if (std::isalnum(
                static_cast<unsigned char>(ch)))
        {
            current +=
                static_cast<char>(
                    std::tolower(
                        static_cast<unsigned char>(
                            ch
                        )
                    )
                );
        }
        else
        {
            if (!current.empty())
            {
                tokens.push_back(current);
                current.clear();
            }
        }
    }

    if (!current.empty())
    {
        tokens.push_back(current);
    }

    return tokens;
}

std::string TextProcessor::stem(
    const std::string& word
) const
{
    std::string result = word;

    if (result.length() > 5 &&
        result.substr(
            result.length() - 3
        ) == "ing")
    {
        result.erase(
            result.length() - 3
        );
    }
    else if (
        result.length() > 4 &&
        result.substr(
            result.length() - 2
        ) == "ed"
    )
    {
        result.erase(
            result.length() - 2
        );
    }
    else if (
        result.length() > 4 &&
        result.substr(
            result.length() - 3
        ) == "ies"
    )
    {
        result.erase(
            result.length() - 3
        );

        result += "y";
    }
    else if (
        result.length() > 3 &&
        result.back() == 's'
    )
    {
        result.pop_back();
    }

    return result;
}

std::vector<std::string>
TextProcessor::process(
    const std::string& text
) const
{
    std::vector<std::string> tokens =
        tokenize(text);

    std::vector<std::string> processed;

    for (const auto& token : tokens)
    {
        std::string stemmed =
            stem(token);

        if (!stemmed.empty())
        {
            processed.push_back(
                stemmed
            );
        }
    }

    return processed;
}