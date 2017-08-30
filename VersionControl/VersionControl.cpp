// VersionControl.cpp : Defines the entry point for the console application.
//

#include "../PicoSHA2/picosha2.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>

// Internal codes for commands which we know how to handle, plus an error code (unknownCommand)
enum class Command : uint8_t {unknownCommand, init, add, commit};

void usage(char* invoke, Command source) {
	if (source == Command::unknownCommand) {
		std::cout << "Usage:\n";
		std::cout << invoke << " init\n";
		std::cout << invoke << " add [files]\n";
		std::cout << invoke << " commit [files] [-a]\n";
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		usage(argv[0], Command::unknownCommand);
	}
    return 0;
}

