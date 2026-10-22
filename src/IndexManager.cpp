#include "../include/IndexManager.h"
#include "../include/SuffixArray.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

IndexManager::IndexManager(
    const std::string& directory
)
    : indexDirectory(directory)
{
}

void IndexManager::initialize()
{
    fs::create_directories(
        indexDirectory
    );
}

bool IndexManager::saveDocumentIndex(
    const std::vector<Document>& documents
)
{
    initialize();

    std::ofstream file(
        indexDirectory +
        "/document_index.txt"
    );

    if (!file)
    {
        return false;
    }

    for (const auto& document :
         documents)
    {
        file
            << document.id
            << "|"
            << document.fileName
            << "|"
            << document.filePath
            << "|"
            << document.fileSize
            << "|"
            << document.wordCount
            << "\n";
    }

    return true;
}

bool IndexManager::loadDocumentIndex(
    std::vector<Document>& documents
)
{
    std::ifstream file(
        indexDirectory +
        "/document_index.txt"
    );

    if (!file)
    {
        return false;
    }

    documents.clear();

    std::string line;

    while (std::getline(file, line))
    {
        std::vector<std::string> fields;

        std::string current;

        for (char ch : line)
        {
            if (ch == '|')
            {
                fields.push_back(current);
                current.clear();
            }
            else
            {
                current += ch;
            }
        }

        fields.push_back(current);

        if (fields.size() != 5)
        {
            continue;
        }

        Document document;

        document.id =
            std::stoi(fields[0]);

        document.fileName =
            fields[1];

        document.filePath =
            fields[2];

        document.fileSize =
            std::stoll(fields[3]);

        document.wordCount =
            std::stoi(fields[4]);

        std::ifstream contentFile(
            document.filePath
        );

        if (contentFile)
        {
            document.content =
                std::string(
                    (
                        std::istreambuf_iterator<char>(
                            contentFile
                        )
                    ),
                    std::istreambuf_iterator<char>()
                );
        }

        documents.push_back(
            document
        );
    }

    return true;
}

bool IndexManager::saveTrieIndex(
    const std::vector<
        std::pair<std::string, int>
    >& words
)
{
    initialize();

    std::ofstream file(
        indexDirectory +
        "/trie_index.txt"
    );

    if (!file)
    {
        return false;
    }

    for (const auto& word : words)
    {
        file
            << word.first
            << "|"
            << word.second
            << "\n";
    }

    return true;
}

bool IndexManager::saveSuffixArrayIndex(
    const std::vector<Document>& documents
)
{
    initialize();

    std::ofstream file(
        indexDirectory +
        "/suffix_index.txt"
    );

    if (!file)
    {
        return false;
    }

    for (const auto& document :
         documents)
    {
        SuffixArray suffixArray(
            document.content
        );

        file
            << "DOCUMENT|"
            << document.id
            << "\n";

        const auto& suffixes =
            suffixArray.getSuffixArray();

        for (int position :
             suffixes)
        {
            file
                << position
                << " ";
        }

        file << "\n";
    }

    return true;
}

bool IndexManager::buildIndexes(
    const std::vector<Document>& documents,
    const std::vector<
        std::pair<std::string, int>
    >& words
)
{
    bool documentResult =
        saveDocumentIndex(
            documents
        );

    bool trieResult =
        saveTrieIndex(
            words
        );

    bool suffixResult =
        saveSuffixArrayIndex(
            documents
        );

    return documentResult &&
           trieResult &&
           suffixResult;
}

bool IndexManager::indexExists() const
{
    return
        fs::exists(
            indexDirectory +
            "/document_index.txt"
        )
        &&
        fs::exists(
            indexDirectory +
            "/trie_index.txt"
        )
        &&
        fs::exists(
            indexDirectory +
            "/suffix_index.txt"
        );
}

void IndexManager::displayIndexFiles() const
{
    std::cout
        << "\nINDEX FILES\n";

    std::cout
        << "-----------\n";

    std::cout
        << "Document Index : "
        << (
            fs::exists(
                indexDirectory +
                "/document_index.txt"
            )
            ? "AVAILABLE"
            : "MISSING"
        )
        << "\n";

    std::cout
        << "Trie Index     : "
        << (
            fs::exists(
                indexDirectory +
                "/trie_index.txt"
            )
            ? "AVAILABLE"
            : "MISSING"
        )
        << "\n";

    std::cout
        << "Suffix Index   : "
        << (
            fs::exists(
                indexDirectory +
                "/suffix_index.txt"
            )
            ? "AVAILABLE"
            : "MISSING"
        )
        << "\n";
}

void IndexManager::displayIndex(
    const std::vector<Document>& documents
) const
{
    std::cout
        << "\nDOCUMENT INDEX\n";

    std::cout
        << "--------------\n";

    for (const auto& document :
         documents)
    {
        std::cout
            << "ID: "
            << document.id
            << " | "
            << document.fileName
            << " | Size: "
            << document.fileSize
            << " | Words: "
            << document.wordCount
            << "\n";
    }
}

bool IndexManager::clearIndex()
{
    if (!fs::exists(indexDirectory))
    {
        return true;
    }

    fs::remove_all(
        indexDirectory
    );

    return true;
}