#ifndef EYEGLE_COMMON_GLOBAL_H_
#define EYEGLE_COMMON_GLOBAL_H_

#include <string>
#include <stdlib.h>

extern std::string g_eyegle_home;

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//Common宏
#ifdef COMMON_EXPORT
#define COMMON_API  __declspec(dllexport)
#else 
#define COMMON_API  __declspec(dllimport)
#endif  //COMMON_EXPORT

#else	//LINUX平台，定义导出宏为空
#include <unistd.h>
#define COMMON_API
#endif  //WIN32

class COMMON_API Global {
public:
	static const Global* GetInstance();
public:
	std::string fnd_home;

private:
	Global();
	void Init();


	static Global* m_instance;
};

#endif
