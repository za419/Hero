// VersionControl.cpp : Defines the entry point for the console application.
//

#include "../date/date.h"
#include "../PicoSHA2/picosha2.h"
#include "crossplatform.h"
#include "Utils.h"
#include "VersionControl.h"

#include <iostream>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>

// Internal codes for commands which we know how to handle, plus an error code (unknownCommand)
enum class Command : uint8_t { unknownCommand, init, add, commit, commitLast, commitFiles };

// Function declarations for running commands
void init();
void add(const std::vector<std::string>&);
void commit();
void commitLast();
void commitFiles(const std::vector<std::string>&);

// Issue the usage message appropriate to the command being run, with the command we were invoked with
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

		if (argc>2) {
			if (!strcmp(argv[2], "-h")) {
				usage(argv[0], Command::commit);
			}
			else if (!strcmp(argv[2], "-a")) {
				mode = Command::commitLast;
			}
			else {
				mode = Command::commitFiles;
			}
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

	// Dispatch command execution to the appropriate function
	switch (mode) {
		case Command::init:
		{
			init();
			break;
		}
		case Command::add:
		{
			std::vector<std::string> addFiles;
			addFiles.reserve(argc - 2); // Reserve enough space for all files
			for (int i = 2; i < argc; ++i) {
				addFiles.emplace_back(argv[i]);
			}
			add(addFiles);
			break;
		}
		case Command::commit:
		{
			commit();
			break;
		}
		case Command::commitLast:
		{
			commitLast();
			break;
		}
		case Command::commitFiles:
		{
			std::vector<std::string> addFiles;
			addFiles.reserve(argc - 2); // Reserve enough space for all files
			for (int i = 2; i < argc; ++i) {
				addFiles.emplace_back(argv[i]);
			}
			commitFiles(addFiles);
			break;
		}
		default:
		{
			std::cerr << "Unrecognized commandline:";
			for (int i = 0; i < argc; ++i) {
				std::cerr << " " << argv[i];
			}
			std::cerr << "\n";
			exit(127);
		}
	}

	return 0;
}

// Definitions for command funcctions

// First, the easiest and always-run command: init.
// Initialize all the files the other commands assume to exist.
void init() {
	mkdir(".vcs");
	mkdir(".vcs/index");
	mkdir(".vcs/commits");

	// Make a plain initial commit marking repository creation
	// First, the easy part.
	std::stringstream commit; // Stores the growing commit in memory. Technically, we shouldn't do this, but... you know.
	commit << "COMMIT HEADER\n";
	commit << "&&&\n";
	commit << "parent 0\n";

	// Write datetime using date
	// Examples taken from the date documentation: https://howardhinnant.github.io/date/date.html
	auto today = date::year_month_day(date::floor<date::days>(std::chrono::system_clock::now())); // Get current date
	commit << "date " << today << "\n";
	auto clock = std::chrono::system_clock::now();
	auto now = clock - date::floor<date::days>(clock);
	auto time = date::make_time(date::floor<std::chrono::seconds>(now));
	commit << "time " << time << " UTC\n";

	// Finish off the header
	commit << "title Initial Commit\n";
	commit << "message This commit marks the initialization of the repository.\n";
	commit << "files []\n";
	commit << "&&&&&\n";

	// Now, write the commit footer
	commit << "COMMIT FOOTER\n";
	commit << "&&&\n";
	commit << "count 0\n";
	commit << "size 0\n";
	commit << "&&&&&\n";

	// Generate the hash of the commit
	std::string contents = commit.str();
	std::string hash = picosha2::hash256_hex_string(contents);

	// And write to the commit file
	std::ofstream file(".vcs/commits/" + hash);
	if (!file) {
		removeDirectory(".vcs");
		std::cerr << "Could not initialize repository.\n";
		exit(1);
	}
	file.write(contents.c_str(), contents.size());
	file.close();

	// Write the HEAD marker
	std::ofstream head(".vcs/HEAD");
	if (!head) {
		removeDirectory(".vcs");
		std::cerr << "Could not initialize repository.\n";
		exit(1);
	}
	head << hash << "\n";
	head.close();

	std::cout << "Initialized repository.\n";
}

// Next up, add.
// Take the files in the provided vector, and copy them to the index
void add(const std::vector<std::string>& files) {
	std::string tmp;
	for (auto file : files) {
		tmp = ".vcs/index/" + file;
		if (!copyfile(file.c_str(), tmp.c_str())) {
			std::cout << "Error: Could not copy file " << file << ".\n";

			emptyDirectory(".vcs/index");
			std::cout << "Index emptied.\n";
			std::cout << "Please re-add the appropriate files to the index.\n";

			exit(1);
		}
	}

	std::cout << "All files added to index.\n";
}

