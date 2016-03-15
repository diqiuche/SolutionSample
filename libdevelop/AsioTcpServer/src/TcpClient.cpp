#include "TcpClient.h"

namespace itstation {

	TcpClient::TcpClient(const char *ip, int port, ITcpMessageNotify* ptrNotify)
		: m_ip(ip)
		, m_port(port)
		, m_isConnected(false)
		, m_chkThread(nullptr)
		, m_ptrNotify(ptrNotify)
	{
	}
	TcpClient::~TcpClient()
	{}

	void TcpClient::StartUp()
	{
		m_ptrClient = std::make_shared<clientSession>(m_ioservice, m_ptrNotify);
		m_ptrClient->SetCallBackError([this](int connid){__HandleRWError(connid); });
		__handle_connect();

		Start();
	}

	void TcpClient::SendMessages( session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header )
	{
		while ( !m_isConnected )
		{
			q_sleep(1000);
		}

		m_ptrClient->SendMessages(stream, header);
	}

	void TcpClient::OnRun()
	{
		while (IsRuning())
		{
			m_ioservice.run();

			q_sleep(3000);
		}
	}

	void TcpClient::__handle_connect()
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(m_ip), m_port);
		//连接服务端 connect
		m_ptrClient->GetSocket().async_connect(endpoint,
			[this](const boost::system::error_code& error)
		{
			//cout << __IsConnected() << endl;
			if ( error )
			{
				if ( !__IsConnected() )
				{
					__HandleConnectError(error);
				}
				
				return;
			}
			cout << "connect ok" << endl;
			m_isConnected = true;
			m_ptrClient->StartUp(); //连接成功后发起一个异步读的操作
		});
	}

	void TcpClient::__HandleConnectError( const boost::system::error_code& error )
	{
		//m_isConnected = false;
		if ( !__IsConnected() )
		{
			cout << error.message() << endl;
			//m_ptrClient->CloseSocket();
			//__CheckConnect();
		}

	}

	void TcpClient::__HandleRWError( int connid )
	{
		m_isConnected = false;
		__CheckConnect();
	}

	void TcpClient::__CheckConnect()
	{
		if (m_chkThread != nullptr)
			return;

		m_chkThread = std::make_shared<boost::thread>([this]
		{
			while (1)
			{
				if ( !__IsConnected() )
				{
					__handle_connect();
				}
				
				boost::this_thread::sleep(boost::posix_time::seconds(1));
			}
		});
	}

	bool TcpClient::__IsConnected() const
	{
		return m_isConnected;
	}


}
