// herorepofix.cpp : Defines the entry point for the console application.
//

#include "crossplatform.h"
#include "classes\indexmap.h"
#include <string>
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
	std::cout << "        \"0.02.2\"\n";
	std::cout << "        \"0.03.0 (current)\"\n";
	std::cout << "  --heuristic\tAttempts to guess which source-version is appropriate\n";
	std::cout << "    Any known conditions where this is guaranteed to fail shall be listed here.\n";
	std::cout << "    This is never, however, guaranteed to succeed. If you know a past version where\n";
	std::cout << "      `hero` can parse the repository, you should pass in the lowest version listed\n";
	std::cout << "      above which is greater than that version.\n";
	exit(1);
}

void upgradeFrom(const std::string& source) {
	if (source == "0.03.0") {
		// Upgrade this version to itself. Do nothing
		exit(0);
	}
	else if (source == "0.02.2") {
		// There isn't a different in the commit format or directory structures.
		// We should, however, make sure the index stays sane
		// And 0.03.0 entirely remade the index structure.
		// We need to take out the index, and remake it ourselves.
		chdir(".hero/index/"); // So that the new index is functionally equivalent to the old one
		Indexmap imap;
		std::vector<std::string> files;
		if (filesInDirectory(".", files)) {
			std::cerr << "Could not upgrade repository past 0.02.2.\n";
			exit(1);
		}

		// Make an imap and rename files as we go
		for (const auto& file : files) {
			imap.add(file);
			rename(file.c_str(), imap[file].c_str());
		}

		// Write the imap to the proper on-disk location
		std::ofstream output("map", std::ios::out | std::ios::trunc);
		output << imap;

		// Restore our cwd
		chdir("../../");
		// And upgrade from the next version
		upgradeFrom("0.03.0");
	}
	else if (source == "0.02.1") {
		// The difference between 0.02.2 and 0.02.1 is simply the name of the repository directory
		rename(".vcs", ".hero");
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
			std::vector<std::string> indexfiles;
			if (filesInDirectory(".hero/index/", indexfiles)) {
				std::cerr << "Could not perform heuristic recognition.\n";
				return 1;
			}

			// If there are some files in the index, we should try to check for an indexmap
			if (indexfiles.size() && !std::ifstream(".hero/index/map")) // If no indexmap, but index files, then we are in 0.02.2
				upgradeFrom("0.02.2");
			else {
				// If there are no index files we don't care about 0.03.0. If there are, and there is an indexmap, we are post-0.03.0
				upgradeFrom("0.03.0");
			}
		}
	}
	else {
		// Assume that the argument is the source version
		upgradeFrom(argv[1]);
	}
    return 0;
}
