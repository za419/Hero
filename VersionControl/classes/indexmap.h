// indexmap.h: Defines the indexmap class, which handles the translation of filenames to hashes (for directory support purposes)

#ifndef INDEXMAP_H
#define INDEXMAP_H
#pragma once

#include "../../PicoSHA2/picosha2.h"

#include <string>
#include <utility>
#include <fstream>
#include <map>

class Indexmap {
public:
	using Filename = std::string;
	using Hash = std::string;

	Indexmap() {}
	Indexmap(const Indexmap& map):m_map(map.m_map) {}
	Indexmap(Indexmap&& map):m_map(std::move(map.m_map)) {}
	
	void add(const Filename& file) {
		m_map[file] = picosha2::hash256_hex_string(std::ifstream(file));
	}

	Hash getHash(const Filename& file) const {
		return m_map.at(file);
	}

	Filename getFile(const Hash& file) const {
		for (const auto& it : m_map) {
			if (it.second == file) {
				return it.first;
			}
		}
		return "";
	}

	Hash& operator[] (const Filename& file) {
		return m_map[file];
	}

	const Hash& operator[] (const Filename& file) const {
		return m_map.at(file);
	}

	friend std::ostream& operator << (std::ostream& stream, const Indexmap& map) {
		for (const auto& it : map.m_map) {
			stream << it.first << ',' << it.second << '\n';
		}
		return stream;
	}

	friend std::istream& operator >> (std::istream& stream, Indexmap& map) {
		std::string buffer;
		std::getline(stream, buffer);
		while (stream) {
			std::string first, second;
			auto sep(buffer.find_first_of(','));
			first = buffer.substr(0, sep);
			second = buffer.substr(sep + 1);
			map.m_map[first] = second;
			std::getline(stream, buffer);
		}
		return stream;
	}
protected:
	std::map<Filename, Hash> m_map;
};
#endif // !INDEXMAP_H
