#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <unordered_map>
#include <memory>

class Huffman
{
private:

    struct Node
    {
        unsigned char character;
        long long frequency;

        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        Node(
            unsigned char ch,
            long long freq
        );

        Node(
            std::shared_ptr<Node> leftNode,
            std::shared_ptr<Node> rightNode
        );

        bool isLeaf() const;
    };

    struct Compare
    {
        bool operator()(
            const std::shared_ptr<Node>& a,
            const std::shared_ptr<Node>& b
        ) const;
    };

    void buildCodes(
        const std::shared_ptr<Node>& node,
        const std::string& code,
        std::unordered_map<unsigned char, std::string>& codes
    ) const;

public:

    std::string encode(
        const std::string& input
    ) const;

    std::string decode(
        const std::string& input
    ) const;
};

#endif