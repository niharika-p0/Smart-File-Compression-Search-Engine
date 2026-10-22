#ifndef SUFFIXARRAY_H
#define SUFFIXARRAY_H

#include <string>
#include <vector>

class SuffixArray
{
private:

    std::string text;

    std::vector<int> suffixArray;

    int compareSuffixWithPattern(
        int suffixPosition,
        const std::string& pattern
    ) const;

public:

    SuffixArray();

    explicit SuffixArray(
        const std::string& input
    );

    void build(
        const std::string& input
    );

    std::vector<int> search(
        const std::string& pattern
    ) const;

    const std::string& getText() const;

    const std::vector<int>&
    getSuffixArray() const;

    void display() const;
};

#endif