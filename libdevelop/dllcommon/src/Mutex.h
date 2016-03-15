/*
互斥锁：线程会从sleep（加锁）——>running（解锁），过程中有上下文的切换，cpu的抢占，信号的发送等开销。

互斥锁用于临界区持锁时间比较长的操作，比如下面这些情况都可以考虑

1 临界区有IO操作

2 临界区代码复杂或者循环量大

3 临界区竞争非常激烈

4 单核处理器
*/
#ifndef EYEGLE_COMMON_MUTEX_H_  
#define EYEGLE_COMMON_MUTEX_H_  

#include <assert.h> 
#if defined WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  
#else  
#include <pthread.h>  
#include <errno.h>
#endif  

#include "Global.h"

namespace itstation {
namespace common {

//不允许重复锁
class COMMON_API Mutex
{
#if defined WIN32
    mutable CRITICAL_SECTION m_mutex;
#else
    mutable pthread_mutex_t m_mutex;
#endif

public :
    Mutex ();

    ~Mutex ();

#if defined WIN32
	CRITICAL_SECTION* InnerMutex() { return &m_mutex; }
#else
	pthread_mutex_t* InnerMutex() { return &m_mutex; }
#endif

    /**
     * lock the mutex, which may blocked the thread
     */
    void Lock();

    /**
     * unlock the mutex
     */
    void Unlock();

    /**
     * try lock the mutex
     * @return
     *      true, if lock successed
     */
    bool Trylock();
};

inline void Mutex::Lock()
{
#if defined WIN32
	EnterCriticalSection(&m_mutex);  
	assert(m_mutex.RecursionCount == 1);
#else
    int rs = pthread_mutex_lock(&m_mutex);
    assert(0 == rs);
#endif
}

inline void Mutex::Unlock()
{
#if defined WIN32
	assert(m_mutex.RecursionCount == 1);
	LeaveCriticalSection(&m_mutex);
#else
    int rs = pthread_mutex_unlock(&m_mutex);
    assert(0 == rs);
#endif
}

inline bool Mutex::Trylock()
{
#if defined WIN32
	if(!TryEnterCriticalSection(&m_mutex)) { return false; }

	if(m_mutex.RecursionCount > 1) {
		LeaveCriticalSection(&m_mutex);
		throw ("Thread locked expcetion! LeaveCriticalSection");
	}
	return true;
#else
    int rs = pthread_mutex_trylock(&m_mutex);
	if(rs != 0 && rs != EBUSY) {
		throw ("Invalid mutex");
	}
    /** returned values :
        *  0, lock ok
        *  EBUSY, The mutex is already locked.
        *  EINVAL, Mutex is not an initialized mutex.
        *  EFAULT, Mutex is an invalid pointer.
        */
    return 0 == rs;
#endif
}
//TODO:与Locker合并
class MutexLocker {
public:
	MutexLocker(Mutex* mutex) : m_mutex(mutex) {
		m_mutex->Lock();
	}

	~MutexLocker() {
		m_mutex->Unlock();
	}

private:
	Mutex* m_mutex;
};

}
}

#endif
