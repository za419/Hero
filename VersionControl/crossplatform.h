// Crossplatform.h: Simple wrappers to increase cross-platform compatibility

#ifndef CROSSPLATFORM_H
#include <cstdlib>

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
