#include "../include/Graph.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_set>

Graph::Graph(
    int numberOfDocuments
)
    : numberOfDocuments(numberOfDocuments)
{
    adjacencyList.resize(
        numberOfDocuments
    );
}

double Graph::calculateJaccardSimilarity(
    const Document& first,
    const Document& second
) const
{
    std::unordered_set<std::string> setA;
    std::unordered_set<std::string> setB;

    auto extractWords =
        [](
            const std::string& text,
            std::unordered_set<std::string>& words
        )
        {
            std::string current;

            for (char ch : text)
            {
                if (std::isalnum(
                        static_cast<unsigned char>(
                            ch
                        )))
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
                        words.insert(current);
                        current.clear();
                    }
                }
            }

            if (!current.empty())
            {
                words.insert(current);
            }
        };

    extractWords(
        first.content,
        setA
    );

    extractWords(
        second.content,
        setB
    );

    if (setA.empty() &&
        setB.empty())
    {
        return 0.0;
    }

    int intersection = 0;

    for (const auto& word : setA)
    {
        if (setB.find(word) !=
            setB.end())
        {
            ++intersection;
        }
    }

    int unionSize =
        static_cast<int>(
            setA.size() +
            setB.size() -
            intersection
        );

    if (unionSize == 0)
    {
        return 0.0;
    }

    return static_cast<double>(
        intersection
    ) / static_cast<double>(
        unionSize
    );
}

void Graph::buildGraph(
    const std::vector<Document>& documents,
    double threshold
)
{
    numberOfDocuments =
        static_cast<int>(
            documents.size()
        );

    adjacencyList.clear();

    adjacencyList.resize(
        numberOfDocuments
    );

    for (int i = 0;
         i < numberOfDocuments;
         ++i)
    {
        for (int j = i + 1;
             j < numberOfDocuments;
             ++j)
        {
            double similarity =
                calculateJaccardSimilarity(
                    documents[i],
                    documents[j]
                );

            if (similarity >= threshold)
            {
                adjacencyList[i].push_back(
                    {
                        j,
                        similarity
                    }
                );

                adjacencyList[j].push_back(
                    {
                        i,
                        similarity
                    }
                );
            }
        }
    }
}

std::vector<
    std::pair<int, double>
>
Graph::recommend(
    int startDocumentID,
    int maxRecommendations
) const
{
    std::vector<
        std::pair<int, double>
    > recommendations;

    if (startDocumentID < 0 ||
        startDocumentID >= numberOfDocuments)
    {
        return recommendations;
    }

    std::vector<bool> visited(
        numberOfDocuments,
        false
    );

    std::queue<int> queue;

    queue.push(
        startDocumentID
    );

    visited[startDocumentID] =
        true;

    while (!queue.empty())
    {
        int current =
            queue.front();

        queue.pop();

        for (const auto& edge :
             adjacencyList[current])
        {
            int neighbor =
                edge.first;

            double similarity =
                edge.second;

            if (!visited[neighbor])
            {
                visited[neighbor] =
                    true;

                recommendations.push_back(
                    {
                        neighbor,
                        similarity
                    }
                );

                queue.push(
                    neighbor
                );
            }
        }
    }

    std::sort(
        recommendations.begin(),
        recommendations.end(),
        [](const auto& a,
           const auto& b)
        {
            if (a.second != b.second)
            {
                return a.second > b.second;
            }

            return a.first < b.first;
        }
    );

    if (
        static_cast<int>(
            recommendations.size()
        ) > maxRecommendations
    )
    {
        recommendations.resize(
            maxRecommendations
        );
    }

    return recommendations;
}

void Graph::displayGraph(
    const std::vector<Document>& documents
) const
{
    std::cout
        << "\nDOCUMENT SIMILARITY GRAPH\n";

    std::cout
        << "==========================\n";

    for (int i = 0;
         i < numberOfDocuments;
         ++i)
    {
        std::cout
            << "\n"
            << documents[i].fileName
            << " -> ";

        if (adjacencyList[i].empty())
        {
            std::cout
                << "No connections";

            continue;
        }

        for (const auto& edge :
             adjacencyList[i])
        {
            int neighbor =
                edge.first;

            double similarity =
                edge.second;

            std::cout
                << documents[neighbor].fileName
                << " (Similarity: "
                << similarity
                << ") ";
        }
    }

    std::cout << "\n";
}