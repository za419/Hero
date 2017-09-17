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
#include <iomanip>
#include <cctype>

// Internal codes for commands which we know how to handle, plus an error code (unknownCommand)
enum class Command : uint8_t { unknownCommand, init, add, commit, commitLast, commitFiles, log, checkout };

// Function declarations for running commands
void init();
void add(const std::vector<std::string>&);
void commit();
void commitLast();
void commitFiles(const std::vector<std::string>&);
void log();
void checkout(std::string);

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
	case Command::log:
		std::cout << invoke << " log\n";
		std::cout << "Outputs a version history of the repository by commits.\n";
		std::cout << "No arguments are required or allowed.\n";
		break;
	case Command::unknownCommand:
	default:
		std::cout << invoke << " init\n";
		std::cout << invoke << " add [files]\n";
		std::cout << invoke << " commit [files] [-a]\n";
		std::cout << invoke << " log\n";
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

		if (argc > 2) {
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
	else if (!strcmp(argv[1], "log")) {
		mode = Command::log;

		if (argc > 2) {
			usage(argv[0], Command::log);
		}
	}
	else if (!strcmp(argv[1], "checkout")) {
		mode = Command::checkout;

		if (argc > 2) {
			usage(argv[0], Command::checkout);
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
		case Command::log:
		{
			log();
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
	commit << "message &This commit marks the initialization of the repository.&\n";
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
	title = escaped(title, "/", "/sl;");
	commit << "title " << escaped(title, "&", "/amp;") << "\n";

	// Do the same for the commit message
	std::cout << "Commit message (type Ctrl-X then press enter to end):\n";
	std::getline(std::cin, message, char(24));
	message = escaped(message, std::string((char)24,1), ""); // Just in case
	message = escaped(message, "/", "/sl;");
	commit << "message &" << escaped(message, "&", "/amp;") << "&\n";

	// Alert the user that we're working on the commit
	// The commit process can take some time, so we don't want the user to wonder if they need to enter ^x again
	std::cout << "Creating new commit \'" << title << "\'..." << std::endl;

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

	// Confirm to the user that we succeeded
	std::cout << "Done.\n";
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

// Produces a log of the commit history by the commit headers
void log() {
	std::string hash(getHeadHash());
	std::ifstream commit;

	while (hash != "0") {
		commit.open(".vcs/commits/" + hash);
		if (!commit) {
			std::cerr << "Could not access commit " << hash << "\n";
			exit(1);
		}
		std::string line;

		std::cout << "commit " << hash << "\n";

		// Discard two lines. The next line is the parent.
		std::getline(commit, line);
		std::getline(commit, line);

		// Now, get the parent and extract the hash
		std::getline(commit, line);
		hash = line.substr(7); // There are 7 characters before the hash begins: "parent "

		// The next line is the date
		std::getline(commit, line);
		std::cout << "Committed on " << line.substr(5); // 5 characters: "date "

		// And then the time
		std::getline(commit, line);
		std::cout << " at " << line.substr(5) << "\n"; // Again, 5 characters: "time "

		// The title
		std::getline(commit, line);
		line = escaped(line.substr(6), "/amp;", "&");
		line = escaped(line, "/sl;", "/");
		std::cout << "\t" << line << "\n\n"; // 6 characters: "title "

		// And finally the message
		std::getline(commit, line, '&'); // Discard the beginning
		std::getline(commit, line, '&'); // And fetch the entire message
		line = escaped(line, "/amp;", "&");
		line = escaped(line, "/sl;", "/");
		line = escaped(line, "\n", "\n\t"); // Indent every line of the commit message
		std::cout << "\t" << line << "\n\n";

		commit.close();
	}
}

// Given a commit (reference), copies files out to the working directory from the commit.
// Reference can be one of:
//  - A complete hash
//  - HEAD (which shall be resolved to the complete hash of the current head commit)
void checkout(std::string reference) {
	if (reference == "HEAD") {
		reference = getHeadHash();
	}

	std::ifstream commit(reference, std::ios::binary);
	std::string line;

	while (line != "&&&&&") { // Advance through commit header
		std::getline(commit, line);
	}

	// Now, we can iterate over files
	size_t numFiles(0);
	size_t totalSize(0);
	while (true) {
		std::string filename;
		std::getline(commit, filename);
		if (filename == "COMMIT FOOTER") { // The footer is not a file, so we're done.
			break;
		}
		else {
			++numFiles;
		}
		std::cout << "Unpacking file " << filename << "\n";

		// Get the stored file checksum
		std::string hash;
		std::getline(commit, hash);
		hash = hash.substr(std::string("checksum ").size());

		// Test the file in the working directory to see if it matches our checksum
		std::ifstream file(filename, std::ios::binary);
		bool skip(false);
		if (file) {
			std::string test(picosha2::hash256_hex_string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
			file.close();
			if (hash == test) { // Confirm with the user that they're okay with us skipping checkout based on hash
				char result = '\0';
				while (result != 'y' && result != 'n' && result != '\n') {
					std::cout << "File on disk has same SHA256 as file in commit. Checkout anyway? (y/N) ";
					char result = tolower(std::cin.get());
					std::cout << std::endl;
				}
				skip = (result != 'n');
			}
		}

		// Unless skip is set, read the file in the commit out to disk
		if (!skip) {
			std::fstream file(filename, std::ios::binary | std::ios::trunc);

			std::getline(commit, line, ' '); // Line now holds the size field's identifier
			size_t filesize;
			commit >> filesize; // And now we let istream perform the input conversion
			totalSize += filesize; // Add the size to the totalSize counter
			
			// Now, get rid of the extraneous characters
			std::getline(commit, line, '&');
			std::getline(commit, line);

			// The get pointer is now at the beginning of the file
			// Allocate a buffer and read the prescribed size out of the file
			char* contents(new char[filesize]);
			commit.read(contents, filesize);

			// And write the buffer back onto the disk
			file.write(contents, filesize);

			// Now, we do the safety comparison of the hashes
			file.seekg(0, std::ios::beg);
			std::string test(picosha2::hash256_hex_string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
			file.close();
			if (test == hash) { // We're pretty sure checkout succeeded.
				std::cout << "File checked out successfully.\n\n";
			}
			else { // We have a mismatch
				std::cerr << "WARNING: Hash mismatch on checking out " << filename << ".\n";
				std::cerr << "commit stored hash \"" << hash << "\"\n";
				std::cerr << "File written to disk has hash \"" << test << "\"\n\n";
				std::cerr << "This means that either the commit was written improperly,\n";
				std::cerr << "    the commit was modified after being written,\n";
				std::cerr << "    or the file was not checked out correctly.\n\n";
				std::cerr << "While not necessarily indicative of a problem, you might want to check the file.\n";
			}

			// Read out the rest of the line to pass the file footer
			std::getline(commit, line);
		}
		else { // Forward until we pass the file footer
			std::getline(commit, line, ' '); // Line now holds the size field's identifier
			size_t filesize;
			commit >> filesize; // And now we let istream perform the input conversion
			std::getline(commit, line, '&');
			std::getline(commit, line);
			commit.seekg((size_t)commit.tellg() + filesize + 6); // 6 characters are past the EOF: five ampersands and a newline
		}
	}

	// At this point, we've arrived at and read the line "COMMIT FOOTER"
	std::cout << "Done reading files.\n";

	std::getline(commit, line); // Discard the three ampersands that delineate the commit footer's header
	std::getline(commit, line, ' '); // Discard up until the counter
	size_t files;
	commit >> files;
	std::cout << "commit said we were supposed to read " << files << " files.\n";
	std::cout << "We actually read " << numFiles << " files.\n";
	
	std::getline(commit, line, ' '); // Discard up until the next counter
	size_t size;
	commit >> size;
	std::cout << "commit said we were supposed to read " << size << " bytes from files.\n";
	std::cout << "We actually read " << totalSize << " bytes from files.\n";
}
