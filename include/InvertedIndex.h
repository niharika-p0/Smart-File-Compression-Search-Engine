#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <string>
#include <vector>
#include <unordered_map>

struct Posting
{
    int documentID;
    std::vector<int> positions;
};

class InvertedIndex
{
private:
    std::unordered_map<
        std::string,
        std::vector<Posting>
    > index;

public:
    void addToken(
        const std::string& token,
        int documentID,
        int position
    );

    void addDocument(
        int documentID,
        const std::vector<std::string>& tokens
    );

    bool contains(
        const std::string& token
    ) const;

    const std::vector<Posting>& getPostings(
        const std::string& token
    ) const;

    int getDocumentFrequency(
        const std::string& token
    ) const;

    int getTermFrequency(
        const std::string& token,
        int documentID
    ) const;

    std::size_t vocabularySize() const;

    void display() const;

    void clear();
};

#endif