// Copy the files in the index into a commit file in the commits folder
// This file will have its SHA256 as its filename, and will have formatting compatible with the format specified in commit-blob.txt
void commit() {
	std::string title; // Commit title
	std::string message; // Commit message
	std::stringstream commit; // Stores the growing commit in memory. Technically, we shouldn't do this, but... you know.
	commit << "COMMIT HEADER\n";
	commit << "&&&\n";

	// Write parent hash
	std::string parent(getHeadHash());
	if (parent=="") {
		std::cerr << "Could not find repository head - have you run init?\n";
		exit(1);
	}
	commit << "parent " << parent << "\n";

	// Write datetime using date
	// Examples taken from the date documentation: https://howardhinnant.github.io/date/date.html
	auto today = date::year_month_day(date::floor<date::days>(std::chrono::system_clock::now())); // Get current date
	commit << "date " << today << "\n";
	auto clock = std::chrono::system_clock::now();
	auto now = clock - date::floor<date::days>(clock);
	auto time = date::make_time(date::floor<std::chrono::seconds>(now));
	commit << "time " << time << " UTC\n";

	// Get commit title from the user and write, escaped, to commit
	std::cout << "Commit title: ";
	std::getline(std::cin, title);
	commit << "title " << escaped(title, "&", "&amp;") << "\n";

	// Do the same for the commit message
	std::cout << "Commit message (type Ctrl-X then press enter to end):\n";
	std::getline(std::cin, title, char(24));
	commit << "message &" << escaped(title, "&", "&amp;") << "&\n";

	// Now, get the list of files in the index, and add their names to the commit header.
	std::vector<std::string> files;
	if (int err = filesInDirectory(".vcs/index", files)) {
		std::cerr << "Could not list index.\n";
		exit(err);
	}
	commit << "files [";
	for (const auto& file : files) {
		commit << file << ",";
	}
	commit << "]\n";

	// The commit header is now complete.
	commit << "&&&&&\n";

	// Now, loop over the files
	size_t totalSize(0); // Tracks the size of all files, for the footer.
	for (const auto& file : files) {
		// First, write the file path. For now, we don't properly handle subdirectories, so that's just a filename.
		commit << file << "\n";

		// Now, open the file
		std::ifstream ifs(".vcs/index/" + file, std::ios::binary);

		// Now, calculate and write the SHA256 of that file
		commit << "checksum " << picosha2::hash256_hex_string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()) << "\n";

		// Now, get and write the size of the file
		ifs.seekg(0, ifs.end);
		size_t size(ifs.tellg());
		ifs.seekg(0, ifs.beg); // Restore read pointer to the beginning
		totalSize += size;
		commit << "size " << size << "\n";

		// End file header
		commit << "&&&\n";

		// Now, the file contents
		char* buf(new char[size]); // Buffer for reading
		ifs.read(buf, size); // Read into the buffer
		commit.write(buf, size); // And write into the commit
		delete[] buf;

		// Mark the file as ended
		commit << "&&&&&\n";

		// Remove the file from the index
		remove((".vcs/index/" + file).c_str());
	}

	// Finally, the commit footer
	commit << "COMMIT FOOTER\n";
	commit << "&&&\n";
	commit << "count " << files.size() << "\n";
	commit << "size " << totalSize << "\n";
	commit << "&&&&&\n";

	// Finally, write the commit to disk and empty the index
	// First, generate the hash of the commit
	std::string contents = commit.str();
	std::string hash = picosha2::hash256_hex_string(contents);

	// And write to the commit file
	std::ofstream file(".vcs/commits/" + hash);
	if (!file) {
		std::cerr << "Could not create commit.\n";
		exit(1);
	}
	file.write(contents.c_str(), contents.size());
	file.close();

	// And update the HEAD marker to match this commit
	std::ofstream head(".vcs/HEAD", std::ios::trunc);
	if (!head) {
		remove((".vcs/commits/" + hash).c_str());
		std::cerr << "Could not create commit.\n";
		exit(2);
	}
	head << hash << "\n";
	head.close();

	emptyDirectory(".vcs/index");
}

// Handles 'commit -a'
// That is, reads the HEAD commit, lists the files named there into a vector...
// Passes that vector into add, and then calls commit
void commitLast() {
	std::string line(getHeadHash());
	std::ifstream last(".vcs/commits/"+line);
	if (!last) {
		std::cerr << "Could not access last commit.\n";
		exit(1);
	}
	std::stringstream filestream;
	std::vector<std::string> files;

	do {
		std::getline(last, line);
	} while (line.find("files [")!=0);
	last.close();

	// 7 is the first character after "files [", plus one for the ending ']'
	filestream<<line.substr(7, line.size() - 8);

	// Read all entries, using the stream as parser, into the vector
	while (true) {
		std::getline(filestream, line);
		if (filestream.eof())
			break;
		files.emplace_back(line);
	}

	// Finally, we can add these files to the index.
	add(files);
	// And then commit.
	commit();
}

// Handles commit with a list of files
// Has the semantics that all files listed are committed, and no other
// The index is preserved as it was, except that files present in the index and command line are removed from the index
void commitFiles(const std::vector<std::string>& files) {
	removeDirectory(".vcs/indexCopy"); // Just in case

	// Back up the index
	if (!copyDirectory(".vcs/index", ".vcs/indexCopy")) { 
		std::cerr << "Could not back up repository index.\n";
		exit(1);
	}

	// Empty the index entirely
	emptyDirectory(".vcs/index");

	for (const auto& file : files) {
		remove((".vcs/indexCopy/"+file).c_str()); // Make sure the file to be committed is removed from the copied index
	}

	// Add all commandline files
	add(files);
	// And then commit.
	commit();

	removeDirectory(".vcs/index");
	// Restore the index, minus duplicated files
	if (!copyDirectory(".vcs/indexCopy", ".vcs/index")) {
		std::cerr << "Could not restore index.\n";
		removeDirectory(".vcs/indexCopy");
		if (mkdir(".vcs/index") && errno != EEXIST) {
			std::cerr << "Also could not create index directory.\n";
			std::cerr << "Please ensure that the folder \".vcs\" directory is writable...\n";
			std::cerr << "Then create a folder named \"index\" inside it before running \"add\" or \"commit\".\n";
			exit(errno);
		}
		else {
			std::cerr << "The index has been emptied.\n";
			exit(2);
		}
	}
	removeDirectory(".vcs/indexCopy");
}
