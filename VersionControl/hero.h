// hero.h: Utility functions shared across different parts of the program
// Unlike Utils.h, this file is strongly linked to this project: While Utils might be suitable to be copied elsewhere, this is not.

#ifndef HERO_H
#define HERO_H
#pragma once

#include <string>
#include <fstream>

const std::string REPOSITORY_PATH(".vcs");

std::string getHeadHash() {
	std::ifstream HEAD(REPOSITORY_PATH+"/HEAD");
	if (!HEAD)
		return "";
	std::string out;
	std::getline(HEAD, out);
	HEAD.close();
	return out;
}
#endif
