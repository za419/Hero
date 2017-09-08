// Crossplatform.h: Simple wrappers to increase cross-platform compatibility

#ifndef CROSSPLATFORM_H
#define CROSSPLATFORM_H
#pragma once

#include <cstdlib>
#include <vector>

// First, a shim for mkdir
#if defined(_WIN32)
#include <direct.h>
#define mkdir(dirname) _mkdir((dirname))
#else
#include <sys/stat.h>
#endif

// Next, a function to copy files
#if defined(_WIN32)
#include <Windows.h>
#else
#include <sys/sendfile.h>
#include <fnctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif
// Returns whether the operation succeeded
bool copyfile(const char* source, const char* dest) {
#if defined(_WIN32)
	return CopyFile(source, dest, false);
#else // I'm not sure if this will work. I think it will, but...
	int src = open(source, O_RDONLY, 0);
	int dest = open(dest, O_WRONLY | O_CREAT | O_TRUNC);

	struct stat stat_source;
	fstat(src, &stat_source);

	bool out = sendfile(dest, src, 0, stat_source.st_size);

	close(src);
	close(dest);
#endif
}
#endif // !CROSSPLATFORM_H

// Then, a function to list (regular) files in a directory
#if defined(_WIN32)
#include <Windows.h>
#else
#endif
// Returns either 0 or an error code
int filesInDirectory(std::string dir, std::vector<std::string>& out) {
	out.clear();
#if defined(_WIN32) // Adapted from MSDN example: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365200(v=vs.85).aspx
	HANDLE hFind;
	WIN32_FIND_DATA ffd;

	dir += "\\*";
	if ((hFind = FindFirstFile(dir.c_str(), &ffd)) != INVALID_HANDLE_VALUE) {
		do {
			// Skip directories
			if (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				continue;
			out.push_back(dir + "\\" + ffd.cFileName);
		} while (FindNextFile(hFind, &ffd));
		DWORD err(GetLastError());
		if (err == ERROR_NO_MORE_FILES || err == ERROR_SUCCESS)
			return 0;
		else
			return (int)err;
	}
	else
		return (int)ERROR_INVALID_HANDLE;
#else
#endif
}
