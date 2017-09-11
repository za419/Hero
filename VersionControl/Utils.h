// Utils.h: Small general-purpose utility functions

#ifndef UTILS_H
#define UTILS_H
#pragma once

#include <string>

std::string escaped(std::string source, const std::string& term, const std::string& replacement) {
	size_t i(0);
	size_t termLength(term.length());
	size_t replacementLength(replacement.length());
	while ((i = source.find(term, i)) != std::string::npos) {
		source.replace(i, termLength, replacement);
		i += replacementLength;
	}
	return source;
}
#endif
