#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include "TcpSocket.h"
#include "Thread.h"
#include <map>

namespace itstation 
{

	class TcpServer : 
		public ITcpServer, public common::Thread, private ISocketDisconNotify
	{
	public:
		TcpServer(int port, ITcpMessageNotify* ptrNotify);
		~TcpServer(void);

		bool StartUp( void );
		bool SendMessages( const char* stream, TCP_MSG_HEAD &header );

	private:
		static void ListenerCb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int, void *);

		void AddSocket(TcpSocket* sock);
		void DleSocket();

		virtual void OnDisconNotify(TcpSocket *tcp_sock);

		virtual void OnRun();

	private:
		struct sockaddr_in m_sin;

		ITcpMessageNotify* m_ptrNotify;
		struct event_base *m_base;
		struct evconnlistener *m_listener;

		common::SpinLock m_SpinLock;
		std::map< HSocket, TcpSocket* > m_mapclients;
	};

}


#endif


