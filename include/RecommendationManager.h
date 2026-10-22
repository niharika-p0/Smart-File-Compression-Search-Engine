#ifndef RECOMMENDATIONMANAGER_H
#define RECOMMENDATIONMANAGER_H

#include <string>
#include <vector>

#include "FileManager.h"
#include "Graph.h"

class RecommendationManager
{
private:

    std::vector<Document> documents;

    Graph graph;

public:

    explicit RecommendationManager(
        const std::vector<Document>& docs
    );

    void build(
        double threshold = 0.20
    );

    void recommend(
        int documentID,
        int maxRecommendations = 5
    ) const;

    void displayGraph() const;
};

#endif