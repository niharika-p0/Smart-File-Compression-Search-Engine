#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <vector>

#include "Document.h"

class FileManager
{
private:

    std::string directoryPath;

    long long countWords(
        const std::string& text
    );

public:

    FileManager(
        const std::string& path
    );

    std::vector<Document> loadDocuments();

    void displayDocuments(
        const std::vector<Document>& documents
    ) const;
};

#endif