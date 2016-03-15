#ifndef EYEGLE_COMMON_THREAD_H_  
#define EYEGLE_COMMON_THREAD_H_  
 
#include <stdexcept>
#include <iostream>
#include "Global.h"
#include "include_common.h"
#include "SpinLock.h"
#include "Mutex.h"
#include "Condition.h"

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif

using namespace std;

namespace itstation {
namespace common {

class COMMON_API Thread {
public:
	enum ThreadState {
		RUNING,
		REQUEST_STOP,
		STOPED
	};

private:

#ifdef WIN32
	static void ThreadProc(void* arg)
#else
	static void* ThreadProc(void* arg)
#endif
	{
		Thread* thd = (Thread*)arg;
		thd->OnRun();
		thd->SetState(STOPED);
		cout<<"Thread stoped"<<endl;
	}

public:	
	Thread();
	virtual ~Thread();

	virtual void OnRun() = 0;

	bool IsRuning();

	ThreadState GetState();

	void Start();

	void Join();

	void Stop();

private:
	void SetState(ThreadState state);

private:
	mutable ThreadState m_state;
	SpinLock* m_lock;

#ifdef WIN32
#else
	pthread_t m_tid;
#endif
	
};

}
}
#endif
