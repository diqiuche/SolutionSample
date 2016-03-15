#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include "Thread.h"
#include "TcpSocket.h"
#include <unordered_map>
#include <list>
#include "NetworkLibrary.h"
namespace itstation 
{
	const int MaxConnectionNum = 65536;

	class TcpServer
		: public common::Thread, public ITcpServer
	{
	public:
		TcpServer( int port, ITcpMessageNotify* ptrNotify );
		virtual ~TcpServer();

		void StartUp();
		void SendMessages( session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header );

		virtual void OnRun();
		
	private:
		void __handle_accept(const boost::system::error_code& error, session_ptr session);
		session_ptr __CreateClient();
		void __AddClient(session_ptr& client);
		void __StopAccept();
		void __ClientCallBackError(int connId);
	private:
		boost::asio::io_service m_ioservice;
		std::shared_ptr<boost::asio::ip::tcp::acceptor> m_acceptor;
		//boost::asio::ip::tcp::acceptor* m_acceptor;

		common::SpinLock m_SpinLock;
		std::unordered_map< int, session_ptr > m_mapClients;
		std::list<int> m_listCnnIdPool;
		ITcpMessageNotify* m_ptrNotify;
	};

}
#endif


