#include "TaskRegister.h"
#include "TaskInterfaceDef.h"
#include "../projectframe/ProjectServer.h"

namespace itstation 
{

	CTaskRegister::CTaskRegister( TaskData task_desc, session_ptr ptrSocket )
		:EngineTask(task_desc, ptrSocket)
	{
	}


	CTaskRegister::~CTaskRegister(void)
	{
	}

	//@brief    : execute implement method, implement task execute operator
	int CTaskRegister::executeImpl()
	{
		APP_REGISTER *ptrData;

		ASSET_MESSAGE_STRUCT Msg;
		TCP_MSG_HEAD header;

		task_desc_.get_task_message( Msg );

		header.datatype = Msg.header.datatype;
		header.datasize = sizeof(int);

		if ( Msg.ptrData==NULL || Msg.header.datasize!=sizeof(APP_REGISTER) || Msg.header.datatype!=1 )
		{
			return -1;
		}

		ptrData = (APP_REGISTER *)(Msg.ptrData);

		try
		{
			bool bRet = false;

			APP_LOG(common::Applog::LOG_INFO) << "task register: " << ptrData->num;
#if 1
			APP_REGISTER sp = {0};
			sp.num = 1;

			header.datasize = sizeof(APP_REGISTER);
			if ( m_ptrSocket != nullptr )
			{
				CProjectServer::GetInstance()->SendMessages(1, m_ptrSocket, (const char*)&sp, header);
			}
			
			//CProjectServer::GetInstance()->SendMessages(2, (const char*)&sp, header);
#endif
		}
		catch( ... )
		{
			return -1;
		}

		return 0;
	}
} //namespace itstation 