#include "Semaphore.h"

namespace itstation {
namespace common {

Semaphore::Semaphore(long initial/* = 0 */)
{
#if defined(WIN32)
	sem_ = CreateSemaphore(0, initial, 0x7fffffff, 0);
#else
	//第二个参数为0,则在同一个线程间共享
	sem_init(&sem_, 0 ,initial);
#endif
}

Semaphore::~Semaphore()
{
#if defined(WIN32)
	CloseHandle(sem_);
#elif __GNUG__
	sem_destroy(&sem_);
#endif
}

bool Semaphore::wait() const
{
#if defined(WIN32)
	int rc = WaitForSingleObject(sem_, INFINITE);
	if (rc != WAIT_OBJECT_0) {
		return false;
	}
#elif __GNUG__
	sem_wait(&sem_);
#endif

	return true;
}

bool Semaphore::timedWait(const DateTime& time) const
{
	return false;
}

//用于以原子操作的方式将信号量的值加1
bool Semaphore::post(int count/* = 1 */) const
{
#if defined(WIN32)    
	ReleaseSemaphore(sem_, count, 0);
#else
	sem_post(&sem_);
#endif

	return true;
}

}
}
