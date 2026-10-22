#include "../include/Huffman.h"

#include <queue>
#include <sstream>
#include <vector>
#include <stdexcept>

Huffman::Node::Node(
    unsigned char ch,
    long long freq
)
    : character(ch),
      frequency(freq),
      left(nullptr),
      right(nullptr)
{
}

Huffman::Node::Node(
    std::shared_ptr<Node> leftNode,
    std::shared_ptr<Node> rightNode
)
    : character(0),
      frequency(
          leftNode->frequency +
          rightNode->frequency
      ),
      left(leftNode),
      right(rightNode)
{
}

bool Huffman::Node::isLeaf() const
{
    return !left && !right;
}

bool Huffman::Compare::operator()(
    const std::shared_ptr<Node>& a,
    const std::shared_ptr<Node>& b
) const
{
    return a->frequency > b->frequency;
}

void Huffman::buildCodes(
    const std::shared_ptr<Node>& node,
    const std::string& code,
    std::unordered_map<unsigned char, std::string>& codes
) const
{
    if (!node)
        return;

    if (node->isLeaf())
    {
        codes[node->character] =
            code.empty() ? "0" : code;

        return;
    }

    buildCodes(
        node->left,
        code + "0",
        codes
    );

    buildCodes(
        node->right,
        code + "1",
        codes
    );
}

std::string Huffman::encode(
    const std::string& input
) const
{
    if (input.empty())
        return "";

    std::unordered_map<unsigned char, long long> frequency;

    for (unsigned char ch : input)
    {
        frequency[ch]++;
    }

    std::priority_queue<
        std::shared_ptr<Node>,
        std::vector<std::shared_ptr<Node>>,
        Compare
    > pq;

    for (const auto& entry : frequency)
    {
        pq.push(
            std::make_shared<Node>(
                entry.first,
                entry.second
            )
        );
    }

    while (pq.size() > 1)
    {
        auto left = pq.top();
        pq.pop();

        auto right = pq.top();
        pq.pop();

        pq.push(
            std::make_shared<Node>(
                left,
                right
            )
        );
    }

    auto root = pq.top();

    std::unordered_map<unsigned char, std::string> codes;

    buildCodes(
        root,
        "",
        codes
    );

    std::string bits;

    for (unsigned char ch : input)
    {
        bits += codes[ch];
    }

    std::string output;

    // Store frequency table
    output += std::to_string(frequency.size());
    output += ";";

    for (const auto& entry : frequency)
    {
        output += std::to_string(
            static_cast<int>(entry.first)
        );

        output += ":";

        output += std::to_string(
            entry.second
        );

        output += ",";
    }

    output += ";";

    // Store number of valid bits
    output += std::to_string(bits.size());
    output += ";";

    // Pack bits into bytes
    unsigned char currentByte = 0;
    int bitCount = 0;

    for (char bit : bits)
    {
        currentByte <<= 1;

        if (bit == '1')
            currentByte |= 1;

        bitCount++;

        if (bitCount == 8)
        {
            output.push_back(
                static_cast<char>(currentByte)
            );

            currentByte = 0;
            bitCount = 0;
        }
    }

    if (bitCount > 0)
    {
        currentByte <<= (8 - bitCount);

        output.push_back(
            static_cast<char>(currentByte)
        );
    }

    return output;
}

std::string Huffman::decode(
    const std::string& input
) const
{
    if (input.empty())
        return "";

    std::size_t firstSeparator =
        input.find(';');

    if (firstSeparator == std::string::npos)
        return "";

    std::size_t secondSeparator =
        input.find(
            ';',
            firstSeparator + 1
        );

    if (secondSeparator == std::string::npos)
        return "";

    std::size_t thirdSeparator =
        input.find(
            ';',
            secondSeparator + 1
        );

    if (thirdSeparator == std::string::npos)
        return "";

    std::string frequencyData =
        input.substr(
            firstSeparator + 1,
            secondSeparator - firstSeparator - 1
        );

    std::size_t validBits =
        std::stoull(
            input.substr(
                secondSeparator + 1,
                thirdSeparator - secondSeparator - 1
            )
        );

    std::unordered_map<unsigned char, long long> frequency;

    std::stringstream ss(frequencyData);

    std::string entry;

    while (std::getline(ss, entry, ','))
    {
        if (entry.empty())
            continue;

        std::size_t colon =
            entry.find(':');

        if (colon == std::string::npos)
            continue;

        int character =
            std::stoi(
                entry.substr(0, colon)
            );

        long long count =
            std::stoll(
                entry.substr(colon + 1)
            );

        frequency[
            static_cast<unsigned char>(character)
        ] = count;
    }

    if (frequency.empty())
        return "";

    std::priority_queue<
        std::shared_ptr<Node>,
        std::vector<std::shared_ptr<Node>>,
        Compare
    > pq;

    for (const auto& entry : frequency)
    {
        pq.push(
            std::make_shared<Node>(
                entry.first,
                entry.second
            )
        );
    }

    while (pq.size() > 1)
    {
        auto left = pq.top();
        pq.pop();

        auto right = pq.top();
        pq.pop();

        pq.push(
            std::make_shared<Node>(
                left,
                right
            )
        );
    }

    auto root = pq.top();

    std::string result;

    // Single-character input
    if (root->isLeaf())
    {
        for (std::size_t i = 0;
             i < validBits;
             i++)
        {
            result.push_back(
                static_cast<char>(
                    root->character
                )
            );
        }

        return result;
    }

    std::shared_ptr<Node> current = root;

    const std::string encodedData =
        input.substr(
            thirdSeparator + 1
        );

    std::size_t processedBits = 0;

    for (unsigned char byte : encodedData)
    {
        for (int bit = 7;
             bit >= 0;
             bit--)
        {
            if (processedBits >= validBits)
                break;

            bool one =
                (byte >> bit) & 1;

            if (one)
                current = current->right;
            else
                current = current->left;

            if (current->isLeaf())
            {
                result.push_back(
                    static_cast<char>(
                        current->character
                    )
                );

                current = root;
            }

            processedBits++;
        }

        if (processedBits >= validBits)
            break;
    }

    return result;
}