#include <iostream>
#include <vector>
#include <string>
#include <cctype>

#include "../include/FileManager.h"
#include "../include/IndexManager.h"

using namespace std;

int main()
{
    cout << "\n========================================\n";
    cout << "       INDEX PERSISTENCE TEST\n";
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
    // INITIALIZE INDEX MANAGER
    // ========================================

    IndexManager indexManager(
        "indexes"
    );

    indexManager.initialize();

    // ========================================
    // CREATE WORD LIST
    // ========================================

    vector<pair<string, int>> words;

    for (const auto& document : documents)
    {
        string word;

        for (char ch : document.content)
        {
            if (isalnum(
                static_cast<unsigned char>(ch)
            ))
            {
                word += static_cast<char>(
                    tolower(
                        static_cast<unsigned char>(ch)
                    )
                );
            }
            else
            {
                if (!word.empty())
                {
                    words.push_back(
                        {word, document.id}
                    );

                    word.clear();
                }
            }
        }

        if (!word.empty())
        {
            words.push_back(
                {word, document.id}
            );
        }
    }

    // ========================================
    // SAVE INDEXES
    // ========================================

    cout << "\n========================================\n";
    cout << "           SAVING INDEXES\n";
    cout << "========================================\n";

    bool saved =
        indexManager.buildIndexes(
            documents,
            words
        );

    cout << "Save Status : "
         << (saved ? "SUCCESS" : "FAILED")
         << "\n";

    if (!saved)
    {
        return 1;
    }

    // ========================================
    // CHECK INDEX EXISTS
    // ========================================

    bool exists =
        indexManager.indexExists();

    cout << "Index Exists: "
         << (exists ? "YES" : "NO")
         << "\n";

    // ========================================
    // DISPLAY INDEX FILES
    // ========================================

    cout << "\n========================================\n";
    cout << "           INDEX FILES\n";
    cout << "========================================\n";

    indexManager.displayIndexFiles();

    // ========================================
    // LOAD DOCUMENT INDEX
    // ========================================

    vector<Document> loadedDocuments;

    bool loaded =
        indexManager.loadDocumentIndex(
            loadedDocuments
        );

    cout << "\n========================================\n";
    cout << "        LOADING DOCUMENT INDEX\n";
    cout << "========================================\n";

    cout << "Load Status : "
         << (loaded ? "SUCCESS" : "FAILED")
         << "\n";

    cout << "Loaded Documents : "
         << loadedDocuments.size()
         << "\n";

    // ========================================
    // VERIFY DOCUMENT COUNT
    // ========================================

    bool countMatch =
        (
            loadedDocuments.size()
            ==
            documents.size()
        );

    cout << "Document Count : "
         << (
                countMatch
                ? "MATCH"
                : "MISMATCH"
            )
         << "\n";

    // ========================================
    // VERIFY DOCUMENT DETAILS
    // ========================================

    bool detailsMatch = true;

    if (loadedDocuments.size() ==
        documents.size())
    {
        for (size_t i = 0;
             i < documents.size();
             i++)
        {
            if (
                loadedDocuments[i].id
                != documents[i].id
                ||
                loadedDocuments[i].fileName
                != documents[i].fileName
                ||
                loadedDocuments[i].filePath
                != documents[i].filePath
            )
            {
                detailsMatch = false;
                break;
            }
        }
    }
    else
    {
        detailsMatch = false;
    }

    cout << "Document Details: "
         << (
                detailsMatch
                ? "MATCH"
                : "MISMATCH"
            )
         << "\n";

    // ========================================
    // DISPLAY LOADED INDEX
    // ========================================

    cout << "\n========================================\n";
    cout << "        LOADED INDEX INFORMATION\n";
    cout << "========================================\n";

    indexManager.displayIndex(
        loadedDocuments
    );

    // ========================================
    // FINAL RESULT
    // ========================================

    cout << "\n========================================\n";
    cout << "       INDEX PERSISTENCE SUMMARY\n";
    cout << "========================================\n";

    cout << "Original Documents : "
         << documents.size()
         << "\n";

    cout << "Loaded Documents   : "
         << loadedDocuments.size()
         << "\n";

    cout << "Indexes Saved      : "
         << (saved ? "YES" : "NO")
         << "\n";

    cout << "Indexes Loaded     : "
         << (loaded ? "YES" : "NO")
         << "\n";

    cout << "Data Integrity     : "
         << (
                countMatch && detailsMatch
                ? "PASSED"
                : "FAILED"
            )
         << "\n";

    if (
        saved &&
        loaded &&
        countMatch &&
        detailsMatch
    )
    {
        cout << "\nSTATUS : PASSED\n";
    }
    else
    {
        cout << "\nSTATUS : FAILED\n";
    }

    cout << "========================================\n";
    cout << "INDEX PERSISTENCE TEST COMPLETED\n";
    cout << "========================================\n";

    return 0;
}