
#include "RequestCommand.h"
#include "Applog.h"

namespace itstation 
{    

RequestCommand::RequestCommand() 
{
}

RequestCommand::~RequestCommand() 
{
} 

void RequestCommand::exeCommandImpl()
{  
    for (auto it = tasks_.begin(); it != tasks_.end(); it++) 
	{
		std::shared_ptr<EngineTask> task_ptr = it->second;

        task_ptr->executeImpl();
    }
}

} // namespace itstation 

