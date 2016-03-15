#include "Mutex.h"

namespace itstation {
namespace common {

Mutex::Mutex ()
{
#if defined WIN32
    InitializeCriticalSection(&m_mutex);
#else
	int rc;
#if defined(__linux) && !defined(__USE_UNIX98)
	const pthread_mutexattr_t attr = { PTHREAD_MUTEX_ERRORCHECK_NP };
#else
	pthread_mutexattr_t attr;
	rc = pthread_mutexattr_init(&attr);
	assert(rc == 0);
	rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	assert(rc == 0);
#endif
	rc = pthread_mutex_init(&m_mutex, &attr);

#if defined(__linux) && !defined(__USE_UNIX98)
	// Nothing to do
#else
	rc = pthread_mutexattr_destroy(&attr);
	assert(rc == 0);
#endif

#endif
}

Mutex::~Mutex ()
{
#if defined WIN32
        DeleteCriticalSection(&m_mutex);
#else
    int rs = ::pthread_mutex_destroy(&m_mutex);
    assert(0 == rs);
#endif
}

}
}
