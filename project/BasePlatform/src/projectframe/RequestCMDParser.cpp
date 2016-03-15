#include <memory>
#include "RequestCMDParser.h"
#include "TaskFactory.h"
#include "EngineTask.h"
#include "Applog.h"

namespace itstation 
{
	RequestCMDParser::RequestCMDParser() 
	{
	}
	RequestCMDParser::~RequestCMDParser() 
	{
	}


	int RequestCMDParser::parseTaskGroup(ASSET_MESSAGE_STRUCT Message, RequestCommand& cmd, session_ptr ptrSocket) 
	{
		std::shared_ptr<EngineTask> task( TaskFactory::getInstance()->createEngineTask(Message, ptrSocket));
		if(!task)
		{
			throw "Unknown task id";
		}			

		cmd.tasks_[Message.header.datatype] = task;

		return 0;
	}
} // namespace itstation
