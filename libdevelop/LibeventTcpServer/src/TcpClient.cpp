#include "TcpClient.h"
#include "include_common.h"

namespace itstation {

	TcpClient::TcpClient(const char *ip, int port, ITcpMessageNotify* ptrNotify)
		: m_ip(ip), m_port(port), m_ptrNotify(ptrNotify), m_base(NULL), m_tcp_socket(NULL)
	{

	}


	TcpClient::~TcpClient(void)
	{
		if (NULL != m_base) { event_base_free(m_base); }
		if (NULL != m_tcp_socket) { delete m_tcp_socket; }
	}

	bool TcpClient::StartUp( void )
	{
		string err;

		if (!TcpSocket::WinStartUp(err)) { return false; }

		m_base = event_base_new();
		if (!m_base) {
			event_base_free(m_base);
			m_base = NULL;
			return false;
		}

		m_tcp_socket = new TcpSocket(m_base, m_ptrNotify, this);
		if (!m_tcp_socket->ConnectServer(m_ip.c_str(), m_port, err)) { 
			event_base_free(m_base);
			m_base = NULL;
			delete m_tcp_socket;
			m_tcp_socket = NULL;
			return false; 
		}

		Start();

		return true;
	}

	void TcpClient::ShutDown()
	{
		if (NULL != m_tcp_socket)
		{
			delete m_tcp_socket;
			m_tcp_socket = NULL;
		}

		if (NULL != m_base) { 
			event_base_loopbreak(m_base); 
			event_base_free(m_base);
			m_base = NULL;
		}

		Stop();
	}

	bool TcpClient::SendMessages( const char* stream, TCP_MSG_HEAD &header )
	{
		return m_tcp_socket->SendMessages(stream, header);
	}

	void TcpClient::OnDisconNotify(TcpSocket *tcp_sock)
	{
		printf("Reconnect\n");

		while (1)
		{
			if (ReConnect())
			{
				break;
			}
			q_sleep(3000);
		}

		printf("connect succed\n");
	}

	bool TcpClient::ReConnect()
	{
		std::string err;
		if (!m_tcp_socket->ConnectServer(m_ip.c_str(), m_port, err))
		{
			return false;
		}
		m_tcp_socket->SetInvalid(true);

		return true;
	}

	void TcpClient::OnRun()
	{
		while (IsRuning())
		{
			if (event_base_dispatch(m_base) < 0)
			{
				fprintf(stderr, "Dispatch event error\n");
			}
			q_sleep(3000);
		}
	}

}
