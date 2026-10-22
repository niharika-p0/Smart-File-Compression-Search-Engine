#ifndef SEARCHENGINECLI_H
#define SEARCHENGINECLI_H

#include "ProjectManager.h"

class SearchEngineCLI
{
private:

    ProjectManager& project;

    void showMenu() const;

    void handleSearch();

    void handleAutocomplete();

    void handleCompression();

    void handleStatistics();

    void handleIndexes();

public:

    explicit SearchEngineCLI(
        ProjectManager& projectManager
    );

    void run();
};

#endif