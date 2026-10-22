#include <iostream>
#include <vector>
#include <string>
#include <cctype>

#include "../include/FileManager.h"
#include "../include/SpellCorrector.h"

using namespace std;

int main()
{
    cout << "\n========================================\n";
    cout << "       SPELL CORRECTION TEST\n";
    cout << "    SMART FILE SEARCH ENGINE\n";
    cout << "========================================\n";

    FileManager fileManager(
        "data/documents"
    );

    vector<Document> documents =
        fileManager.loadDocuments();

    if (documents.empty())
    {
        cout << "\nNo documents found.\n";
        return 1;
    }

    // ========================================
    // BUILD WORD DICTIONARY
    // ========================================

    vector<string> words;

    for (const auto& document : documents)
    {
        string word;

        for (char ch : document.content)
        {
            if (isalnum(
                static_cast<unsigned char>(ch)
            ))
            {
                word += static_cast<char>(
                    tolower(
                        static_cast<unsigned char>(ch)
                    )
                );
            }
            else
            {
                if (!word.empty())
                {
                    words.push_back(word);
                    word.clear();
                }
            }
        }

        if (!word.empty())
        {
            words.push_back(word);
        }
    }

    SpellCorrector corrector;

    corrector.build(words);

    // ========================================
    // TEST WORDS
    // ========================================

    vector<string> testWords =
    {
        "pythno",
        "databse",
        "machne",
        "algoritm",
        "compresson",
        "seach"
    };

    int passed = 0;
    int failed = 0;

    for (const string& word : testWords)
    {
        cout << "\n----------------------------------------\n";

        cout << "Input Word : "
             << word
             << "\n";

        string corrected =
            corrector.correct(word);

        cout << "Correction : "
             << corrected
             << "\n";

        vector<string> suggestions =
            corrector.suggestions(
                word,
                2
            );

        cout << "Suggestions: ";

        if (suggestions.empty())
        {
            cout << "NONE";
        }
        else
        {
            for (const string& suggestion :
                 suggestions)
            {
                cout << suggestion << " ";
            }
        }

        cout << "\n";

        if (!corrected.empty() &&
            corrected != word)
        {
            cout << "Status     : PASSED\n";
            passed++;
        }
        else
        {
            cout << "Status     : CHECK\n";
            failed++;
        }
    }

    // ========================================
    // SUMMARY
    // ========================================

    cout << "\n========================================\n";
    cout << "       SPELL CORRECTION SUMMARY\n";
    cout << "========================================\n";

    cout << "Words Tested : "
         << testWords.size()
         << "\n";

    cout << "Passed       : "
         << passed
         << "\n";

    cout << "Check Needed : "
         << failed
         << "\n";

    cout << "========================================\n";
    cout << "SPELL CORRECTION TEST COMPLETED\n";
    cout << "========================================\n";

    return 0;
}