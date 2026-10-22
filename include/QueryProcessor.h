#ifndef QUERYPROCESSOR_H
#define QUERYPROCESSOR_H

#include <string>
#include <vector>

#include "SearchEngine.h"
#include "SpellCorrector.h"

class QueryProcessor
{
private:

    SearchEngine& searchEngine;
    SpellCorrector& spellCorrector;

    std::string normalize(
        const std::string& query
    ) const;

    std::vector<std::string> tokenize(
        const std::string& query
    ) const;

    bool isValidForSpellCorrection(
        const std::string& word
    ) const;

public:

    QueryProcessor(
        SearchEngine& engine,
        SpellCorrector& corrector
    );

    std::vector<SearchResult> process(
        const std::string& query
    );

    std::string correctQuery(
        const std::string& query
    ) const;

    std::vector<std::string> autocomplete(
        const std::string& prefix,
        int limit = 5
    );
};

#endif