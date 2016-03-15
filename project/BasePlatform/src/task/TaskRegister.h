#ifndef WORKPLAN_TASK_REGISTER_H_
#define WORKPLAN_TASK_REGISTER_H_

#include "../projectframe/EngineTask.h"

namespace itstation 
{
	class CTaskRegister :
		public EngineTask
	{
	public:
		CTaskRegister(void){};
		explicit CTaskRegister( TaskData task_desc, session_ptr ptrSocket );

		virtual ~CTaskRegister(void);

		//@brief    : execute implement method, implement task execute operator
		//@return   : return 0 if success, or -1 if failed
		virtual int executeImpl();
	};

} // namespace itstation 
#endif

