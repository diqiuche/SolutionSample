/************************************************************************/
/* 内存信号量，可以用于线程（多用于）/进程间同步（信号灯要放在共享内存中） 
/************************************************************************/
#ifndef EYEGLE_COMMON_SEMAPHORE_H_  
#define EYEGLE_COMMON_SEMAPHORE_H_  

#if defined(WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#else
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#endif

#include "Global.h"
#include "DateTime.h"

namespace itstation {
namespace common {


class COMMON_API Semaphore {
public:
	Semaphore(long initial = 0);

	~Semaphore();

	bool wait() const;
	bool timedWait(const DateTime&) const;
	bool post(int count = 1) const;

private:
#if defined(WIN32)
	mutable HANDLE sem_;
#else
	mutable sem_t sem_;
#endif
};

}
}

#endif
