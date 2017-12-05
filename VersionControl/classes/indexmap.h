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
		return m_map[file];
	}
protected:
	std::map<Filename, Hash> m_map;
};
#endif // !INDEXMAP_H
