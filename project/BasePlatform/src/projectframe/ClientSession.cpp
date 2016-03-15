#include <iostream>
#include <memory>
#include "ClientSession.h"
#include "RequestCMDParser.h"
#include "RequestCommand.h"
#include <Applog.h>
#include <string.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

namespace itstation {
	ClientSession::ClientSession( int nType )  
		:m_nType(nType)
	{    

	}

	ClientSession::~ClientSession() 
	{
		APP_LOG(common::Applog::LOG_INFO) << __FUNCTION__;
	}


#ifndef _WIN32
	int timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y) 
	{ 
		int nsec; 

		if ( x->tv_sec>y->tv_sec ) 
			return -1; 

		if ( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec) ) 
			return -1; 

		result->tv_sec = ( y->tv_sec-x->tv_sec ); 
		result->tv_usec = ( y->tv_usec-x->tv_usec ); 

		if (result->tv_usec<0) 
		{ 
			result->tv_sec--; 
			result->tv_usec+=1000000; 
		} 

		return 0; 
	}
#endif

	void ClientSession::OnMessageStream( const char* stream, TCP_MSG_HEAD &header )
	{ 
		int nDataSize = header.datasize;

		if ( nDataSize<1 )
		{
			APP_LOG(common::Applog::LOG_ERROR) << "Recv stream msg error!";
			return;
		}
#if 1
#ifndef _WIN32	//测试性能
		struct timeval start,stop,diff; 

		gettimeofday(&start,0); 
#endif
#endif

		ASSET_MESSAGE_STRUCT message;

		message.header.datatype			= header.datatype;
		message.header.datasize			= nDataSize;
		message.ptrData					= new char[nDataSize];
		if ( message.ptrData==NULL )
		{
			APP_LOG(common::Applog::LOG_ERROR) << "Memory exhausted!";
			exit( -444 );
		}

		if ( message.ptrData )
		{
			memcpy( message.ptrData, stream, nDataSize );

			std::shared_ptr<RequestCMDParser> cmdParse(new RequestCMDParser());
			std::shared_ptr<RequestCommand> command(new RequestCommand());

#if 0
			APP_LOG(common::Applog::LOG_INFO) << "Start to parse task = " << message.header.datatype;
#endif

			try
			{
				cmdParse->parseTaskGroup(message, *command, GetSessionPtr());
			}
			catch(...)
			{
				APP_LOG(common::Applog::LOG_CRITICAL ) << "Fatal Error, Task Can not find！" << header.datatype;
				return;
			}

			command->exeCommandImpl();

			delete[] message.ptrData;
			message.ptrData = NULL;
		}
#if 1	
#ifndef _WIN32	//测试性能
		gettimeofday(&stop,0); 
		timeval_subtract(&diff,&start,&stop); 

		time_t now; //实例化time_t结构 
		struct tm *timenow; //实例化tm结构指针 
		time(&now); 
		//time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now 

		timenow = localtime(&now); 
		//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区) 

		printf("[%s]", asctime(timenow)); 
		printf( "Task [ datatype=%d ]. Time elapsed:%d(us)\n", message.header.datatype, diff.tv_usec );
#endif
#endif
	}

} // namespace WorkPlan

