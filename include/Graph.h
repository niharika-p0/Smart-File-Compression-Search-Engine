#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>

#include "FileManager.h"

class Graph
{
private:

    int numberOfDocuments;

    std::vector<
        std::vector<
            std::pair<int, double>
        >
    > adjacencyList;

    double calculateJaccardSimilarity(
        const Document& first,
        const Document& second
    ) const;

public:

    explicit Graph(
        int numberOfDocuments
    );

    void buildGraph(
        const std::vector<Document>& documents,
        double threshold = 0.20
    );

    std::vector<
        std::pair<int, double>
    > recommend(
        int startDocumentID,
        int maxRecommendations = 5
    ) const;

    void displayGraph(
        const std::vector<Document>& documents
    ) const;
};

#endif