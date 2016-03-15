#ifndef EYEGLE_COMMON_DIRECTORY_H_
#define EYEGLE_COMMON_DIRECTORY_H_

#include <string>
#include <set>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <direct.h>
#include <io.h>
#include <windows.h>
#include <tchar.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include "Global.h"

#ifdef WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif

using namespace std;

bool COMMON_API IsDirExist(const string& dir_path);

bool COMMON_API MakeDir(const string& dirpath);

bool COMMON_API GetFilesInDir(string path, set<string>& files);

#endif
