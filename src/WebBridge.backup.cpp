
#include "../include/FileManager.h"
#include "../include/SearchEngine.h"
#include "../include/SpellCorrector.h"
#include "../include/QueryProcessor.h"
#include "../include/SmartCompressor.h"
#include "../include/CompressionPipeline.h"

#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <iomanip>

namespace fs = std::filesystem;

std::string jsonEscape(const std::string& text)
{
    std::string result;

    for (unsigned char ch : text)
    {
        switch (ch)
        {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;

            default:
                if (ch < 32)
                {
                    result += ' ';
                }
                else
                {
                    result += static_cast<char>(ch);
                }
        }
    }

    return result;
}

void printError(const std::string& message)
{
    std::cout
        << "{\"success\":false,\"error\":\""
        << jsonEscape(message)
        << "\"}";
}

void buildSpellCorrector(
    SpellCorrector& spellCorrector,
    const std::vector<Document>& documents)
{
    std::vector<std::string> words;

    for (const auto& document : documents)
    {
        std::string word;

        for (unsigned char ch : document.content)
        {
            if (std::isalpha(ch))
            {
                word += static_cast<char>(std::tolower(ch));
            }
            else if (!word.empty())
            {
                words.push_back(word);
                word.clear();
            }
        }

        if (!word.empty())
        {
            words.push_back(word);
        }
    }

    spellCorrector.build(words);
}

bool readBinaryFile(
    const fs::path& path,
    std::string& content)
{
    std::ifstream file(path, std::ios::binary);

    if (!file)
    {
        return false;
    }

    content.assign(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());

    return file.good() || file.eof();
}

