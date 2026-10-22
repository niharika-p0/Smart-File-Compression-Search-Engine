#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>

struct Document
{
    int id;

    std::string fileName;
    std::string filePath;

    std::string content;

    long long fileSize;
    long long wordCount;
};

#endif