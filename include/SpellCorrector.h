#ifndef SPELLCORRECTOR_H
#define SPELLCORRECTOR_H

#include <string>
#include <vector>
#include <unordered_map>

class SpellCorrector
{
private:

    std::unordered_map<
        std::string,
        int
    > frequency;

    int editDistance(
        const std::string& a,
        const std::string& b
    ) const;

public:

    void build(
        const std::vector<std::string>& words
    );

    std::string correct(
        const std::string& word
    ) const;

    std::vector<std::string> suggestions(
        const std::string& word,
        int maxDistance = 2
    ) const;
};

#endif