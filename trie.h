#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <queue>
#include <map>

class Trie {
public:
    // Constructors and Destructor
    Trie();
    ~Trie();

    // Public Methods
    int getFrequency(std::vector<char> word);
    char getCurr();
    bool checkWord(std::vector<char> word);
    void addWord(std::vector<char> word);
    std::vector<std::vector<char>> findWithPrefix(std::vector<char> prefix, int matches);

private:
    // Private Member Variables
    Trie* children;
    int frequency;
    char curr;
    bool isWord;

    // Private Methods
    Trie& addChar(Trie& trie, char c, bool w);
    void initializeChildren(Trie& trie, char c, bool w);
    void initializeChar(Trie& trie, char c, bool w);
    Trie* findNode(std::vector<char> word);
};

#endif // TRIE_H
