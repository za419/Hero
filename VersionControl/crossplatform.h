// Crossplatform.h: Simple wrappers to increase cross-platform compatibility

#ifndef CROSSPLATFORM_H
#include <direct.h>
#include <cstdlib>

#if defined(_WIN32)
int mkdir(const char* dirname) {
	return _mkdir(dirname);
}
#endif
#endif // !CROSSPLATFORM_H
