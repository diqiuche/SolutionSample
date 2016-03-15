#include "Condition.h"

namespace itstation {
namespace common {

Condition::Condition()
{
#if defined WIN32
    ::InitializeConditionVariable(&m_cond);
#else
    int rs = pthread_cond_init(&m_cond, NULL);
    assert(0 == rs);
#endif
}

Condition::~Condition()
{
#if defined WIN32
    /* no need to destroy in windows */
#else
    int rs = pthread_cond_destroy(&m_cond);
    assert(0 == rs);
#endif
}

bool Condition::Signal()
{
#if defined WIN32
    ::WakeConditionVariable(&m_cond);
    return true;
#else
    return 0 == pthread_cond_signal(&m_cond);
#endif
}

bool Condition::Broadcast()
{
#if defined WIN32
    ::WakeAllConditionVariable(&m_cond);
    return true;
#else
    return 0 == pthread_cond_broadcast(&m_cond);
#endif
}

/**
 * release lock, wait for signal or interrupt, lock and wake
 */
bool Condition::Wait(condition_lock_type *mutex)
{
    assert(NULL != mutex);
#if defined WIN32
    return SleepConditionVariableCS(&m_cond, mutex->InnerMutex(), INFINITE);
#else
    return 0 == pthread_cond_wait(&m_cond, mutex->InnerMutex());
#endif
}

/**
 * work the same as above
 */
bool Condition::TimedWait(condition_lock_type *mutex, unsigned s, unsigned ms/* = 0*/)
{
    assert(NULL != mutex);
#if defined WIN32
    DWORD dwMilliseconds = s * 1000 + ms;
    return SleepConditionVariableCS(&m_cond, mutex->InnerMutex(), dwMilliseconds);
#else
    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += s;
    abstime.tv_nsec += ((long)ms) * 1000 * 1000;
    return 0 == pthread_cond_timedwait(&m_cond, mutex->InnerMutex(), &abstime);
#endif
}

}
}
