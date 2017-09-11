// VersionControl.h: Utility functions shared across different parts of the program
// Unlike Utils.h, this file is strongly linked to this project: While Utils might be suitable to be copied elsewhere, this is not.

#ifndef VERSIONCONTROL_H
#define VERSIONCONTROL_H
#pragma once

#include <string>
#include <fstream>

std::string getHeadHash() {
	std::ifstream HEAD(".vcs/HEAD");
	std::string out;
	std::getline(HEAD, out);
	return out;
}

#endif
