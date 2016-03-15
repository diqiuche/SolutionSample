#include "NetworkLibrary.h"
#include "TcpServer.h"
#include "TcpClient.h"

namespace itstation 
{
	ITcpServer * CreateTcpServer(int port, ITcpMessageNotify * ptrNotify )
	{
		return new TcpServer( port, ptrNotify );
	}

	ITcpClient * CreateTcpClient(const char *ip, int port, ITcpMessageNotify * ptrNotify )
	{
		return new TcpClient( ip, port, ptrNotify );
	}

	void DeleteTcpServer( ITcpServer * ptrObj )
	{
		if ( ptrObj )
		{
			delete ptrObj;
			ptrObj = NULL;
		}
	}

	void DeleteTcpClient( ITcpClient * ptrObj )
	{
		if ( ptrObj )
		{
			delete ptrObj;
			ptrObj = NULL;
		}
	}
}