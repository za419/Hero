// indexmap.h: Defines the indexmap class, which handles the translation of filenames to hashes (for directory support purposes)

#ifndef INDEXMAP_H
#define INDEXMAP_H
#pragma once

#include "../../PicoSHA2/picosha2.h"

#include <string>
#include <utility>
#include <fstream>
#include <map>

class Commitmap;

class Indexmap {
public:
	using Filename = std::string;
	using Hash = std::string;

	using iterator = std::map<Filename, Hash>::iterator;
	using const_iterator = std::map<Filename, Hash>::const_iterator;

	Indexmap() {}
	Indexmap(const Indexmap& map):m_map(map.m_map) {}
	Indexmap(Indexmap&& map):m_map(std::move(map.m_map)) {}
	Indexmap(const Commitmap& map) {
		for (const auto& it : map) {
			m_map[it.second] = it.first;
		}
	}
	
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

	iterator begin() {
		return m_map.begin();
	}

	const_iterator begin() const {
		return m_map.cbegin();
	}

	const_iterator cbegin() const {
		return m_map.cbegin();
	}

	iterator end() {
		return m_map.end();
	}

	const_iterator end() const {
		return m_map.cend();
	}

	const_iterator cend() const {
		return m_map.cend();
	}

	bool exists(const Filename& file) const {
		return m_map.count(file) > 0;
	}

	Hash& operator[] (const Filename& file) {
		return m_map[file];
	}

	const Hash& operator[] (const Filename& file) const {
		return m_map.at(file);
	}

	static Indexmap loadFrom(std::istream& stream) {
		Indexmap result;
		stream >> result;
		return result;
	}

	static Indexmap loadFrom(const std::string& file) {
		return loadFrom(std::ifstream(file));
	}

	static Indexmap loadFrom(const char* file) {
		return loadFrom(std::ifstream(file));
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

class Commitmap {
public:
	using Filename = std::string;
	using Hash = std::string;

	using iterator = std::map<Filename, Hash>::iterator;
	using const_iterator = std::map<Filename, Hash>::const_iterator;

	Commitmap() {}
	Commitmap(const Commitmap& map) :m_map(map.m_map) {}
	Commitmap(Commitmap&& map) :m_map(std::move(map.m_map)) {}
	Commitmap(const Indexmap& map) {
		for (const auto& it : map) {
			m_map[it.second] = it.first;
		}
	}

	void add(const Filename& file) {
		m_map[picosha2::hash256_hex_string(std::ifstream(file))] = file;
	}

	Hash getHash(const Filename& file) const {
		for (const auto& it : m_map) {
			if (it.second == file) {
				return it.first;
			}
		}
		return "";
	}

	Filename getFile(const Hash& file) const {
		return m_map.at(file);
	}

	iterator begin() {
		return m_map.begin();
	}

	const_iterator begin() const {
		return m_map.cbegin();
	}

	const_iterator cbegin() const {
		return m_map.cbegin();
	}

	iterator end() {
		return m_map.end();
	}

	const_iterator end() const {
		return m_map.cend();
	}

	const_iterator cend() const {
		return m_map.cend();
	}

	bool exists(const Hash& file) const {
		return m_map.count(file) > 0;
	}

	Filename& operator[] (const Hash& file) {
		return m_map[file];
	}

	const Filename& operator[] (const Hash& file) const {
		return m_map.at(file);
	}

	static Commitmap loadFrom(std::istream& stream) {
		Commitmap result;
		stream >> result;
		return result;
	}

	static Commitmap loadFrom(const std::string& file) {
		return loadFrom(std::ifstream(file));
	}

	static Commitmap loadFrom(const char* file) {
		return loadFrom(std::ifstream(file));
	}

	friend std::ostream& operator << (std::ostream& stream, const Commitmap& map) {
		for (const auto& it : map.m_map) {
			stream << it.second << ',' << it.first << '\n';
		}
		return stream;
	}

	friend std::istream& operator >> (std::istream& stream, Commitmap& map) {
		std::string buffer;
		std::getline(stream, buffer);
		while (stream) {
			std::string first, second;
			auto sep(buffer.find_first_of(','));
			first = buffer.substr(0, sep);
			second = buffer.substr(sep + 1);
			map.m_map[second] = first;
			std::getline(stream, buffer);
		}
		return stream;
	}
protected:
	std::map<Hash, Filename> m_map;
};
#endif // !INDEXMAP_H
