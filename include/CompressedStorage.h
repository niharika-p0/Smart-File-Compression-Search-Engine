#ifndef COMPRESSEDSTORAGE_H
#define COMPRESSEDSTORAGE_H

#include <string>
#include <vector>
#include <unordered_map>

#include "FileManager.h"
#include "CompressionFileManager.h"

struct StoredDocument
{
    int documentID;
    std::string fileName;
    std::string originalPath;
    std::string compressedPath;
    bool compressed;
};

class CompressedStorage
{
private:

    std::string storageDirectory;

    std::unordered_map<
        int,
        StoredDocument
    > storageMap;

    CompressionFileManager compressionManager;

public:

    explicit CompressedStorage(
        const std::string& directory =
            "data/compressed"
    );

    void initialize();

    bool storeDocument(
        const Document& document
    );

    bool storeDocuments(
        const std::vector<Document>& documents
    );

    bool restoreDocument(
        int documentID,
        const std::string& outputPath
    ) const;

    std::string getStoragePath(
        int documentID
    ) const;

    bool isCompressed(
        int documentID
    ) const;

    void displayStorage() const;

    std::size_t size() const;
};

#endif