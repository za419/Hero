// hero.h: Utility functions shared across different parts of the program
// Unlike Utils.h, this file is strongly linked to this project: While Utils might be suitable to be copied elsewhere, this is not.

#ifndef HERO_H
#define HERO_H
#pragma once

#include "Utils.h"
#include "../PicoSHA2/picosha2.h"
#include <string>
#include <fstream>

const std::string REPOSITORY_PATH(".hero");
const std::string INDEXMAP_PATH("index/map");

// Returns a convertible path to the file which could be accessed by filename from a program whose working directory is REPOSITORY_PATH
CStr repositoryPath(const std::string& filename) {
	return appended(REPOSITORY_PATH, "/"+filename);
}

// Returns a path to the file in which the branch HEAD hash is stored
std::string branchHead() {
	std::ifstream HEAD(repositoryPath("HEAD"));
	if (!HEAD)
		return "";
	std::string out;
	std::getline(HEAD, out);
	HEAD.close();
	return repositoryPath("branches/"+out);
}

std::string getHeadHash() {
	std::ifstream HEAD(branchHead());
	std::string out;
	std::getline(HEAD, out);
	return out;
}

// Converts the reference into one 'printable' format.
// In other words, attempt to make the reference as user-readable as possible
// (c4053b238029d133b65efdea01d468db2a06cbb1fd0e550545e907daea27585c is not very user-friendly)
std::string normalizeReference(std::string ref) {
	// First, see if this is already in 'most-normal' form - A branch name
	std::vector<std::string> branches;
	if (filesInDirectory(repositoryPath("branches/"), branches)) {
		// There is probably something bad going on here, but we shouldn't print an error for it.
		// However, if we cannot read into the list of branches, we can't really do anything to our hash.
		return ref;
	}

	if (std::find(branches.begin(), branches.end(), ref) == branches.end()) {
		// ref is not a branch name.
		// Check to see if ref is a branch head
		const std::string prefix(repositoryPath("branches/"));
		std::string line;
		for (const auto& branch : branches) {
			std::ifstream file(prefix + branch);
			std::getline(file, line);
			if (line == ref) {
				// ref refers to this branch
				ref = branch;
				break;
			}
		}
	}
	return ref;
}

// Returns the SHA256 hash of the stream
std::string hashOfFile(std::istream& ifs) {
	return picosha2::hash256_hex_string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

// Returns the SHA256 hash of the file at filename
std::string hashOfFile(const std::string& filename) {
	std::ifstream ifs(filename, std::ios::binary);
	return hashOfFile(ifs);
}

// Specialize the hasher for an ifstream reference
namespace picosha2 {
	std::string hash256_hex_string(std::ifstream& ifs) {
		return hashOfFile(ifs);
	}

	std::string hash256_hex_string(std::ifstream&& ifs) {
		return hashOfFile(ifs);
	}
}
#endif
