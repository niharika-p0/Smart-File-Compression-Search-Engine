#include "../include/SearchEngine.h"

#include <algorithm>
#include <cctype>
#include <iostream>

SearchEngine::SearchEngine(
    std::size_t cacheCapacity
)
    : cache(cacheCapacity)
{
}

std::string SearchEngine::normalize(
    const std::string& text
) const
{
    std::string result;

    for (char ch : text)
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
SearchEngine::tokenize(
    const std::string& text
) const
{
    std::vector<std::string> tokens;

    std::string current;

    for (char ch : text)
    {
        if (
            std::isalnum(
                static_cast<unsigned char>(ch)
            )
        )
        {
            current += static_cast<char>(
                std::tolower(
                    static_cast<unsigned char>(ch)
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

void SearchEngine::build(
    const std::vector<Document>& docs
)
{
    documents = docs;

    invertedIndex.clear();
    suffixArrays.clear();

    for (const auto& document : documents)
    {
        std::vector<std::string> tokens =
            tokenize(document.content);

        invertedIndex.addDocument(
            document.id,
            tokens
        );

        for (const auto& token : tokens)
        {
            trie.insert(token);
        }

        SuffixArray suffixArray(
            normalize(document.content)
        );

        suffixArrays[document.id] =
            suffixArray;
    }

    bm25 = std::make_unique<BM25>(
        documents,
        invertedIndex
    );

    cache.clear();
}

std::vector<SearchResult>
SearchEngine::search(
    const std::string& query
)
{
    std::string normalizedQuery =
        normalize(query);

    if (normalizedQuery.empty())
    {
        return {};
    }

    /*
        Reject queries containing digits.

        Example:
        ghiu9ot
        tt78t765

        These should not be treated as normal
        natural-language search terms.
    */
    for (char ch : normalizedQuery)
    {
        if (
            std::isdigit(
                static_cast<unsigned char>(ch)
            )
        )
        {
            return {};
        }
    }

    std::vector<SearchResult> cachedResults;

    if (
        cache.get(
            normalizedQuery,
            cachedResults
        )
    )
    {
        return cachedResults;
    }

    std::vector<std::string> queryTerms =
        tokenize(normalizedQuery);

    if (queryTerms.empty())
    {
        return {};
    }

    /*
        Check whether every query term exists
        in the inverted index.
    */

    bool allTermsExist = true;

    for (const std::string& term : queryTerms)
    {
        if (!invertedIndex.contains(term))
        {
            allTermsExist = false;
            break;
        }
    }

    /*
        If any exact term does not exist,
        perform substring search.
    */

    if (!allTermsExist)
    {
        std::vector<SearchResult> substringResults =
            substringSearch(normalizedQuery);

        cache.put(
            normalizedQuery,
            substringResults
        );

        return substringResults;
    }

    /*
        Exact term search using BM25.
    */

    if (!bm25)
    {
        return {};
    }

    std::vector<BM25Result> rankedResults =
        bm25->rank(queryTerms);

    std::vector<SearchResult> results;

    /*
        Verify that every candidate document
        actually contains every query term.
    */

    for (
        const BM25Result& rankedResult :
        rankedResults
    )
    {
        const Document* document = nullptr;

        for (const Document& doc : documents)
        {
            if (
                doc.id ==
                rankedResult.documentID
            )
            {
                document = &doc;
                break;
            }
        }

        if (document == nullptr)
        {
            continue;
        }

        bool documentContainsAllTerms = true;

        for (const std::string& term :
             queryTerms)
        {
            if (
                invertedIndex.getTermFrequency(
                    term,
                    document->id
                ) <= 0
            )
            {
                documentContainsAllTerms = false;
                break;
            }
        }

        if (!documentContainsAllTerms)
        {
            continue;
        }

        SearchResult result;

        result.documentID =
            document->id;

        result.fileName =
            document->fileName;

        result.score =
            rankedResult.score;

        results.push_back(result);
    }

    cache.put(
        normalizedQuery,
        results
    );

    return results;
}

std::vector<SearchResult>
SearchEngine::substringSearch(
    const std::string& pattern
) const
{
    std::vector<SearchResult> results;

    std::string normalizedPattern =
        normalize(pattern);

    if (normalizedPattern.empty())
    {
        return results;
    }

    for (const Document& document :
         documents)
    {
        auto it =
            suffixArrays.find(
                document.id
            );

        if (
            it ==
            suffixArrays.end()
        )
        {
            continue;
        }

        std::vector<int> positions =
            it->second.search(
                normalizedPattern
            );

        if (positions.empty())
        {
            continue;
        }

        SearchResult result;

        result.documentID =
            document.id;

        result.fileName =
            document.fileName;

        result.score =
            static_cast<double>(
                positions.size()
            );

        results.push_back(result);
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const SearchResult& a,
           const SearchResult& b)
        {
            if (a.score != b.score)
            {
                return a.score > b.score;
            }

            return a.documentID <
                   b.documentID;
        }
    );

    return results;
}

std::vector<std::string>
SearchEngine::autocomplete(
    const std::string& prefix,
    int limit
) const
{
    std::string normalizedPrefix =
        normalize(prefix);

    if (normalizedPrefix.empty())
    {
        return {};
    }

    std::vector<
        std::pair<std::string, int>
    > trieResults =
        trie.autocomplete(
            normalizedPrefix,
            limit
        );

    std::vector<std::string> results;

    for (const auto& item :
         trieResults)
    {
        results.push_back(
            item.first
        );
    }

    return results;
}

void SearchEngine::displayResults(
    const std::vector<SearchResult>& results
) const
{
    std::cout
        << "\nSEARCH RESULTS\n";

    std::cout
        << "==============\n";

    if (results.empty())
    {
        std::cout
            << "No results found.\n";

        return;
    }

    for (
        std::size_t i = 0;
        i < results.size();
        ++i
    )
    {
        std::cout
            << "\n"
            << i + 1
            << ". "
            << results[i].fileName
            << "\n";

        std::cout
            << "   Document ID: "
            << results[i].documentID
            << "\n";

        std::cout
            << "   Score       : "
            << results[i].score
            << "\n";
    }
}

std::size_t SearchEngine::documentCount() const
{
    return documents.size();
}