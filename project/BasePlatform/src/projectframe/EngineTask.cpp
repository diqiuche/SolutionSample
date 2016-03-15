#include "EngineTask.h"

namespace itstation
{

	EngineTask::EngineTask( TaskData task_desc, session_ptr ptrSocket )
	{
		task_desc_ = task_desc;
		m_ptrSocket = ptrSocket;
	}

	EngineTask::~EngineTask() 
	{
	}

} // namespace itstation
