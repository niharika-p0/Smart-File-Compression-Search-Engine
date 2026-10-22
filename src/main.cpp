#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "../include/FileManager.h"
#include "../include/ProjectManager.h"
#include "../include/CompressedStorage.h"

int main()
{
    ProjectManager project;

    if (!project.initialize())
    {
        return 1;
    }

    const std::vector<Document>& documents =
        project.getDocuments();

    CompressedStorage storage(
        "data/compressed"
    );

    bool running = true;

    while (running)
    {
        std::cout
            << "\n"
            << "========================================\n"
            << "       SMART FILE SEARCH ENGINE\n"
            << "========================================\n"
            << "1. Search Documents\n"
            << "2. Autocomplete\n"
            << "3. Smart Compression\n"
            << "4. Build Indexes\n"
            << "5. Project Statistics\n"
            << "6. Document Recommendations\n"
            << "7. Display Document Graph\n"
            << "8. Compressed Storage\n"
            << "9. Restore Document\n"
            << "10. Exit\n"
            << "========================================\n";

        std::cout
            << "Enter choice: ";

        int choice;

        if (!(std::cin >> choice))
        {
            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<
                    std::streamsize
                >::max(),
                '\n'
            );

            std::cout
                << "Invalid choice.\n";

            continue;
        }

        std::cin.ignore(
            std::numeric_limits<
                std::streamsize
            >::max(),
            '\n'
        );

        switch (choice)
        {
            case 1:
            {
                std::string query;

                std::cout
                    << "\nEnter search query: ";

                std::getline(
                    std::cin,
                    query
                );

                project.search(
                    query
                );

                break;
            }

            case 2:
            {
                std::string prefix;

                std::cout
                    << "\nEnter prefix: ";

                std::getline(
                    std::cin,
                    prefix
                );

                project.autocomplete(
                    prefix
                );

                break;
            }

            case 3:
            {
                project.compressDocuments();

                break;
            }

            case 4:
            {
                project.buildIndexes();

                break;
            }

            case 5:
            {
                project.displayStatistics();

                break;
            }

            case 6:
            {
                if (documents.empty())
                {
                    std::cout
                        << "No documents available.\n";

                    break;
                }

                int documentID;

                std::cout
                    << "\nAvailable Documents:\n";

                for (const auto& document :
                     documents)
                {
                    std::cout
                        << document.id
                        << " -> "
                        << document.fileName
                        << "\n";
                }

                std::cout
                    << "\nEnter document ID: ";

                std::cin
                    >> documentID;

                std::cin.ignore(
                    std::numeric_limits<
                        std::streamsize
                    >::max(),
                    '\n'
                );

                project.displayRecommendations(
                    documentID,
                    5
                );

                break;
            }

            case 7:
            {
                project.displayDocumentGraph();

                break;
            }

            case 8:
            {
                std::cout
                    << "\nBuilding compressed storage...\n";

                if (storage.storeDocuments(
                        documents))
                {
                    std::cout
                        << "Compressed storage created successfully.\n";
                }
                else
                {
                    std::cout
                        << "Some documents could not be stored.\n";
                }

                storage.displayStorage();

                break;
            }

            case 9:
            {
                if (documents.empty())
                {
                    std::cout
                        << "No documents available.\n";

                    break;
                }

                int documentID;

                std::cout
                    << "\nAvailable Documents:\n";

                for (const auto& document :
                     documents)
                {
                    std::cout
                        << document.id
                        << " -> "
                        << document.fileName
                        << "\n";
                }

                std::cout
                    << "\nEnter document ID: ";

                std::cin
                    >> documentID;

                std::cin.ignore(
                    std::numeric_limits<
                        std::streamsize
                    >::max(),
                    '\n'
                );

                std::string outputPath;

                std::cout
                    << "Enter restore path: ";

                std::getline(
                    std::cin,
                    outputPath
                );

                if (storage.restoreDocument(
                        documentID,
                        outputPath))
                {
                    std::cout
                        << "Document restored successfully.\n";
                }
                else
                {
                    std::cout
                        << "Document restoration failed.\n";
                }

                break;
            }

            case 10:
            {
                running = false;

                std::cout
                    << "\nExiting Smart File Search Engine...\n";

                break;
            }

            default:
            {
                std::cout
                    << "Invalid choice.\n";

                break;
            }
        }
    }

    return 0;
}