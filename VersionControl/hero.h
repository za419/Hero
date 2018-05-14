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

std::string getHeadHash() {
	std::ifstream HEAD(repositoryPath("HEAD"));
	if (!HEAD)
		return "";
	std::string out;
	std::getline(HEAD, out);
	HEAD.close();
	return out;
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
#endif
