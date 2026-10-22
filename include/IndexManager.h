#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include <string>
#include <vector>

#include "FileManager.h"

class IndexManager
{
private:

    std::string indexDirectory;

public:

    explicit IndexManager(
        const std::string& directory = "indexes"
    );

    void initialize();

    bool saveDocumentIndex(
        const std::vector<Document>& documents
    );

    bool loadDocumentIndex(
        std::vector<Document>& documents
    );

    bool saveTrieIndex(
        const std::vector<
            std::pair<std::string, int>
        >& words
    );

    bool saveSuffixArrayIndex(
        const std::vector<Document>& documents
    );

    bool buildIndexes(
        const std::vector<Document>& documents,
        const std::vector<
            std::pair<std::string, int>
        >& words
    );

    bool indexExists() const;

    void displayIndexFiles() const;

    void displayIndex(
        const std::vector<Document>& documents
    ) const;

    bool clearIndex();
};

#endif