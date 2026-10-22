#include "../include/ProjectManager.h"

#include <cctype>
#include <iostream>
#include <map>
#include <string>
#include <vector>

ProjectManager::ProjectManager(
    const std::string& documentDirectory,
    const std::string& indexDirectory,
    std::size_t cacheCapacity
)
    : fileManager(documentDirectory),
      searchEngine(cacheCapacity),
      indexManager(indexDirectory),
      queryProcessor(
          searchEngine,
          spellCorrector
      ),
      recommendationManager(nullptr)
{
}

ProjectManager::~ProjectManager()
{
    delete recommendationManager;
}

bool ProjectManager::initialize()
{
    documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        std::cout
            << "No documents found.\n";

        return false;
    }

    searchEngine.build(
        documents
    );

    buildSpellCorrector();

    indexManager.initialize();

    delete recommendationManager;

    recommendationManager =
        new RecommendationManager(
            documents
        );

    recommendationManager->build(
        0.20
    );

    return true;
}

void ProjectManager::buildSpellCorrector()
{
    std::vector<std::string> words;

    for (const auto& document :
         documents)
    {
        std::string current;

        for (char ch :
             document.content)
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
                    words.push_back(
                        current
                    );

                    current.clear();
                }
            }
        }

        if (!current.empty())
        {
            words.push_back(
                current
            );
        }
    }

    spellCorrector.build(
        words
    );
}

void ProjectManager::search(
    const std::string& query
)
{
    std::vector<SearchResult>
        results =
            queryProcessor.process(
                query
            );

    searchEngine.displayResults(
        results
    );

    if (results.empty())
    {
        std::cout
            << "No matching documents found.\n";
    }
}

void ProjectManager::autocomplete(
    const std::string& prefix
)
{
    std::vector<std::string>
        suggestions =
            queryProcessor.autocomplete(
                prefix,
                5
            );

    std::cout
        << "\nAUTOCOMPLETE\n";

    std::cout
        << "------------\n";

    if (suggestions.empty())
    {
        std::cout
            << "No suggestions found.\n";

        return;
    }

    for (const auto& suggestion :
         suggestions)
    {
        std::cout
            << suggestion
            << "\n";
    }
}

void ProjectManager::compressDocuments()
{
    std::cout
        << "\nSMART COMPRESSION\n";

    std::cout
        << "=================\n";

    for (const auto& document :
         documents)
    {
        std::string outputFile =
            "data/compressed/" +
            document.fileName +
            ".huff";

        CompressionResult result =
            smartCompressor.compressFile(
                document.filePath,
                outputFile
            );

        smartCompressor.displayResult(
            result
        );
    }
}

void ProjectManager::buildIndexes()
{
    std::map<
        std::string,
        int
    > wordFrequency;

    for (const auto& document :
         documents)
    {
        std::string current;

        for (char ch :
             document.content)
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
                    ++wordFrequency[current];

                    current.clear();
                }
            }
        }

        if (!current.empty())
        {
            ++wordFrequency[current];
        }
    }

    std::vector<
        std::pair<std::string, int>
    > words(
        wordFrequency.begin(),
        wordFrequency.end()
    );

    if (indexManager.buildIndexes(
            documents,
            words
        ))
    {
        std::cout
            << "\nIndexes built successfully.\n";
    }
    else
    {
        std::cout
            << "\nIndex building failed.\n";
    }
}

void ProjectManager::displayStatistics() const
{
    std::cout
        << "\nPROJECT STATISTICS\n";

    std::cout
        << "------------------\n";

    std::cout
        << "Documents : "
        << documents.size()
        << "\n";

    long long totalSize = 0;

    long long totalWords = 0;

    for (const auto& document :
         documents)
    {
        totalSize +=
            document.fileSize;

        totalWords +=
            document.wordCount;
    }

    std::cout
        << "Total Size : "
        << totalSize
        << " bytes\n";

    std::cout
        << "Total Words: "
        << totalWords
        << "\n";
}

void ProjectManager::displayRecommendations(
    int documentID,
    int maxRecommendations
) const
{
    if (recommendationManager == nullptr)
    {
        std::cout
            << "Recommendation system "
               "is not initialized.\n";

        return;
    }

    recommendationManager->recommend(
        documentID,
        maxRecommendations
    );
}

void ProjectManager::displayDocumentGraph() const
{
    if (recommendationManager == nullptr)
    {
        return;
    }

    recommendationManager->displayGraph();
}

const std::vector<Document>&
ProjectManager::getDocuments() const
{
    return documents;
}