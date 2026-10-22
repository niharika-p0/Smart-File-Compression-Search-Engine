#include "../include/RecommendationManager.h"

#include <iostream>
#include <utility>
#include <vector>

RecommendationManager::RecommendationManager(
    const std::vector<Document>& docs
)
    : documents(docs),
      graph(
          static_cast<int>(
              docs.size()
          )
      )
{
}

void RecommendationManager::build(
    double threshold
)
{
    graph.buildGraph(
        documents,
        threshold
    );
}

void RecommendationManager::recommend(
    int documentID,
    int maxRecommendations
) const
{
    if (documentID < 0 ||
        documentID >=
            static_cast<int>(
                documents.size()
            ))
    {
        std::cout
            << "Invalid document ID.\n";

        return;
    }

    std::vector<
        std::pair<int, double>
    > results =
        graph.recommend(
            documentID,
            maxRecommendations
        );

    std::cout
        << "\nDOCUMENT RECOMMENDATIONS\n";

    std::cout
        << "=========================\n";

    std::cout
        << "For: "
        << documents[documentID].fileName
        << "\n\n";

    if (results.empty())
    {
        std::cout
            << "No similar documents found.\n";

        return;
    }

    for (std::size_t i = 0;
         i < results.size();
         ++i)
    {
        int id =
            results[i].first;

        double similarity =
            results[i].second;

        std::cout
            << i + 1
            << ". "
            << documents[id].fileName
            << " | Similarity: "
            << similarity
            << "\n";
    }
}

void RecommendationManager::displayGraph() const
{
    graph.displayGraph(
        documents
    );
}