#include <string>
#include <fstream>
#include <cctype>
#include <iostream>
#include <filesystem>
#include <vector>
#include "trie.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <chrono>
#include <termios.h>

void disableEcho() {
    struct termios t;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &t);

    // Disable echoing (turn off the ECHO flag)
    t.c_lflag &= ~ECHO;

    // Set the new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableEcho() {
    struct termios t;

    // Get the current terminal settings
    tcgetattr(STDIN_FILENO, &t);

    // Enable echoing (turn on the ECHO flag)
    t.c_lflag |= ECHO;

    // Set the new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

char keycodeToChar(int code) {
    switch (code) {
        // Letters
        case KEY_A: return 'a';
        case KEY_B: return 'b';
        case KEY_C: return 'c';
        case KEY_D: return 'd';
        case KEY_E: return 'e';
        case KEY_F: return 'f';
        case KEY_G: return 'g';
        case KEY_H: return 'h';
        case KEY_I: return 'i';
        case KEY_J: return 'j';
        case KEY_K: return 'k';
        case KEY_L: return 'l';
        case KEY_M: return 'm';
        case KEY_N: return 'n';
        case KEY_O: return 'o';
        case KEY_P: return 'p';
        case KEY_Q: return 'q';
        case KEY_R: return 'r';
        case KEY_S: return 's';
        case KEY_T: return 't';
        case KEY_U: return 'u';
        case KEY_V: return 'v';
        case KEY_W: return 'w';
        case KEY_X: return 'x';
        case KEY_Y: return 'y';
        case KEY_Z: return 'z';

        // Numbers
        case KEY_1: return '1';
        case KEY_2: return '2';
        case KEY_3: return '3';
        case KEY_4: return '4';
        case KEY_5: return '5';
        case KEY_6: return '6';
        case KEY_7: return '7';
        case KEY_8: return '8';
        case KEY_9: return '9';
        case KEY_0: return '0';

        // Whitespace
        case KEY_SPACE: return ' ';
        case KEY_ENTER: return '\n';
        case KEY_TAB: return '\t';

        // Punctuation
        case KEY_MINUS: return '-';
        case KEY_EQUAL: return '=';
        case KEY_LEFTBRACE: return '[';
        case KEY_RIGHTBRACE: return ']';
        case KEY_BACKSLASH: return '\\';
        case KEY_SEMICOLON: return ';';
        case KEY_APOSTROPHE: return '\'';
        case KEY_GRAVE: return '`';
        case KEY_COMMA: return ',';
        case KEY_DOT: return '.';
        case KEY_SLASH: return '/';

        default:
            return 0; // non-printable / unsupported
    }
}

std::vector<std::string> split(std::string s) {
	std::vector<std::string> splitStrings;
	std::string buffer;

	for(int i = 0; i < s.size(); i++) {
		char c = std::tolower(s[i]);
		
		// Contractions and possession are not supported.
		// Currently, they are being treated as if the apostrophe doesn't exist.
		if(c == '\'') continue;

		// Treat any non a-z character as if it was a space between words.
		if(c - 'a' < 0 || c - 'z' > 25) {
			if(buffer.size() > 0) {
				splitStrings.push_back(buffer);
				buffer.clear();
			}
		}
		else {
			buffer.push_back(c);
		}
	}

	if(buffer.size() > 0) {
		splitStrings.push_back(buffer);
	}

	return splitStrings;
}

void builder(Trie* root, std::string path) {
	std::ifstream file(path);	
	std::string line;

	if(!file.is_open() || root == nullptr) return;

	while(std::getline(file, line)) {
		for(std::string str : split(line)) {
			std::vector<char> wordVector(str.begin(), str.end());
			root->addWord(wordVector);
		}
	}
}

void buildPathsInDir(Trie* root, std::string dir) {
	std::filesystem::path path(dir);

	for(auto const file : std::filesystem::directory_iterator(path)) {
		builder(root, file.path().string());
	}
}

int openKeyboardInput() {
	const char* INPUT_DEV = "/dev/input/by-path/platform-i8042-serio-0-event-kbd";
	int fd;

	fd = open(INPUT_DEV, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Cannot open %s: %s.\n", INPUT_DEV, strerror(errno));
	}

	return fd;	
}

void handleKeyboardInput(int fd) {
	std::vector<char> buffer;
	struct input_event ev;
	ssize_t n;

	auto lastTime = std::chrono::steady_clock::now();

	while (true) {
		n = read(fd, &ev, sizeof ev);
		if (n == (ssize_t)-1) {
			if (errno == EINTR)
				continue;
			else
				break;
		} else {
			if (n != sizeof ev) {
				errno = EIO;
				break;
			}
		}
	
		// Handle input - ev.value 1 == pressed down & 2 == held down
		if (ev.type == EV_KEY && ev.value == 1) {
			char input = keycodeToChar(ev.code);

			buffer.push_back(input);
			std::cout << "\r";
			for(char c : buffer) {
				std::cout << c;
			}
			std::cout << std::flush;

			if(input == '\n') buffer.clear();
		}
	}
}

int main() {
	// Initialization
	disableEcho();
	Trie* root = new Trie();
	int fd = openKeyboardInput();

	// Testing functions
	/*
	handleKeyboardInput(fd);
	buildPathsInDir(root, "/home/mallen/autocomplete-engine/dictionary");
	std::string test = "the";
	std::cout << root->getFrequency(std::vector<char>(test.begin(), test.end())) << "\n";
	*/

	// Cleanup
	enableEcho();
	delete root;
}
