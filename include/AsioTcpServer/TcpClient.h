#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_
#include "Thread.h"
#include "TcpSocket.h"
#include <boost/thread/thread.hpp>  
#include "NetworkLibrary.h"
namespace itstation 
{
	class TcpClient 
		: public common::Thread, public ITcpClient
	{
	public:
		TcpClient(const char *ip, int port, ITcpMessageNotify* ptrNotify = NULL);
		virtual ~TcpClient();

		void StartUp( void );
		void SendMessages( session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header );

		virtual void OnRun();
	private:
		void __handle_connect();

		void __HandleConnectError(const boost::system::error_code& error);
		void __HandleRWError(int connid);
		void __CheckConnect();
		bool __IsConnected() const; 
		

	private:
		std::string m_ip;
		int m_port;
		boost::asio::io_service m_ioservice;
		session_ptr m_ptrClient; 
		bool m_isConnected;
		std::shared_ptr<boost::thread> m_chkThread; //专门检测重连的线程

		ITcpMessageNotify* m_ptrNotify;
	};

}

#endif

