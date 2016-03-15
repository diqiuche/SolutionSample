#include "SpinLock.h"

namespace itstation {
namespace common {

SpinLock::SpinLock(int spin_count/* = 10000*/)
{  
#if defined(WIN32)  
	InitializeCriticalSectionAndSpinCount( &m_critical_section, spin_count);	//旋转spin_count个时钟周期后进入阻塞
#else  
	int rs = ::pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);  
	assert(0 == rs);  
#endif  
}  

SpinLock::~SpinLock()  
{  
#if defined(WIN32)  
	DeleteCriticalSection(&m_critical_section);  
#else  
	int rs = ::pthread_spin_destroy(&m_spinlock);  
	assert(0 == rs);  
#endif  
}  

}
}