bool writeBinaryFile(
    const fs::path& path,
    const std::string& content)
{
    if (path.has_parent_path())
    {
        std::error_code error;
        fs::create_directories(path.parent_path(), error);

        if (error)
        {
            return false;
        }
    }

    std::ofstream file(
        path,
        std::ios::binary | std::ios::trunc);

    if (!file)
    {
        return false;
    }

    file.write(
        content.data(),
        static_cast<std::streamsize>(content.size()));

    return file.good();
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printError(
            "Usage: WebBridge.exe <search|autocomplete|status|compress|restore> ..."
        );
        return 1;
    }

    const std::string command = argv[1];

    /*
        STATUS
        Usage:
        WebBridge.exe status "data/documents"
    */
    if (command == "status")
    {
        const fs::path directory =
            argc >= 3 ? fs::path(argv[2])
                      : fs::path("data/documents");

        if (!fs::exists(directory) ||
            !fs::is_directory(directory))
        {
            printError("Document directory not found");
            return 1;
        }

        std::size_t count = 0;

        for (const auto& entry : fs::directory_iterator(directory))
        {
            if (entry.is_regular_file() &&
                entry.path().extension() == ".txt")
            {
                ++count;
            }
        }

        std::cout
            << "{\"success\":true,"
            << "\"status\":\"ready\","
            << "\"engine\":\"C++\","
            << "\"document_count\":" << count << ","
            << "\"document_directory\":\""
            << jsonEscape(directory.string())
            << "\"}";

        return 0;
    }

    /*
        COMPRESS
        Usage:
        WebBridge.exe compress input.txt output.huff
    */
    if (command == "compress")
    {
        if (argc < 4)
        {
            printError(
                "Usage: WebBridge.exe compress <input_file> <output_file>"
            );
            return 1;
        }

        const std::string inputFile = argv[2];
        const std::string outputFile = argv[3];

        if (!fs::exists(inputFile) ||
            !fs::is_regular_file(inputFile))
        {
            printError("Input file not found");
            return 1;
        }

        SmartCompressor compressor;

        const CompressionResult result =
            compressor.compressFile(inputFile, outputFile);

        std::cout
            << std::fixed << std::setprecision(2)
            << "{"
            << "\"success\":true,"
            << "\"original_file\":\""
            << jsonEscape(result.originalFile) << "\","
            << "\"compressed_file\":\""
            << jsonEscape(result.compressedFile) << "\","
            << "\"original_size\":" << result.originalSize << ","
            << "\"compressed_size\":" << result.compressedSize << ","
            << "\"compression_percentage\":"
            << result.compressionPercentage << ","
            << "\"compressed\":"
            << (result.compressed ? "true" : "false")
            << "}";

        return 0;
    }

    /*
        RESTORE
        Usage:
        WebBridge.exe restore input.huff restored.txt
    */
    if (command == "restore")
    {
        if (argc < 4)
        {
            printError(
                "Usage: WebBridge.exe restore <compressed_file> <output_file>"
            );
            return 1;
        }

        const std::string inputFile = argv[2];
        const std::string outputFile = argv[3];

        std::string compressedData;

        if (!readBinaryFile(inputFile, compressedData))
        {
            printError("Could not read compressed file");
            return 1;
        }

        try
        {
            CompressionPipeline pipeline;

            const std::string restoredData =
                pipeline.decompress(compressedData);

            if (compressedData.size() > 0 &&
                restoredData.empty())
            {
                printError(
                    "Restoration produced empty output; compressed data may be invalid"
                );
                return 1;
            }

            if (!writeBinaryFile(outputFile, restoredData))
            {
                printError("Could not write restored file");
                return 1;
            }

            std::cout
                << "{"
                << "\"success\":true,"
                << "\"compressed_file\":\""
                << jsonEscape(inputFile) << "\","
                << "\"restored_file\":\""
                << jsonEscape(outputFile) << "\","
                << "\"restored_size\":"
                << restoredData.size()
                << "}";

            return 0;
        }
        catch (const std::exception& error)
        {
            printError(
                std::string("Restoration failed: ") + error.what()
            );
            return 1;
        }
    }

    /*
        SEARCH AND AUTOCOMPLETE
        Usage:
        WebBridge.exe search machine "data/documents"
        WebBridge.exe autocomplete dat "data/documents"
    */
    if (command != "search" &&
        command != "autocomplete")
    {
        printError("Unknown command");
        return 1;
    }

    if (argc < 3)
    {
        printError("A search query or autocomplete prefix is required");
        return 1;
    }

    const std::string query = argv[2];

    const std::string documentDirectory =
        argc >= 4 ? argv[3] : "data/documents";

    FileManager fileManager(documentDirectory);

    std::vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        printError("No documents found");
        return 1;
    }

    SearchEngine searchEngine(5);
    searchEngine.build(documents);

    SpellCorrector spellCorrector;
    buildSpellCorrector(spellCorrector, documents);

    QueryProcessor queryProcessor(
        searchEngine,
        spellCorrector);

    if (command == "search")
    {
        const std::vector<SearchResult> results =
            queryProcessor.process(query);

        std::cout
            << "{"
            << "\"success\":true,"
            << "\"query\":\"" << jsonEscape(query) << "\","
            << "\"document_directory\":\""
            << jsonEscape(documentDirectory) << "\","
            << "\"document_count\":" << documents.size() << ","
            << "\"result_count\":" << results.size() << ","
            << "\"results\":[";

        for (std::size_t i = 0; i < results.size(); ++i)
        {
            if (i > 0)
            {
                std::cout << ",";
            }

            const auto& result = results[i];

            std::cout
                << "{"
                << "\"document_id\":" << result.documentID << ","
                << "\"file_name\":\""
                << jsonEscape(result.fileName) << "\","
                << "\"score\":" << result.score
                << "}";
        }

        std::cout << "]}";
        return 0;
    }

    if (command == "autocomplete")
    {
        const std::vector<std::string> suggestions =
            queryProcessor.autocomplete(query, 5);

        std::cout
            << "{"
            << "\"success\":true,"
            << "\"query\":\"" << jsonEscape(query) << "\","
            << "\"suggestions\":[";

        for (std::size_t i = 0; i < suggestions.size(); ++i)
        {
            if (i > 0)
            {
                std::cout << ",";
            }

            std::cout
                << "\""
                << jsonEscape(suggestions[i])
                << "\"";
        }

        std::cout << "]}";
        return 0;
    }

    return 0;
}
