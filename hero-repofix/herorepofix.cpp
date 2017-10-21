// herorepofix.cpp : Defines the entry point for the console application.
//

#include "../VersionControl/crossplatform.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

void usage(const char* invoke) {
	std::cout << "Usage: " << invoke << " source-version|-h|--heuristic\n";
	std::cout << "Attempts to upgrade the repository whose root is in the working directory to\n";
	std::cout << "  be compatible with the latest version of `hero`.\n";
	std::cout << "Accepts exactly one argument, which must be one of:\n";
	std::cout << "  -h\tGenerates this help message\n";
	std::cout << "  source-version\tThe latest version the repository is now compatible with\n";
	std::cout << "    Thanks to the repository format only having breaking changes fairly rarely,\n";
	std::cout << "      only a few versions are valid here. These are:\n";
	std::cout << "        \"0.02.1\"\n";
	std::cout << "        \"0.02.2 (current)\"\n";
	std::cout << "  --heuristic\tAttempts to guess which source-version is appropriate\n";
	std::cout << "    Any known conditions where this is guaranteed to fail shall be listed here.\n";
	std::cout << "    This is never, however, guaranteed to succeed. If you know a past version where\n";
	std::cout << "      `hero` can parse the repository, you should pass in the lowest version listed\n";
	std::cout << "      above which is greater than that version.\n";
	exit(1);
}

void upgradeFrom(const std::string& source) {
	if (source == "0.02.2") {
		// Upgrade this version to itself. Do nothing
		exit(0);
	}
	else if (source == "0.02.1") {
		// The difference between 0.02.2 and 0.02.1 is simply the name of the repository directory
		copyDirectory(".vcs", ".hero");
		removeDirectory(".vcs");
		// Now upgrade from the next version
		upgradeFrom("0.02.2");
	}
	else {
		// Unknown version
		std::cout << "Unrecognized version " << source << "\n";
		exit(127);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		usage(argv[0]);
	}
	else if (!strcmp(argv[1], "-h")) {
		usage(argv[0]);
	}
	else if (!strcmp(argv[1], "--heuristic")) {
		std::string version;
		if (std::ifstream(".vcs/HEAD")) {
			// The repository data is in .vcs, so we're pre-0.02.2
			upgradeFrom("0.02.1");
		}
		else {
			// Post 0.02.2
			upgradeFrom("0.02.2");
		}
	}
	else {
		// Assume that the argument is the source version
		upgradeFrom(argv[1]);
	}
    return 0;
}
