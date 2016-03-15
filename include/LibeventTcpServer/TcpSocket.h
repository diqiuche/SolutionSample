#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#ifdef WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
typedef SOCKET HSocket; 
#define NAN_SOCKET INVALID_SOCKET

#else
#include <unistd.h>
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <sys/types.h>  
typedef int HSocket;  
#define SOCKET_ERROR  (-1)  
#define INVALID_SOCKET  0 
#include <arpa/inet.h>
#define NAN_SOCKET INVALID_SOCKET
#endif

#include <stdlib.h>
#include <ctype.h>
#include <string>
#include "SpinLock.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>

#include "NetworkLibrary.h"

namespace itstation {

	class TcpSocket;

	class ISocketDisconNotify {
	public:
		virtual void OnDisconNotify(TcpSocket *tcp_sock) = 0;
	};

	class TcpSocket
	{
	public:
		TcpSocket(struct event_base *p_base, ITcpMessageNotify* ptrNotify, ISocketDisconNotify* ptrDisconNotify);
		~TcpSocket(void);

		static bool WinStartUp(std::string& err);
		static void WinClean();

		bool ConnectServer(const char *ip, int port, std::string& err);

		bool SetBufSize(HSocket socket, bool is_recv, int size, std::string& err);
		bool SetBufferevent(HSocket socket, std::string& err);

		bool SendMessages( const char* stream, TCP_MSG_HEAD &header );

		HSocket GetSocket();
		bool GetInvalid();
		void SetInvalid(bool arg);

	private:
		bool GetAddressFrom(sockaddr_in *addr, const char *ip, int port, std::string& err);
		HSocket SocketOpen(std::string& err);
		void SocketClose(HSocket &handle);

		void ReadBuf( const char* stream, TCP_MSG_HEAD &header );

		static void ReadCb(struct bufferevent *, void *);
		static void EventCb(struct bufferevent *, short, void *);

	private:
		struct event_base* m_p_base;	//外部对象，不需要再本类中区销毁资源
		ITcpMessageNotify* m_ptrNotify;
		ISocketDisconNotify* m_ptrDisconNotify;

		HSocket m_socket;
		struct bufferevent *m_bev;
		bool invalid_;
		common::SpinLock inva_mtx_;

		static bool m_winstart_status;
	};

}

#endif
