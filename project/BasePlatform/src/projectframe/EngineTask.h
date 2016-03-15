////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Engine task class define task standard interface.
/// One task corresponding to the task definition in the request.
///
/// @version $Id: EngineTask.h 1068 2012-02-14 07:46:19Z liuzq $
////////////////////////////////////////////////////////////////////////////////
#ifndef WORKPLAN_ENGINETASK_H_
#define WORKPLAN_ENGINETASK_H_

#ifndef _WIN32
//#define _DEBUG_SHOW_
#endif

#include "TaskData.h"
#include "Applog.h"
#include <string>
#include <vector>
#include <list>
#ifdef __GNUG__
#include <sys/time.h>	
#endif
#include "TcpSocket.h"
using namespace std;

namespace itstation
{
	// Engine task.
	class EngineTask{
	public:
		EngineTask(){};
		// Create EngineTask.
		EngineTask( TaskData task_desc, session_ptr ptrSocket);

		virtual ~EngineTask();

		virtual int executeImpl() = 0;

		TaskData task_desc_;
		session_ptr m_ptrSocket; 
	};
} // namespace itstation
#endif
