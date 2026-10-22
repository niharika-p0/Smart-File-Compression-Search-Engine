#include "../include/SearchEngineCLI.h"

#include <iostream>
#include <limits>
#include <string>

SearchEngineCLI::SearchEngineCLI(
    ProjectManager& projectManager
)
    : project(projectManager)
{
}

void SearchEngineCLI::showMenu() const
{
    std::cout
        << "\n==============================\n"
        << " SMART FILE SEARCH ENGINE\n"
        << "==============================\n"
        << "1. Search Documents\n"
        << "2. Autocomplete\n"
        << "3. Smart Compression\n"
        << "4. Build Indexes\n"
        << "5. Project Statistics\n"
        << "6. Exit\n"
        << "==============================\n";
}

void SearchEngineCLI::handleSearch()
{
    std::string query;

    std::cout
        << "\nEnter search query: ";

    std::getline(
        std::cin,
        query
    );

    if (query.empty())
    {
        std::cout
            << "Query cannot be empty.\n";

        return;
    }

    project.search(query);
}

void SearchEngineCLI::handleAutocomplete()
{
    std::string prefix;

    std::cout
        << "\nEnter prefix: ";

    std::getline(
        std::cin,
        prefix
    );

    if (prefix.empty())
    {
        std::cout
            << "Prefix cannot be empty.\n";

        return;
    }

    project.autocomplete(prefix);
}

void SearchEngineCLI::handleCompression()
{
    project.compressDocuments();
}

void SearchEngineCLI::handleStatistics()
{
    project.displayStatistics();
}

void SearchEngineCLI::handleIndexes()
{
    project.buildIndexes();
}

void SearchEngineCLI::run()
{
    bool running = true;

    while (running)
    {
        showMenu();

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
                handleSearch();
                break;

            case 2:
                handleAutocomplete();
                break;

            case 3:
                handleCompression();
                break;

            case 4:
                handleIndexes();
                break;

            case 5:
                handleStatistics();
                break;

            case 6:
                running = false;

                std::cout
                    << "\nExiting...\n";

                break;

            default:
                std::cout
                    << "Invalid choice.\n";
        }
    }
}