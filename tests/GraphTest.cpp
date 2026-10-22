#include <iostream>
#include <vector>
#include <string>

#include "../include/FileManager.h"
#include "../include/Graph.h"

using namespace std;

int main()
{
    cout << "\n========================================\n";
    cout << "       GRAPH RECOMMENDATION TEST\n";
    cout << "    SMART FILE SEARCH ENGINE\n";
    cout << "========================================\n";

    FileManager fileManager(
        "data/documents"
    );

    vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        cout << "\nNo documents found.\n";
        return 1;
    }

    cout << "\nDocuments Loaded : "
         << documents.size()
         << "\n";

    // ========================================
    // BUILD GRAPH
    // ========================================

    Graph graph(
        static_cast<int>(
            documents.size()
        )
    );

    graph.buildGraph(
        documents,
        0.20
    );

    cout << "\n========================================\n";
    cout << "          DOCUMENT GRAPH\n";
    cout << "========================================\n";

    graph.displayGraph(
        documents
    );

    // ========================================
    // RECOMMENDATIONS
    // ========================================

    int startDocumentID =
        documents[0].id;

    vector<pair<int, double>> recommendations =
        graph.recommend(
            startDocumentID,
            5
        );

    cout << "\n========================================\n";
    cout << "        RECOMMENDATIONS\n";
    cout << "========================================\n";

    cout << "Starting Document : "
         << startDocumentID
         << "\n";

    cout << "\nRecommended Documents:\n";

    if (recommendations.empty())
    {
        cout << "No recommendations found.\n";
    }
    else
    {
        int count = 1;

        for (const auto& recommendation :
             recommendations)
        {
            int documentID =
                recommendation.first;

            double similarity =
                recommendation.second;

            string fileName =
                "Unknown";

            for (const auto& document :
                 documents)
            {
                if (document.id == documentID)
                {
                    fileName =
                        document.fileName;
                    break;
                }
            }

            cout
                << count
                << ". "
                << fileName
                << "\n";

            cout
                << "   Document ID : "
                << documentID
                << "\n";

            cout
                << "   Similarity  : "
                << similarity
                << "\n";

            count++;
        }
    }

    // ========================================
    // TEST SUMMARY
    // ========================================

    cout << "\n========================================\n";
    cout << "       GRAPH TEST SUMMARY\n";
    cout << "========================================\n";

    cout << "Documents Tested : "
         << documents.size()
         << "\n";

    cout << "Threshold        : "
         << 0.20
         << "\n";

    cout << "Recommendations   : "
         << recommendations.size()
         << "\n";

    if (!recommendations.empty())
    {
        cout << "Status            : PASSED\n";
    }
    else
    {
        cout << "Status            : CHECK\n";
    }

    cout << "========================================\n";
    cout << "GRAPH TEST COMPLETED\n";
    cout << "========================================\n";

    return 0;
}