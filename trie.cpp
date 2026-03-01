#include "trie.h"
#include <iostream>

/*
int main() {
	Trie root;

	root.addWord(std::vector<char>{'h','e','l','l','o'});
	root.addWord(std::vector<char>{'h','e','l','p'});
	root.addWord(std::vector<char>{'h','e','l','p'});

	std::cout << root.checkWord(std::vector<char>{'h','e','l','l','o'}) << "\n";
	std::cout << root.checkWord(std::vector<char>{'m','e','l','l','o'}) << "\n";

	std::vector<std::vector<char>> matches = root.findWithPrefix(std::vector<char>{'h','e','l'}, 3);
	for(std::vector<char> v : matches) {
		for(char c : v) {
			std::cout << c;
		}
		std::cout << "\n";
	}

	return 0;
}
*/

Trie::Trie() :
	frequency(0),
	curr('g'),
	isWord(false)
{	
	children = nullptr;
}

Trie::~Trie() {
	delete[] children;
}

int Trie::getFrequency(std::vector<char> word) {
	Trie* trie = findNode(word);	
	return trie == nullptr ? 0 : trie->frequency;
}

char Trie::getCurr() {
	return this->curr;
}

bool Trie::checkWord(std::vector<char> word) {
	Trie* trie = findNode(word);	
	return trie == nullptr ? false : trie->isWord;
}

void Trie::addWord(std::vector<char> word) {
	Trie* trie = this;
	for(char c : word) {
		trie = &trie->addChar(*trie, c, false);
	}
	trie->isWord = true;
}

std::vector<std::vector<char>> Trie::findWithPrefix(std::vector<char> prefix, int matches) {
	std::vector<std::vector<char>> suggestions;

	// Starting point
	Trie* trie = findNode(prefix);
	if(trie == nullptr) return suggestions;

	// BFS for words
	std::queue<std::pair<std::vector<char>, Trie*>> queue;
	queue.push({prefix, trie});

	while(!queue.empty()) {
		// Loop through all items from current level
		int size = queue.size();
		std::map<int, std::vector<std::vector<char>>> map;
		for(int i = 0; i < size; i++) {
			// Pop queue
			auto pair = queue.front();
			queue.pop();
			std::vector<char> path = pair.first;
			Trie* node = pair.second;

			// Add current character to map based on frequency
			if(node->isWord) {
				if(map.count(node->frequency) == 1) map[node->frequency].push_back(path);
				else map[node->frequency] = std::vector<std::vector<char>>{path};
			}

			// Add children of current node to queue
			for(int j = 0; j < 26; j++) {
				if(node->children == nullptr) continue;
				Trie* child = &(node->children[j]);

				if(child->frequency != 0) {
					std::vector<char> newPath(path);
					newPath.push_back(child->curr);
					queue.push({newPath, child});
				}
			}
		}

		// Add matches and return if the number of requested suggestions has been reached
		for(auto it = map.rbegin(); it != map.rend(); it++){ 
			std::vector<std::vector<char>> paths = it->second;
			suggestions.insert(suggestions.end(), paths.begin(), paths.end());
			if(suggestions.size() >= matches) return suggestions;
		}
	}

	return suggestions;
}

Trie& Trie::addChar(Trie& trie, char c, bool w) {
	if(trie.children == nullptr) initializeChildren(trie, c, w);
	Trie& child = trie.children[c - 'a'];
	if(child.frequency == 0) {
		initializeChar(child, c, w);
		return child;
	}

	child.frequency++;
	child.isWord = child.isWord ? child.isWord : w;

	return child;
}

void Trie::initializeChildren(Trie& trie, char c, bool w) {
	trie.children = new Trie[26];
}

void Trie::initializeChar(Trie& trie, char c, bool w) {
	trie.frequency = 1;
	trie.curr = c;
	trie.isWord = w;
}

Trie* Trie::findNode(std::vector<char> word) {
	if(word.size() == 0) return nullptr;

	Trie* trie = this;
	for(char c : word) {
		if(trie->children == nullptr) return nullptr;
		trie = &(trie->children[c - 'a']);
	}
	return trie;
}
