// VersionControl.cpp : Defines the entry point for the console application.
//

#include "crossplatform.h"
#include "../PicoSHA2/picosha2.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <fstream>

// Internal codes for commands which we know how to handle, plus an error code (unknownCommand)
enum class Command : uint8_t { unknownCommand, init, add, commit };

void usage(char* invoke, Command source) {
	std::cout << "Usage:\n";

	switch (source) {
	case Command::init:
		std::cout << invoke << " init\n";
		std::cout << "Initializes a new repository in the current folder.\n";
		std::cout << "No arguments are required or allowed.\n";
		break;
	case Command::add:
		std::cout << invoke << " add [files]\n";
		std::cout << "Adds a file or several files to the index, using their state on disk at the time of invocation.\n";
		std::cout << "Accepts an arbitrary number of arguments, all of which must be files on disk to add (excepting \"-h\" to produce this output).\n";
		std::cout << "Note that if added files are changed while this command is running, the index may be left in an inconsistent state.\n";
		break;
	case Command::commit:
		std::cout << invoke << " commit [files] [-a]\n";
		std::cout << "Creates a new commit with the files in the index at the time of invocation.\n";
		std::cout << "If \'-a\' is present, adds all files which were committed in the most recent commit first.\n";
		std::cout << "If other arguments are present, they must be files on disk.\n";
		std::cout << "These files will be treated as those which shall be the ONLY ones committed.\n";
		std::cout << "Note that if the index is altered while this command is running, the commit may be produced in an inconsistent state.\n";
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
	Command mode=Command::unknownCommand;

	// First, argument handling.
	if (argc < 2) {
		usage(argv[0], Command::unknownCommand);
	}
	else if (!strcmp(argv[1], "add")) {
		mode = Command::add;

		if (argc == 2) {
			std::cout << "No files to add.\n";
			return 0;
		}

		if (!strcmp(argv[2], "-h")) {
			usage(argv[0], Command::add);
		}
	}
	else if (!strcmp(argv[1], "commit")) {
		mode = Command::commit;

		if (argc == 2) {
			std::cout << "No files to add.\n";
			return 0;
		}

		if (!strcmp(argv[2], "-h")) {
			usage(argv[0], Command::commit);
		}
	}
	else if (!strcmp(argv[1], "init")) {
		mode = Command::init;

		if (argc > 2) {
			usage(argv[0], Command::init);
		}
	}
	else {
		usage(argv[0], Command::unknownCommand);
	}
	
	// First, the easiest and always-run command: init.
	// Initialize all the files the other commands assume to exist.
	if (mode == Command::init) {
		mkdir(".vcs");
	}

	return 0;
}

