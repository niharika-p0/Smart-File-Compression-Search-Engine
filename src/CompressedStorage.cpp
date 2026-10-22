#include "../include/CompressedStorage.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

CompressedStorage::CompressedStorage(
    const std::string& directory
)
    : storageDirectory(directory)
{
    initialize();
}

void CompressedStorage::initialize()
{
    fs::create_directories(
        storageDirectory
    );
}

bool CompressedStorage::storeDocument(
    const Document& document
)
{
    initialize();

    std::string compressedPath =
        storageDirectory +
        "/" +
        document.fileName +
        ".huff";

    CompressionFileResult result =
        compressionManager.compressFile(
            document.filePath,
            compressedPath
        );

    StoredDocument stored;

    stored.documentID =
        document.id;

    stored.fileName =
        document.fileName;

    stored.originalPath =
        document.filePath;

    /*
        Smart compression decision:
        compressed file must actually be
        smaller than the original file.
    */
    if (result.success &&
        result.compressedSize <
            result.originalSize)
    {
        stored.compressedPath =
            compressedPath;

        stored.compressed =
            true;
    }
    else
    {
        stored.compressedPath =
            document.filePath;

        stored.compressed =
            false;

        if (fs::exists(compressedPath))
        {
            fs::remove(
                compressedPath
            );
        }
    }

    storageMap[
        document.id
    ] = stored;

    return result.success;
}

bool CompressedStorage::storeDocuments(
    const std::vector<Document>& documents
)
{
    bool success = true;

    for (const auto& document :
         documents)
    {
        if (!storeDocument(document))
        {
            success = false;
        }
    }

    return success;
}

bool CompressedStorage::restoreDocument(
    int documentID,
    const std::string& outputPath
) const
{
    auto it =
        storageMap.find(
            documentID
        );

    if (it == storageMap.end())
    {
        return false;
    }

    const StoredDocument& document =
        it->second;

    fs::path outputFile(
        outputPath
    );

    if (outputFile.has_parent_path())
    {
        fs::create_directories(
            outputFile.parent_path()
        );
    }

    /*
        If smart compression decided
        not to compress the document,
        simply copy the original file.
    */
    if (!document.compressed)
    {
        std::ifstream input(
            document.originalPath,
            std::ios::binary
        );

        std::ofstream output(
            outputPath,
            std::ios::binary
        );

        if (!input || !output)
        {
            return false;
        }

        output << input.rdbuf();

        return true;
    }

    /*
        Otherwise decompress the
        compressed file.
    */
    return compressionManager.decompressFile(
        document.compressedPath,
        outputPath
    );
}

std::string
CompressedStorage::getStoragePath(
    int documentID
) const
{
    auto it =
        storageMap.find(
            documentID
        );

    if (it == storageMap.end())
    {
        return "";
    }

    return it->second.compressedPath;
}

bool CompressedStorage::isCompressed(
    int documentID
) const
{
    auto it =
        storageMap.find(
            documentID
        );

    if (it == storageMap.end())
    {
        return false;
    }

    return it->second.compressed;
}

void CompressedStorage::displayStorage() const
{
    std::cout
        << "\nCOMPRESSED STORAGE\n";

    std::cout
        << "==================\n";

    if (storageMap.empty())
    {
        std::cout
            << "No documents stored.\n";

        return;
    }

    for (const auto& entry :
         storageMap)
    {
        const StoredDocument& document =
            entry.second;

        std::cout
            << "Document ID : "
            << document.documentID
            << "\n";

        std::cout
            << "File        : "
            << document.fileName
            << "\n";

        std::cout
            << "Storage     : "
            << document.compressedPath
            << "\n";

        std::cout
            << "Compressed  : "
            << (
                document.compressed
                ? "YES"
                : "NO"
            )
            << "\n";

        std::cout
            << "------------------\n";
    }
}

std::size_t
CompressedStorage::size() const
{
    return storageMap.size();
}