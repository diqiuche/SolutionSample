
#include "TaskFactory.h"
#include "Applog.h"

#include "../task/TaskRegister.h"
#include "../task/TaskLogin.h"

namespace itstation 
{
	TaskFactory* TaskFactory::instance_ = 0;
	common::Mutex TaskFactory::m_mutex;

	TaskFactory::TaskFactory() {
	}

	TaskFactory::~TaskFactory() {
	}

	TaskFactory* TaskFactory::getInstance()
	{
		if(!instance_)
		{
			common::MutexLocker lock(&m_mutex);
			if(!instance_)
			{
				instance_ = new TaskFactory();
			}
		}
		return instance_;    
	}	

	TaskID TaskFactory::TaskCodeToTaskID(const int& code) {
		switch ( code )
		{
		case 1:	//APP用户注册
			return eRegister;	

		case 2://APP用户登录
			return eLogin;      

		default:
			return eUnknownTaskID;
		}
	}

	std::shared_ptr<EngineTask> TaskFactory::createEngineTask(ASSET_MESSAGE_STRUCT Message, session_ptr ptrSocket) {	
		int id = Message.header.datatype;
		TaskID t = TaskCodeToTaskID( id );
		std::shared_ptr<TaskData> taskData(new TaskData());
		taskData->set_task_message( Message );

		switch (t) {
		case eRegister:
			return std::shared_ptr<EngineTask>(new CTaskRegister(*taskData, ptrSocket));

		case eLogin:
			return std::shared_ptr<EngineTask>(new CTaskLogin(*taskData, ptrSocket));

		default:
			{

			}
		}

		return std::shared_ptr<EngineTask>((EngineTask *)0);
	}

} //namespace itstation 
