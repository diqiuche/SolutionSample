
#ifndef EYEGLE_INCLUDE_COMMON_H_ 
#define EYEGLE_INCLUDE_COMMON_H_

#if defined __GNUG__
#define _Q_Linux
#else
#if defined _MSC_VER
#define _Q_WIN
#endif
#endif


#if defined _Q_WIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define q_sleep(milliseconds) Sleep(milliseconds)
#define get_th_id() GetCurrentThreadId()

#elif defined _Q_Linux
#include <pthread.h>
#include <semaphore.h>
#include <sys/syscall.h>

#define q_sleep(milliseconds) usleep(milliseconds*1000)
#define get_th_id() syscall(SYS_gettid)

#elif defined _QDEF_UNIX

#endif

#if defined(_WIN32) || defined(_WIN64)
#include <math.h>
#define fmax max
#define fmin min
#define fabs abs
#endif


#endif

