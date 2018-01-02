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

// Simple class, holds a dynamically allocated string whose lifecycle the class manages.
// Note that this is far less featured than a class like std::string. This class is not intended for any sort of use as its own object.
// Instead, it is intended to hold a cstring allocated on the heap in such a way that the object itself can be passed to functions which accept a cstring (without freeing it)
// In effect, this is to abstract away the memory management part of working with a cstring, in such a way that its invisible most of the time.
class CStr {
public: 
	CStr() noexcept {
		m_data = nullptr;
	}

	explicit CStr (const char* str) {
		m_data = m_strdup(str);
	}

	CStr(const CStr& str) {
		m_data = m_strdup(str.m_data);
	}

	CStr(CStr&& str) {
		m_data = str.m_data;
		str.m_data = nullptr;
	}

	virtual ~CStr() noexcept {
		delete[] m_data;
	}

	CStr& operator = (const CStr& str) {
		if (this != &str) {
			delete[] m_data;
			m_data = m_strdup(str.m_data);
		}
		return *this;
	}

	CStr& operator = (CStr&& str) {
		if (this != &str) {
			m_data = str.m_data;
			str.m_data = nullptr;
		}
		return *this;
	}

	std::string asStdString() const noexcept {
		return m_data;
	}

	operator char* () noexcept {
		return m_data;
	}

	operator const char* () const noexcept {
		return m_data;
	}
protected:
	// Returns a pointer to a copy of str allocated on the heap by new
	char* m_strdup(const char* str) {
		char* out(new char[strlen(str) + 1]);
		strcpy(out, str);
		return out;
	}
protected:
	char* m_data;
};

// Returns prefix+suffix as a c-string (equivalent)
CStr appended(std::string prefix, const std::string& suffix) {
	prefix += suffix;
	return CStr(prefix.c_str());
}
#endif
