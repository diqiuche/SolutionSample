#include "TcpServer.h"
#include <numeric>
#include <functional>
namespace itstation {

	TcpServer::TcpServer( int port, ITcpMessageNotify* ptrNotify )
		: m_ptrNotify(ptrNotify)
		, m_listCnnIdPool(MaxConnectionNum)
	{
		boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::v4(),port);
		//m_acceptor = new boost::asio::ip::tcp::acceptor(m_ioservice, endPoint);
		m_acceptor = std::make_shared<boost::asio::ip::tcp::acceptor>(m_ioservice, endPoint);

		m_listCnnIdPool.resize(MaxConnectionNum);
		std::iota(m_listCnnIdPool.begin(), m_listCnnIdPool.end(), 1);
	}
	TcpServer::~TcpServer()
	{
	}

	void TcpServer::OnRun()
	{
		while (IsRuning())
		{
			m_ioservice.run();
		}
	}

	void TcpServer::StartUp()
	{
		session_ptr new_session = __CreateClient();
		//m_acceptor->async_accept(new_session->GetSocket(),
		//	boost::bind(&TcpServer::__handle_accept,this,
		//	boost::asio::placeholders::error, new_session));

		m_acceptor->async_accept(new_session->GetSocket(), [this, new_session](const boost::system::error_code& error)
		{
			if (error)
			{
				cout << error.value() << " " << error.message() << endl;
				new_session->CloseSocket();
				__StopAccept();
			} else
			{
				m_mapClients.insert(std::make_pair(new_session->GetConnId(), new_session));
				cout << "current connect count: " << m_mapClients.size() << endl;

				new_session->StartUp();
				StartUp();
			}
		});

		Start();
	}

	void TcpServer::SendMessages( session_ptr ptrSocket,  const char* stream, TCP_MSG_HEAD &header )
	{
		ptrSocket->SendMessages(stream, header);

		//common::Locker locker(&m_SpinLock);
		//for ( auto it = m_mapClients.begin(); it != m_mapClients.end(); it++ )
		//{
		//	(it->second).get()->SendMessages(stream, header);
		//}
	}

	void TcpServer::__handle_accept(const boost::system::error_code& error, session_ptr session)
	{
		if(!error)
		{
			std::cout << "get a new client!" << std::endl;
			//实现对每个客户端的数据处理
			this->__AddClient(session);
			session->StartUp();

			//在这就应该看出为什么要封session类了吧，每一个session就是一个客户端
			StartUp();
		} else
		{
			session->CloseSocket();
			__StopAccept();
		}
	}

	itstation::session_ptr TcpServer::__CreateClient()
	{
		int connId = m_listCnnIdPool.front();
		m_listCnnIdPool.pop_front();

		session_ptr client = std::make_shared<clientSession>(m_ioservice, m_ptrNotify);
		//client->SetConnId(connId);
		//auto fr = std::bind(&TcpServer::__ClientCallBackError, this, std::placeholders::_1);
		client->SetCallBackError([this](int connid){__ClientCallBackError(connid); });

		return client;
	}

	void TcpServer::__AddClient( session_ptr& client )
	{
		common::Locker locker(&m_SpinLock);
		m_mapClients.insert(std::make_pair(client->GetConnId(), client));
	}

	void TcpServer::__StopAccept()
	{
		boost::system::error_code ec;
		m_acceptor->cancel(ec);
		m_acceptor->close(ec);
		m_ioservice.stop();
	}

	void TcpServer::__ClientCallBackError(int connId)
	{
		common::Locker locker(&m_SpinLock);
		auto it = m_mapClients.find(connId);
		if ( it != m_mapClients.end() )
		{
			m_mapClients.erase(it);
		}

		m_listCnnIdPool.push_back(connId);
	}

}
