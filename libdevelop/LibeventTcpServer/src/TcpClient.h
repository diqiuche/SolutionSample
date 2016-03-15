#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

#include "TcpSocket.h"
#include "Thread.h"

namespace itstation 
{

	class TcpClient : 
		public ITcpClient, public common::Thread, private ISocketDisconNotify
	{
	public:
		TcpClient(const char *ip, int port, ITcpMessageNotify* ptrNotify = NULL);
		~TcpClient(void);

		bool StartUp( void );
		void ShutDown();

		bool SendMessages( const char* stream, TCP_MSG_HEAD &header );

	private:
		bool ReConnect();
		virtual void OnDisconNotify(TcpSocket *tcp_sock);

		virtual void OnRun();

	private:
		std::string m_ip;
		int m_port;
		ITcpMessageNotify* m_ptrNotify;

		struct event_base *m_base;
		TcpSocket* m_tcp_socket;

	};

}

#endif

