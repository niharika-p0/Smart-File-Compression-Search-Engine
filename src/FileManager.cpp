#include "../include/FileManager.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;

FileManager::FileManager(
    const std::string& path
)
{
    directoryPath = path;
}


long long FileManager::countWords(
    const std::string& text
)
{
    std::stringstream ss(text);

    std::string word;

    long long count = 0;

    while (ss >> word)
    {
        count++;
    }

    return count;
}


std::vector<Document>
FileManager::loadDocuments()
{
    std::vector<Document> documents;

    int documentID = 1;

    if (!fs::exists(directoryPath))
    {
        std::cout
            << "Directory does not exist: "
            << directoryPath
            << std::endl;

        return documents;
    }


    for (
        const auto& entry :
        fs::directory_iterator(directoryPath)
    )
    {
        if (!entry.is_regular_file())
        {
            continue;
        }


        if (entry.path().extension() != ".txt")
        {
            continue;
        }


        std::ifstream file(
            entry.path(),
            std::ios::in
        );


        if (!file.is_open())
        {
            std::cout
                << "Could not open: "
                << entry.path()
                << std::endl;

            continue;
        }


        std::stringstream buffer;

        buffer << file.rdbuf();

        std::string content =
            buffer.str();

        file.close();


        Document document;

        document.id = documentID++;

        document.fileName =
            entry.path()
                .filename()
                .string();

        document.filePath =
            entry.path()
                .string();

        document.content =
            std::move(content);

        document.fileSize =
            static_cast<long long>(
                fs::file_size(
                    entry.path()
                )
            );

        document.wordCount =
            countWords(
                document.content
            );


        documents.push_back(
            std::move(document)
        );
    }


    return documents;
}


void FileManager::displayDocuments(
    const std::vector<Document>& documents
) const
{
    std::cout
        << "\n========================================\n";

    std::cout
        << "          LOADED DOCUMENTS\n";

    std::cout
        << "========================================\n";


    for (const Document& document :
         documents)
    {
        std::cout
            << "\nDocument ID : "
            << document.id;

        std::cout
            << "\nFile Name   : "
            << document.fileName;

        std::cout
            << "\nFile Size   : "
            << document.fileSize
            << " bytes";

        std::cout
            << "\nWord Count  : "
            << document.wordCount;

        std::cout
            << "\n----------------------------------------\n";
    }
}