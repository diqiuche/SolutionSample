#ifndef NETWORK_LIBRARY_H_
#define NETWORK_LIBRARY_H_

#include "Global.h"
#include "TcpMsgHead.h"
#include "TcpSocket.h"
namespace itstation 
{

	class COMMON_API ITcpServer
	{ 
	public:

		virtual ~ITcpServer(){};

		virtual void StartUp( void ) = 0;
		virtual void SendMessages(session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header ) = 0;
	};

	class COMMON_API ITcpClient
	{ 
	public:

		virtual ~ITcpClient(){};

		virtual void StartUp( void ) = 0;
		virtual void SendMessages(session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header ) = 0;
	};

	extern "C"
	{
		COMMON_API ITcpServer * CreateTcpServer(int port, ITcpMessageNotify * ptrNotify );

		COMMON_API ITcpClient * CreateTcpClient(const char *ip, int port, ITcpMessageNotify * ptrNotify );

		COMMON_API void DeleteTcpServer( ITcpServer * ptrObj );

		COMMON_API void DeleteTcpClient( ITcpClient * ptrObj );
	}

}
#endif
