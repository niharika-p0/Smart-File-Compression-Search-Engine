#include "../include/InvertedIndex.h"

#include <algorithm>
#include <iostream>
#include <utility>

void InvertedIndex::addToken(
    const std::string& token,
    int documentID,
    int position
)
{
    if (token.empty())
    {
        return;
    }

    auto& postings = index[token];

    if (
        postings.empty() ||
        postings.back().documentID != documentID
    )
    {
        Posting posting;

        posting.documentID = documentID;
        posting.positions.push_back(position);

        postings.push_back(
            std::move(posting)
        );
    }
    else
    {
        postings.back()
            .positions
            .push_back(position);
    }
}

void InvertedIndex::addDocument(
    int documentID,
    const std::vector<std::string>& tokens
)
{
    for (
        int position = 0;
        position < static_cast<int>(tokens.size());
        ++position
    )
    {
        addToken(
            tokens[position],
            documentID,
            position
        );
    }
}

bool InvertedIndex::contains(
    const std::string& token
) const
{
    if (token.empty())
    {
        return false;
    }

    return index.find(token) != index.end();
}

const std::vector<Posting>&
InvertedIndex::getPostings(
    const std::string& token
) const
{
    static const std::vector<Posting>
        emptyPostings;

    auto it = index.find(token);

    if (it == index.end())
    {
        return emptyPostings;
    }

    return it->second;
}

int InvertedIndex::getDocumentFrequency(
    const std::string& token
) const
{
    auto it = index.find(token);

    if (it == index.end())
    {
        return 0;
    }

    return static_cast<int>(
        it->second.size()
    );
}

int InvertedIndex::getTermFrequency(
    const std::string& token,
    int documentID
) const
{
    auto it = index.find(token);

    if (it == index.end())
    {
        return 0;
    }

    for (
        const Posting& posting :
        it->second
    )
    {
        if (posting.documentID == documentID)
        {
            return static_cast<int>(
                posting.positions.size()
            );
        }
    }

    return 0;
}

std::size_t
InvertedIndex::vocabularySize() const
{
    return index.size();
}

void InvertedIndex::display() const
{
    std::cout
        << "\n============================================\n";

    std::cout
        << "           INVERTED INDEX\n";

    std::cout
        << "============================================\n";

    for (
        const auto& entry :
        index
    )
    {
        const std::string& word =
            entry.first;

        const std::vector<Posting>& postings =
            entry.second;

        std::cout
            << "\n"
            << word
            << " -> ";

        for (
            const Posting& posting :
            postings
        )
        {
            std::cout
                << "Doc "
                << posting.documentID
                << " [";

            for (
                std::size_t i = 0;
                i < posting.positions.size();
                ++i
            )
            {
                std::cout
                    << posting.positions[i];

                if (
                    i + 1 <
                    posting.positions.size()
                )
                {
                    std::cout << ", ";
                }
            }

            std::cout << "] ";
        }
    }

    std::cout << "\n";
}

void InvertedIndex::clear()
{
    index.clear();
}