#ifndef TEXTPROCESSOR_H
#define TEXTPROCESSOR_H

#include <string>
#include <vector>

class TextProcessor
{
public:

    std::string toLower(
        const std::string& text
    ) const;

    std::vector<std::string> tokenize(
        const std::string& text
    ) const;

    std::string stem(
        const std::string& word
    ) const;

    std::vector<std::string> process(
        const std::string& text
    ) const;
};

#endif