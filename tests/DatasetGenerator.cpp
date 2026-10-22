#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

int main()
{
    const std::string directory =
        "data/documents";

    fs::create_directories(directory);

    std::vector<std::string> topics =
    {
        "python",
        "database",
        "machine learning",
        "data structures",
        "algorithms",
        "computer networks",
        "operating systems",
        "artificial intelligence",
        "web development",
        "software engineering",
        "data science",
        "cloud computing",
        "cyber security",
        "information retrieval",
        "file compression",
        "search engine",
        "natural language processing",
        "computer architecture",
        "distributed systems",
        "object oriented programming"
    };

    std::vector<std::string> sentences =
    {
        "Python is widely used for software development and data science.",
        "Database systems store and manage structured information.",
        "Machine learning algorithms learn useful patterns from data.",
        "Data structures provide efficient ways to organize information.",
        "Algorithms solve computational problems using systematic procedures.",
        "Computer networks allow devices to communicate and share resources.",
        "Operating systems manage hardware and software resources.",
        "Artificial intelligence enables machines to perform intelligent tasks.",
        "Web development uses frontend and backend technologies.",
        "Software engineering focuses on designing reliable software systems.",
        "Data science combines statistics programming and analytical methods.",
        "Cloud computing provides scalable computing and storage resources.",
        "Cyber security protects systems networks and information from attacks.",
        "Information retrieval finds relevant documents for user queries.",
        "File compression reduces the storage space required by documents.",
        "Search engines use indexing and ranking algorithms to find information.",
        "Natural language processing helps computers understand human language.",
        "Computer architecture describes how hardware components work together.",
        "Distributed systems coordinate multiple computers to perform tasks.",
        "Object oriented programming organizes software using classes and objects."
    };

    int documentCount = 40;

    for (int i = 1;
         i <= documentCount;
         ++i)
    {
        std::string fileName =
            directory +
            "/document" +
            std::to_string(i) +
            ".txt";

        std::ofstream file(
            fileName
        );

        if (!file)
        {
            std::cout
                << "Failed to create: "
                << fileName
                << "\n";

            continue;
        }

        int topicIndex =
            (i - 1) %
            static_cast<int>(
                topics.size()
            );

        file
            << "Document "
            << i
            << "\n\n";

        file
            << "Topic: "
            << topics[topicIndex]
            << "\n\n";

        for (int j = 0;
             j < 15;
             ++j)
        {
            int sentenceIndex =
                (
                    topicIndex +
                    j
                ) %
                static_cast<int>(
                    sentences.size()
                );

            file
                << sentences[sentenceIndex]
                << " ";

            file
                << "This document contains information "
                   "about "
                << topics[topicIndex]
                << " and its practical applications. ";

            file
                << "Efficient indexing searching and "
                   "storage are important for large "
                   "text collections.\n";
        }

        file.close();

        std::cout
            << "Created: "
            << fileName
            << "\n";
    }

    std::cout
        << "\n================================\n"
        << "Dataset generation completed.\n"
        << "Documents created: "
        << documentCount
        << "\n"
        << "================================\n";

    return 0;
}