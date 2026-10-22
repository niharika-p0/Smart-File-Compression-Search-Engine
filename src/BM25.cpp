#include "../include/BM25.h"

#include <algorithm>
#include <cmath>

BM25::BM25(
    const std::vector<Document>& documents,
    const InvertedIndex& invertedIndex,
    double k1,
    double b
)
{
    this->documents = &documents;
    this->invertedIndex = &invertedIndex;
    this->k1 = k1;
    this->b = b;

    averageDocumentLength = 0.0;

    calculateAverageDocumentLength();
}

void BM25::calculateAverageDocumentLength()
{
    if (
        documents == nullptr ||
        documents->empty()
    )
    {
        averageDocumentLength = 0.0;
        return;
    }

    long long totalLength = 0;

    for (const Document& document : *documents)
    {
        totalLength += document.wordCount;
    }

    averageDocumentLength =
        static_cast<double>(totalLength) /
        static_cast<double>(documents->size());
}

const Document* BM25::findDocument(
    int documentID
) const
{
    if (documents == nullptr)
    {
        return nullptr;
    }

    for (const Document& document : *documents)
    {
        if (document.id == documentID)
        {
            return &document;
        }
    }

    return nullptr;
}

double BM25::calculateIDF(
    const std::string& term
) const
{
    if (
        documents == nullptr ||
        documents->empty() ||
        invertedIndex == nullptr
    )
    {
        return 0.0;
    }

    int totalDocuments =
        static_cast<int>(
            documents->size()
        );

    int documentFrequency =
        invertedIndex->getDocumentFrequency(
            term
        );

    if (documentFrequency <= 0)
    {
        return 0.0;
    }

    double numerator =
        static_cast<double>(
            totalDocuments -
            documentFrequency
        ) + 0.5;

    double denominator =
        static_cast<double>(
            documentFrequency
        ) + 0.5;

    return std::log(
        (numerator / denominator) + 1.0
    );
}

double BM25::calculateTermScore(
    const std::string& term,
    const Document& document
) const
{
    if (invertedIndex == nullptr)
    {
        return 0.0;
    }

    int termFrequency =
        invertedIndex->getTermFrequency(
            term,
            document.id
        );

    if (termFrequency <= 0)
    {
        return 0.0;
    }

    double idf =
        calculateIDF(term);

    if (idf <= 0.0)
    {
        return 0.0;
    }

    double documentLength =
        static_cast<double>(
            document.wordCount
        );

    if (averageDocumentLength <= 0.0)
    {
        return 0.0;
    }

    double lengthNormalization =
        1.0 -
        b +
        b *
        (
            documentLength /
            averageDocumentLength
        );

    double denominator =
        static_cast<double>(
            termFrequency
        ) +
        k1 *
        lengthNormalization;

    if (denominator <= 0.0)
    {
        return 0.0;
    }

    double numerator =
        static_cast<double>(
            termFrequency
        ) *
        (k1 + 1.0);

    return
        idf *
        (
            numerator /
            denominator
        );
}

double BM25::score(
    const std::string& term,
    int documentID
) const
{
    const Document* document =
        findDocument(documentID);

    if (document == nullptr)
    {
        return 0.0;
    }

    return calculateTermScore(
        term,
        *document
    );
}

double BM25::scoreDocument(
    const std::vector<std::string>& queryTerms,
    int documentID
) const
{
    if (queryTerms.empty())
    {
        return 0.0;
    }

    double totalScore = 0.0;

    for (const std::string& term : queryTerms)
    {
        totalScore +=
            score(
                term,
                documentID
            );
    }

    return totalScore;
}

std::vector<BM25Result> BM25::rank(
    const std::vector<std::string>& queryTerms
) const
{
    std::vector<BM25Result> results;

    if (
        queryTerms.empty() ||
        documents == nullptr ||
        invertedIndex == nullptr
    )
    {
        return results;
    }

    std::vector<int> candidateDocuments;

    /*
        Only documents containing at least one
        query term become BM25 candidates.
    */

    for (const std::string& term : queryTerms)
    {
        const std::vector<Posting>& postings =
            invertedIndex->getPostings(term);

        for (const Posting& posting : postings)
        {
            if (
                std::find(
                    candidateDocuments.begin(),
                    candidateDocuments.end(),
                    posting.documentID
                )
                ==
                candidateDocuments.end()
            )
            {
                candidateDocuments.push_back(
                    posting.documentID
                );
            }
        }
    }

    /*
        If no query term exists in the index,
        return NO results.
    */

    if (candidateDocuments.empty())
    {
        return results;
    }

    for (int documentID : candidateDocuments)
    {
        double documentScore =
            scoreDocument(
                queryTerms,
                documentID
            );

        if (documentScore > 0.0)
        {
            BM25Result result;

            result.documentID =
                documentID;

            result.score =
                documentScore;

            results.push_back(
                result
            );
        }
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const BM25Result& a,
           const BM25Result& b)
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

double BM25::getAverageDocumentLength() const
{
    return averageDocumentLength;
}