#ifndef BM25_H
#define BM25_H

#include <string>
#include <vector>

#include "Document.h"
#include "InvertedIndex.h"


struct BM25Result
{
    int documentID;

    double score;
};


class BM25
{
private:

    const std::vector<Document>* documents;

    const InvertedIndex* invertedIndex;

    double k1;

    double b;

    double averageDocumentLength;


    void calculateAverageDocumentLength();

    const Document* findDocument(
        int documentID
    ) const;


    double calculateIDF(
        const std::string& term
    ) const;


    double calculateTermScore(
        const std::string& term,
        const Document& document
    ) const;


public:

    BM25(
        const std::vector<Document>& documents,
        const InvertedIndex& invertedIndex,
        double k1 = 1.5,
        double b = 0.75
    );


    double score(
        const std::string& term,
        int documentID
    ) const;


    double scoreDocument(
        const std::vector<std::string>& queryTerms,
        int documentID
    ) const;


    std::vector<BM25Result> rank(
        const std::vector<std::string>& queryTerms
    ) const;


    double getAverageDocumentLength() const;
};

#endif