// VersionControl.cpp : Defines the entry point for the console application.
//

#include "../PicoSHA2/picosha2.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>

// Internal codes for commands which we know how to handle, plus an error code (unknownCommand)
enum class Command : uint8_t {unknownCommand, init, add, commit};

void usage(char* invoke, Command source) {
	std::cout << "Usage:\n";

	switch (source) {
	case Command::init:
		std::cout << invoke << " init\n";
		std::cout << "Initializes a new repository in the current folder.\n";
		std::cout << "No arguments are required or allowed.\n";
		break;
	case Command::unknownCommand:
	default:
		std::cout << invoke << " init\n";
		std::cout << invoke << " add [files]\n";
		std::cout << invoke << " commit [files] [-a]\n";
		break;
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		usage(argv[0], Command::unknownCommand);
	}
    return 0;
}

