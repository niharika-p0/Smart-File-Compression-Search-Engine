#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <string>
#include <vector>
#include <cstddef>

#include "FileManager.h"
#include "SearchEngine.h"
#include "SmartCompressor.h"
#include "SpellCorrector.h"
#include "IndexManager.h"
#include "QueryProcessor.h"
#include "RecommendationManager.h"

class ProjectManager
{
private:

    std::vector<Document> documents;

    FileManager fileManager;

    SearchEngine searchEngine;

    SmartCompressor smartCompressor;

    SpellCorrector spellCorrector;

    IndexManager indexManager;

    QueryProcessor queryProcessor;

    RecommendationManager* recommendationManager;

    void buildSpellCorrector();

public:

    ProjectManager(
        const std::string& documentDirectory =
            "data/documents",
        const std::string& indexDirectory =
            "indexes",
        std::size_t cacheCapacity = 5
    );

    ~ProjectManager();

    bool initialize();

    void search(
        const std::string& query
    );

    void autocomplete(
        const std::string& prefix
    );

    void compressDocuments();

    void buildIndexes();

    void displayStatistics() const;

    void displayRecommendations(
        int documentID,
        int maxRecommendations = 5
    ) const;

    void displayDocumentGraph() const;

    const std::vector<Document>&
    getDocuments() const;
};

#endif