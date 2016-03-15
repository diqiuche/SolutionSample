#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#endif
#include <boost/asio/buffer.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include "TcpMsgHead.h"
//#include "NetworkLibrary.h"

namespace itstation {

	class clientSession;
	typedef std::shared_ptr<clientSession> session_ptr;

	class ITcpMessageNotify 
	{
	public:

		virtual void OnMessageStream( const char* stream, TCP_MSG_HEAD &header ) = 0;

		void SetSessionPtr(session_ptr ptrSocket)
		{
			m_ptrSocket = ptrSocket;
		}

		session_ptr GetSessionPtr()
		{
			return m_ptrSocket;
		}

	private:
		session_ptr m_ptrSocket; 
	};

	class clientSession
		:public std::enable_shared_from_this<clientSession>
	{

	public:
		clientSession(boost::asio::io_service& ioservice, ITcpMessageNotify* ptrNotify);
		virtual ~clientSession();

		std::shared_ptr<clientSession> GetSelf()
		{
			return shared_from_this();
		}

		void StartUp();

		void SendMessages( const char* stream, TCP_MSG_HEAD &header );

		boost::asio::ip::tcp::socket& GetSocket();
		void CloseSocket();

		void SetConnId(int connId);
		int GetConnId() const;

		void SetCallBackError(const std::function<void(int)>& f);
	private:

		void __Handle_Read(const boost::system::error_code& error);
		void __Handle_Callback(std::shared_ptr<char>& data, const boost::system::error_code& error);
		void __handle_write(std::shared_ptr<char>& data, const boost::system::error_code& error);

		void __HandleError(const boost::system::error_code& ec);
	private:
		boost::asio::ip::tcp::socket m_socket;
		TCP_MSG_HEAD m_MessageHead;
		int m_connId;
		std::function<void(int)> m_callbackError;
		ITcpMessageNotify* m_ptrNotify;
	};
}

#endif
