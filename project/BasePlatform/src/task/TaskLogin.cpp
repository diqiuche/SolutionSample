#include "TaskLogin.h"
#include "TaskInterfaceDef.h"

namespace itstation 
{

	CTaskLogin::CTaskLogin( TaskData task_desc, session_ptr ptrSocket ):EngineTask(task_desc, ptrSocket)
	{
		
	}

	CTaskLogin::~CTaskLogin(void)
	{
	}

	//@brief    : execute implement method, implement task execute operator
	//@return   : return 0 if success, or -1 if failed
	int CTaskLogin::executeImpl()
	{
		APP_LOGIN *ptrData;

		ASSET_MESSAGE_STRUCT Msg;
		TCP_MSG_HEAD header;

		task_desc_.get_task_message( Msg );

		header.datatype = Msg.header.datatype;
		header.datasize = sizeof(int);

		if ( Msg.ptrData==NULL || Msg.header.datasize!=sizeof(APP_LOGIN) || Msg.header.datatype!=2 )
		{
			return -1;
		}

		ptrData = (APP_LOGIN *)( Msg.ptrData );

		try
		{

		}
		catch( ... )
		{
			return -1;
		}

		return 0;
	}
} //namespace itstation 