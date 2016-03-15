#ifndef EYEGLE_COMMON_CONDITION_H_  
#define EYEGLE_COMMON_CONDITION_H_  

#include <assert.h> 
#if defined WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>  
#else  
#include <pthread.h>  
#endif  

#include "Mutex.h"

namespace itstation {
namespace common {

class COMMON_API Condition
{
#if defined WIN32
    CONDITION_VARIABLE m_cond;
#else
    pthread_cond_t m_cond;
#endif

public:

	typedef Mutex condition_lock_type;

    Condition();

    ~Condition();

    bool Signal();

    bool Broadcast();

    /**
     * release lock, wait for signal or interrupt, lock and wake
     */
    bool Wait(condition_lock_type *mutex);

    /**
     * work the same as above
     */
    bool TimedWait(condition_lock_type *mutex, unsigned s, unsigned ms = 0);

};

}
}
#endif
