#include "SSemaphore.h"

namespace itstation {
namespace common {
	SSemaphore::SSemaphore(long initial/* = 0 */, const char *SemName/*=NULL*/)
	{
#if defined(WIN32)
		sem_=CreateSemaphore(0,initial,0x7fffffff,0);
		if(sem_==NULL)
		{
			sem_=INVALID_HANDLE_VALUE;
		}
#else
		if(SemName)
		{
			SEMName=SemName;
		}
		sem_=sem_open(SemName,O_CREAT,0644,initial);
#endif
	}

#if defined(WIN32)
	bool SSemaphore::Release(int counter_)
	{
		if(INVALID_HANDLE_VALUE==sem_)
		{
			return false;
		}
		return ReleaseSemaphore(sem_,counter_,NULL)==TRUE;
	}

	bool SSemaphore::Wait(int millisecond_)
	{
		if(INVALID_HANDLE_VALUE==sem_)
		{
			return false;
		}
		return WAIT_OBJECT_0==WaitForSingleObject(sem_,millisecond_);
	}

#else

	bool SSemaphore::Release()
	{
		return sem_post(sem_)==0;
	}


	bool SSemaphore::Wait(int millisecond_)
	{
		if(-1==millisecond_)
		{
			return 0==sem_wait(sem_);
		}
		else
		{
			struct timespec ts;
			ts.tv_sec= millisecond_/1000;
			ts.tv_nsec=(millisecond_%1000)*1000000L;

			/* Try to lock SSemaphore */
			return 0 == sem_timedwait(sem_, &ts);
		}
	}

#endif

	bool SSemaphore::TryWait()
	{

#if defined(WIN32)
		if(INVALID_HANDLE_VALUE==sem_)
		{
			return false;
		}
		if(WAIT_OBJECT_0==WaitForSingleObject(sem_,0))
		{
			return true;
		}
		else
		{
			return false;
		}

#else	
		if(NULL==sem_)
		{
			return false;
		}
		if(sem_trywait(sem_)==0)
		{
			return true;
		}
		else
		{
			return false;
		}
#endif
	}

	SSemaphore::~SSemaphore()
	{
#if defined(WIN32)
		if(INVALID_HANDLE_VALUE!=sem_)
		{
			CloseHandle(sem_);
		}			
#else	
		sem_close(sem_);
		sem_unlink(SEMName.c_str());
#endif

	}
}
}
