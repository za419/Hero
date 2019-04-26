// hero.cpp : Defines the entry point for the console application.
//

#include "../date/include/date/date.h"
#include "../PicoSHA2/picosha2.h"
#include "crossplatform.h"
#include "Utils.h"
#include "hero.h"
#include "classes/indexmap.h"

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
enum class Command : uint8_t { unknownCommand, init, add, commit, commitLast, commitFiles, log, checkout, branch, branchCheckout, branchReference, branchCheckoutReference, branchList, merge };

// Function declarations for running commands
void init(const std::string&);
void add(const std::vector<std::string>&);
void commit();
void commitLast();
void commitFiles(const std::vector<std::string>&);
void log();
void checkout(std::string);
void branchReference(const std::string&, const std::string&);
void branchList();

// Issue the usage message appropriate to the command being run, with the command we were invoked with
void usage(char* invoke, Command source) {
	std::cout << "Usage:\n";

	switch (source) {
	case Command::init:
		std::cout << invoke << " init [branchname]\n";
		std::cout << "Initializes a new repository in the current folder.\n";
		std::cout << "Creates the single default branch as branchname if passed, or master.\n";
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
	case Command::checkout:
		std::cout << invoke << " checkout <reference>\n";
		std::cout << "Checks out the files committed in the referenced commit.\n";
		std::cout << "<reference> can be any of:\n";
		std::cout << "  1. The hash of the commit to check out\n";
		std::cout << "  2. The name of a valid branch.\n";
		std::cout << "  3. HEAD (for the most recently checked out branch)\n";
		std::cout << "Any other input is considered an error.\n";
		break;
	case Command::branch:
		std::cout << invoke << " branch [name] [reference] [-c]\n";
		std::cout << "If no arguments are given, prints a list of branchnames, marking the current one with an asterisk (*).\n";
		std::cout << "If arguments are given, creates a new branch with the given name, with the current head commit as branch tip.\n";
		std::cout << "If the branch already exists, will ask for confirmation as to whether the branch tip should be overwritten.\n";
		std::cout << "If \'-c\' is present, the newly-created branch will be checked out immediately.\n";
		std::cout << "If \"reference\" is given, it must follow the branch name.\n";
		std::cout << "reference can be anything which may be passed to \"checkout\" (see " << invoke << " checkout -h).\n";
		std::cout << "reference shall then be the branch tip of the newly created branch.\n";
		std::cout << "If \'-c\' is not given, then the HEAD at the time of invocation shall be the HEAD after invocation.\n";
		break;
	case Command::merge:
		std::cout << invoke << " merge ref1 ref2\n";
		std::cout << "Attempts to merge ref1 and ref2 into a single commit with two parents.\n";
		std::cout << "Currently supports only one merge algorithm, which proceeds to merge each file\n";
		std::cout << "  individually, under the following rules:\n";
		std::cout << "    1. If a file has the same hash in both commits, it is added to the new commit.\n";
		std::cout << "    2. If one commit has an ancestor where the file is present with the same\n";
		std::cout << "       hash as in the other commit, the version from the first is kept.\n";
		std::cout << "    3. If a file in one commit is empty, and nonempty in the other,\n";
		std::cout << "       the nonempty version is kept.\n";
		std::cout << "    4. If no above rule resolves a file, the user will be asked to choose.\n";
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

		if (argc!=3 || !strcmp(argv[2], "-h")) {
			usage(argv[0], Command::checkout);
		}
	}
	else if (!strcmp(argv[1], "branch")) {
		mode = Command::branch;

		if ((argc>2 && !strcmp(argv[2], "-h")) || argc>5) {
			usage(argv[0], Command::branch);
		}
		else if (argc == 2) {
			mode = Command::branchList;
		}
		else if (argc == 4) {
			if (!strcmp(argv[2], "-c") || !strcmp(argv[3], "-c")) {
				// Branch and checkout
				mode = Command::branchCheckout;
			}
			else {
				// Branch on reference
				mode = Command::branchReference;
			}
		}
		else if (argc == 5) {
			// Branch on reference and checkout
			mode == Command::branchCheckoutReference;
		}
	}
	else if (!strcmp(argv[1], "init")) {
		mode = Command::init;

		if (argc > 3) {
			usage(argv[0], Command::init);
		}
	}
	else if (!strcmp(argv[1], "repofix")) {
		// Skip all the mode stuff, just run hero-repofix
		// I'm making this available, but at least for now not publicly documenting it
		std::string arguments("hero-repofix");
		// Go through the arguments we got after repofix, adding them to the string, so we can call repofix correctly
		// At least for now, this is pretty much useless, since repofix can only be called with exactly one argument
		// But I don't think I'll be touching this again, so...
		// I intend to write this so that I'll never need to.
		for (int i = 2; i < argc; ++i) {
			arguments += " ";
			arguments += argv[i];
		}
		// You'd think that arguments would now have our commandline.
		// You'd be right, but only if hero-repofix is in the PATH.
		// In order to handle that, we're going to assume that hero and hero-repofix are in the same directory
		// So we can take the path given in argv[0] and prepend it to the produced commandline to make sure we're calling repofix.
		std::string invocation(argv[0]);
		size_t index(invocation.find_last_of('/'));
		if (index == std::string::npos) {
			index = invocation.find_last_of('\\');
			if (index == std::string::npos) {
				std::cerr << "Could not invoke hero-repofix.\n";
				exit(127);
			}
		}
		invocation = invocation.substr(0, index+1);
		if (system(appended(invocation, arguments))) {
			std::cerr << "Could not invoke hero-repofix.\n";
			exit(128);
		}
		else {
			exit(0);
		}
	}
	else {
		usage(argv[0], Command::unknownCommand);
	}

	// Dispatch command execution to the appropriate function
	switch (mode) {
		case Command::init:
		{
			// Initialize with either a given default branchname, or "master"
			if (argc == 3)
				init(argv[2]);
			else
				init("master");
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
		case Command::checkout:
		{
			checkout(argv[2]);
			break;
		}
		case Command::branch:
		{
			std::string current(getHeadHash());
			if (std::ifstream lock = std::ifstream(repositoryPath("COMMIT_LOCK"), std::ios::binary)) { // Account for detached status
				std::getline(lock, current);
			}
			branchReference(argv[2], current);
			break;
		}
		case Command::branchReference:
		{
			branchReference(argv[2], argv[3]);
			break;
		}
		case Command::branchCheckout:
		{
			size_t index(strcmp(argv[2], "-c") ? 2 : 3); // Index of branch name
			std::string current(getHeadHash());
			if (std::ifstream lock = std::ifstream(repositoryPath("COMMIT_LOCK"), std::ios::binary)) { // Account for detached status
				std::getline(lock, current);
			}
			branchReference(argv[index], current);
			checkout(argv[index]); // Use standard checkout mechanism to get to our new branch
			break;
		}
		case Command::branchCheckoutReference:
		{
			size_t index(strcmp(argv[2], "-c") ? 2 : 3); // Index of branch name (note that -c can also be the last argument, but the branchname must be one of the first two)
			size_t refindex(strcmp(argv[index + 1], "-c") ? index + 1 : index + 2); // Index of reference (can either be right after or two after branch name)
			branchReference(argv[index], argv[refindex]);
			checkout(argv[index]); // Use standard checkout mechanism to get to our new branch
			break;
		}
		case Command::branchList:
		{
			branchList();
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
void init(const std::string& defaultBranch) {
	mkdir(REPOSITORY_PATH.c_str());
	mkdir(repositoryPath("index"));
	mkdir(repositoryPath("commits"));
	mkdir(repositoryPath("branches"));

	std::ofstream indexmap(INDEXMAP_PATH);

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
	std::ofstream file(repositoryPath("commits/" + hash));
	if (!file) {
		removeDirectory(REPOSITORY_PATH);
		std::cerr << "Could not initialize repository.\n";
		exit(1);
	}
	file.write(contents.c_str(), contents.size());
	file.close();

	// Write the HEAD marker
	std::ofstream head(repositoryPath("HEAD"));
	if (!head) {
		removeDirectory(REPOSITORY_PATH);
		std::cerr << "Could not initialize repository.\n";
		exit(1);
	}
	head << defaultBranch << "\n";
	head.close();

	// And now write the master branch head marker
	head.open(branchHead());
	if (!head) {
		removeDirectory(REPOSITORY_PATH);
		std::cerr << "Could not initialize repository.\n";
		exit(1);
	}
	head << hash << "\n";
	head.close();

	std::cout << "Initialized repository.\n";
}

// Next up, add.
// Take the files in the provided vector, and copy them to the index
void addFiles(const std::vector<std::string>& files, Indexmap& imap) {
	std::string tmp;
	std::string hash;
	for (auto file : files) {
		std::ifstream read(file);
		if (!read) {
			// If a provided file is not a file, then try to treat it as a directory
			std::vector<std::string> f;
			if (contentsOfDirectory(file, f)) {
				std::cerr << "Error: Could not index file " << file << ".\n";

				emptyDirectory(repositoryPath("index"));
				std::cerr << "Index emptied.\n";
				std::cerr << "Please re-add the appropriate files to the index.\n";

				exit(2);
			}

			// Add all files in the directory.
			// First, make sure that the paths get prepended to the filenames
			// Then recurse with the newly filled vector.
			if (file.back() != '/' && file.back() != '\\')
				file += '/';
			for (auto& fi : f) {
				fi = file + fi;
			}
			addFiles(f, imap);
			continue;
		}

		hash = picosha2::hash256_hex_string(read);
		read.close();
		imap[file] = hash;
		tmp = repositoryPath("index/" + hash);
		if (!copyfile(file.c_str(), tmp.c_str())) {
			std::cerr << "Error: Could not copy file " << file << ".\n";

			emptyDirectory(repositoryPath("index"));
			std::cerr << "Index emptied.\n";
			std::cerr << "Please re-add the appropriate files to the index.\n";

			exit(1);
		}
	}
}

// Master command for addFiles - Set up the Indexmap, and print out a success message.
void add(const std::vector<std::string>& files) {
	IndexmapLoader imap_ldr;
	Indexmap& imap(imap_ldr.map);

	addFiles(files, imap);

	std::cout << "All files added to index.\n";
}

// Copy the files in the index into a commit file in the commits folder
// This file will have its SHA256 as its filename, and will have formatting compatible with the format specified in commit-blob.txt
void commit() {
	bool detached(false);
	std::string parent(getHeadHash());
	if (parent == "") {
		std::cerr << "Could not find repository head - have you run init?\n";
		exit(1);
	}

	// Check for the commit lock
	if (std::ifstream lock = std::ifstream(repositoryPath("COMMIT_LOCK"), std::ios::binary)) {
		// Committing is locked: We should issue a warning 
		detached = true;

		std::string hash;
		std::getline(lock, hash);

		std::cerr << "Warning: You are in detached head state.\n";
		std::cerr << "The head commit is " << normalizeReference(parent) << ".\n";
		std::cerr << "The currently checked out commit is recorded as " << normalizeReference(hash) << ".\n";
		std::cerr << "Committing in this state will not update any branch markers, and consequently\n";
		std::cerr << "  any commit you might make here won't appear in their logs and will only be\n";
		std::cerr << "  reachable if you know the commit's hash or make a new branch.\n";
		std::cerr << "To avoid this, copy your work to another location, press Ctrl-C to stop commit,\n";
		std::cerr << "  and run `checkout` with a branch name, or `branch -c` to create a new branch.\n";
		std::cerr << "  can then copy your work back and commit.\n";

		// Update parent hash to be the one we have checked out
		parent = hash;
	}

	std::string title; // Commit title
	std::string message; // Commit message
	std::stringstream commit; // Stores the growing commit in memory. Technically, we shouldn't do this, but... you know.
	commit << "COMMIT HEADER\n";
	commit << "&&&\n";

	// Write parent hash
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

	// Ken's Easter Egg
	// This conditional is dedicated to Ken Ellorando.
	if (title == "F") {
		std::cout << "Respects paid.\n";
	}

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
	CommitmapLoader cmap_ldr;
	Commitmap& cmap(cmap_ldr.map);

	commit << "files [";
	for (const auto& pair : cmap) {
		commit << pair.second << ",";
	}
	commit << "]\n";

	// The commit header is now complete.
	commit << "&&&&&\n";

	// Now, loop over the files
	size_t totalSize(0); // Tracks the size of all files, for the footer.
	for (const auto& pair : cmap) {
		const Commitmap::Hash& index(pair.first);
		const Commitmap::Filename& disk(pair.second);

		// First, write the file path, from the commitmap.
		commit << disk << "\n";

		// Now, open the file
		std::ifstream ifs(repositoryPath("index/" + index), std::ios::binary);

		// Now, write the hash of the file.
		// We distrust the indexmap, just in case it's been modified (for some reason):
		//   We want the commit's file hash to always match the hash of the data in the file.
		// It's a data integrity thing. That is, after all, the point of writing the hash.
		auto hash = picosha2::hash256_hex_string(ifs);
		if (hash != index) {
			std::cout << "Indexed file " << disk << " has a hash mismatch.\n"
				<< "  Hash at add time was: " << index << "\n"
				<< "  Hash at commit time is: " << hash << "\n"
				<< "Some data may have been corrupted.\n\n";
		}
		commit << "checksum " << hash << "\n";

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
		remove(repositoryPath("index/" + index));
	}

	// Finally, the commit footer
	commit << "COMMIT FOOTER\n";
	commit << "&&&\n";
	commit << "count " << cmap.size() << "\n";
	commit << "size " << totalSize << "\n";
	commit << "&&&&&\n";

	// Finally, write the commit to disk and empty the index
	// First, generate the hash of the commit
	std::string contents = commit.str();
	std::string hash = picosha2::hash256_hex_string(contents);

	// And write to the commit file
	std::ofstream file(repositoryPath("commits/" + hash), std::ios::binary);
	if (!file) {
		std::cerr << "Could not create commit.\n";
		exit(1);
	}
	file.write(contents.c_str(), contents.size());
	file.close();

	// Now, clear the indexmap (the file on disk will be truncated at end-of-scope)
	cmap.clear();

	// If we're in a detached state, warn about not updating HEAD and print our hash
	if (detached) {
		std::cerr << "Warning: No branch head updated: You are in a detached state.\n";
		std::cerr << "This commit can be accessed in the future via its hash:\n";
		std::cerr << normalizeReference(hash) << "\n";
	}
	// Else, update the HEAD marker to match this commit
	else {
		std::ofstream head(branchHead(), std::ios::trunc);
		if (!head) {
			remove(repositoryPath("commits/" + hash));
			std::cerr << "Could not create commit.\n";
			exit(2);
		}
		head << hash << "\n";
		head.close();
	}

	emptyDirectory(repositoryPath("index"));

	// Confirm to the user that we succeeded
	std::cout << "Done.\n";
}

// Handles 'commit -a'
// That is, reads the HEAD commit, lists the files named there into a vector...
// Passes that vector into add, and then calls commit
void commitLast() {
	std::string line(getHeadHash());
	std::ifstream last(repositoryPath("commits/"+line));
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
		std::getline(filestream, line, ',');
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
	removeDirectory(repositoryPath("indexCopy")); // Just in case

	// Back up the index
	if (!copyDirectory(repositoryPath("index"), repositoryPath("indexCopy"))) { 
		std::cerr << "Could not back up repository index.\n";
		exit(1);
	}

	// If INDEXMAP_PATH doesn't start with index/, our prior copy didn't get it, so we have to do it ourselves.
	if (INDEXMAP_PATH.find("index/")) {
		if (!copyfile(repositoryPath(INDEXMAP_PATH), repositoryPath("indexCopy/" + INDEXMAP_PATH))) {
			std::cerr << "Could not back up repository index.\n";
			exit(1);
		}
	}

	// Empty the index entirely
	emptyDirectory(repositoryPath("index"));

	for (const auto& file : files) {
		remove(repositoryPath("indexCopy/"+file)); // Make sure the file to be committed is removed from the copied index
	}

	// Add all commandline files
	add(files);
	// And then commit.
	commit();

	removeDirectory(repositoryPath("index"));
	// Restore the index, minus duplicated files
	if (!copyDirectory(repositoryPath("indexCopy"), repositoryPath("index"))) {
		std::cerr << "Could not restore index.\n";
		removeDirectory(repositoryPath("indexCopy"));
		if (mkdir(repositoryPath("index")) && errno != EEXIST) {
			std::cerr << "Also could not create index directory.\n";
			std::cerr << "Please ensure that the \""+REPOSITORY_PATH+"\" directory is writable...\n";
			std::cerr << "Then create a folder named \"index\" inside it before running \"add\" or \"commit\".\n";
			exit(errno);
		}
		else {
			std::cerr << "The index has been emptied.\n";
			exit(2);
		}
	}

	// Restore the Indexmap if we backed it up separately
	if (INDEXMAP_PATH.find("index/")) {
		if (!copyfile(repositoryPath("indexCopy/" + INDEXMAP_PATH), repositoryPath(INDEXMAP_PATH))) {
			std::cerr << "Could not restore index.\n";
			exit(3);
		}
	}
	removeDirectory(repositoryPath("indexCopy"));
}

// Produces a log of the commit history by the commit headers
void log() {
	std::string hash(getHeadHash());
	std::ifstream commit;

	while (hash != "0") {
		commit.open(repositoryPath("commits/" + hash));
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
//  - A branch name
//  - HEAD (which shall be resolved to the complete hash of the head commit of the most recent branch)
void checkout(std::string reference) {
	auto head = getHeadHash(); // For the lockout warning
	std::vector<std::string> branches;

	if (filesInDirectory(repositoryPath("branches/"), branches)) {
		// Not necessarily an error condition (we could have a hash)
		// But definitely a warning condition
		std::cerr << "Warning: Cannot list branches.\n";
		std::cerr << "Checkout may fail unexpectedly if " << reference << " is a branch name.\n";
		std::cerr << "Please ensure that your " << REPOSITORY_PATH << " directory exists and is accessible.\n";
	}

	if (reference == "HEAD") {
		reference = head;
		remove(repositoryPath("COMMIT_LOCK")); // Delete the lock file
	}
	else if (std::find(branches.begin(), branches.end(), reference) != branches.end()) {
		// Branch head checkout
		// Set target branch as current
		std::ofstream head(repositoryPath("HEAD"), std::ios::trunc);
		head << reference << '\n';
		head.close();

		// Set reference to check out the HEAD of the now-current branch
		std::ifstream branch(repositoryPath("branches/" + reference));
		std::getline(branch, reference);

		// Delete lock file
		remove(repositoryPath("COMMIT_LOCK"));
	}
	else if (reference != head) {
		// Create the lock file
		std::ofstream lock(repositoryPath("COMMIT_LOCK"), std::ios::binary);
		lock << reference << "\n";

		// And issue a warning
		std::cerr << "Warning: You are detached from the HEAD commit.\n";
		std::cerr << "Commits made in this state will be lost forever unless you remember their hash or create a new branch for them.\n\n";
	}
	else {
		remove(repositoryPath("COMMIT_LOCK")); // Delete the lock file
	}

	std::ifstream commit(repositoryPath("commits/"+reference), std::ios::binary);
	if (!commit) {
		std::cerr << "Could not open commit " << reference << "\n";
		exit(1);
	}

	std::string line;

	while (line != "&&&&&") { // Advance through commit header
		std::getline(commit, line);
		line = escaped(line, "\r", "");
	}

	// Now, we can iterate over files
	size_t numFiles(0);
	size_t totalSize(0);
	while (true) {
		std::string filename;
		std::getline(commit, filename);
		filename = escaped(filename, "\r", "");
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
		hash = escaped(hash, "/r", "");
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
					result = tolower(std::cin.get());
					std::cin.ignore(1);
					std::cout << std::endl;
				}
				skip = (result != 'n');
			}
		}

		// Unless skip is set, read the file in the commit out to disk
		if (!skip) {
			// If the filename includes a directory mark, we need to go through it and make sure the directory exists before performing checkout.
			if (filename.find('/')!=std::string::npos || filename.find('\\') != std::string::npos) {
				// Split the path into a list of directories
				std::vector<std::string> parts;
				if (filename.find("/"))
					parts = split(filename, '/');
				else
					parts = split(filename, '\\');

				// And then make all those directories (except the last one, which is a filename)
				std::string path;
				for (size_t i = 0; i < parts.size() - 1; ++i) {
					path += parts[i] + '/';
					mkdir(path.c_str());
				}
			}

			std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
			if (!file) {
				std::cerr << "Unable to open file " << filename << " for writing.\n";
				exit(2);
			}

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
			std::getline(commit, line, '&');
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

// Given a branchname and a commit (reference), mark that commit as the tip of a new branch with the given name
// Reference can be anything that checkout understands.
void branchReference(const std::string& branchname, const std::string& ref) {
	std::vector<std::string> branches;
	std::string reference;

	if (filesInDirectory(repositoryPath("branches/"), branches)) {
		// Not necessarily an error condition (we could have a hash)
		// But definitely a warning condition
		std::cerr << "Warning: Cannot list branches.\n";
		std::cerr << "Creation may fail unexpectedly if " << ref << " is a branch name.\n";
		std::cerr << "Please ensure that your " << REPOSITORY_PATH << " directory exists and is accessible.\n";
	}

	if (ref == "HEAD") {
		reference = getHeadHash();
	}
	else if (std::find(branches.begin(), branches.end(), ref) != branches.end()) {
		// If we're given a branch...
		// Set reference to be the HEAD of the given branch
		std::ifstream branch(repositoryPath("branches/" + ref));
		std::getline(branch, reference);
	}
	// Else, assume it's a hash.

	// Confirm that reference now refers to a valid commit
	branches.clear(); // Variable reuse
	if (filesInDirectory(repositoryPath("commits/"), branches)) {
		// Error out if we can't list commits
		// (Definitely do not create a bad branch, that could break things even more)
		std::cerr << "Cannot find reference " << ref << ".\n";
		std::cerr << "Repository may be improperly configured... Have you run init?\n";
		exit(1);
	}

	if (std::find(branches.begin(), branches.end(), ref) != branches.end()) {
		// Commit does not exist
		std::cerr << "Cannot find reference " << ref << ".\n";
		exit(2);
	}

	// Reference is a hash referring to a valid commit.
	// If branchname is already a branch, get confirmation before proceeding
	std::ifstream prior(repositoryPath("branches/" + branchname));
	std::string prev;
	if (prior) {
		std::cerr << "Warning: Branch " << branchname << " already exists.\n";
		std::getline(prior, prev);
		std::cerr << "(current tip at " << prev << ".)\n";
		char ans = '\0';
		while (ans != 'y' && ans != 'n' && ans != '\n') {
			std::cout << "Are you sure you would like to overwrite it (y/N)? ";
			ans = tolower(std::cin.get());
			std::cin.ignore(1);
			std::cout << std::endl;
		}
		if (ans != 'y') {
			std::cerr << "Aborted branch creation.\n";
			exit(3);
		}
	}

	// Create or overwrite branch
	std::ofstream branch(repositoryPath("branches/" + branchname), std::ios::out | std::ios::trunc);
	branch << reference << '\n';
	branch.close();

	// If the new branch tip was the detached head, remove the commit lock
	if (std::ifstream lock = std::ifstream(repositoryPath("COMMIT_LOCK"))) {
		std::string loc;
		std::getline(lock, loc);
		if (loc == reference) {
			remove(repositoryPath("COMMIT_LOCK"));
		}
	}

	// Print success message (dependent on whether this was an overwrite)
	if (prior) {
		std::cout << "Overwrote branch " << branchname << " (was " << prev << ").\n";
	}
	else {
		std::cout << "Successfuly created branch " << branchname << ".\n";
	}
}

// Prints a list of branches, marking the current one with an asterisk iff COMMIT_LOCK isn't present
// If COMMIT_LOCK is present, print a shortened version of the detached HEAD
void branchList() {
	std::vector<std::string> branches;
	if (filesInDirectory(repositoryPath("branches/"), branches)) {
		std::cerr << "Cannot read list of branches.\n";
		exit(1);
	}

	// Get current branch
	std::ifstream HEAD(repositoryPath("HEAD"));
	std::string current;
	std::getline(HEAD, current);
	HEAD.close();

	// Check for detached state
	HEAD.open(repositoryPath("COMMIT_LOCK"), std::ios::binary);
	if (HEAD) {
		// We're detached.
		std::getline(HEAD, current);
		HEAD.close();

		// Print out our detached HEAD
		std::cout << "* HEAD (detached; at " << current.substr(0, 10) << ")\n"; // 10 should be enough characters

		current = ""; // Just in case someone wanted to name their branch after a commit... For some stupid reason
		// (checkout doesn't work in this case, but it's easy enough to make this work)
	}

	// Print out all of our branches, checking to see if they're equal to current
	for (const auto& branch : branches) {
		if (branch == current) {
			std::cout << "* ";
		}
		std::cout << branch << '\n';
	}
}
