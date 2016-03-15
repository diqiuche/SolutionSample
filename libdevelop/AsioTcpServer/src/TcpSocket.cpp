#include "TcpSocket.h"
#include <sstream>
#include <Thread.h>
#include <string.h>

namespace itstation {

	clientSession::clientSession(boost::asio::io_service& ioservice, ITcpMessageNotify* ptrNotify)
		: m_socket(ioservice), m_ptrNotify(ptrNotify)
	{
	}
	clientSession::~clientSession()
	{

	}

	void clientSession::StartUp()
	{
		//boost::asio::async_read(...)读取的字节长度不能大于数据流的长度，否则就会进入
		//ioservice.run()线程等待，read后面的就不执行了。
		boost::asio::async_read(m_socket,boost::asio::buffer(&m_MessageHead,sizeof(TCP_MSG_HEAD)),
			boost::bind(&clientSession::__Handle_Read,shared_from_this(),
			boost::asio::placeholders::error));

		//三种情况下会返回：1.缓冲区满；2.transfer_at_least为真(收到特定数量字节即返回)；3.有错误发生
		//boost::asio::async_read(m_socket, boost::asio::buffer(&m_MessageHead,sizeof(TCP_MSG_HEAD)),
		//	[this](const boost::system::error_code& ec, size_t size)
		//{
		//	if( ec != nullptr )
		//	{
		//		__HandleError(ec);
		//		return;
		//	}
		//	else
		//	{
		//		std::cout << m_MessageHead.datatype << " " << m_MessageHead.datasize  << std::endl;

		//		char *messageBody = new char(m_MessageHead.datasize);

		//		boost::asio::async_read(m_socket,boost::asio::buffer(messageBody, m_MessageHead.datasize),
		//			boost::bind(&clientSession::Handle_Callback,shared_from_this(),
		//			boost::asio::placeholders::error));

		//		TCP_MSG_HEAD header;
		//		memcpy(&header, &m_MessageHead, sizeof(TCP_MSG_HEAD));
		//		const char *ptrData = messageBody;

		//		m_ptrNotify->OnMessageStream( ptrData, header);

		//		delete[] messageBody;
		//		messageBody = NULL;
		//	}

		//	StartUp();
		//});

		//max_len可以换成较小的数字，就会发现async_read_some可以连续接收未收完的数据
		//m_socket->async_read_some(boost::asio::buffer(&m_MessageHead,sizeof(TCP_MSG_HEAD)),
		//	boost::bind(&clientSession::handle_read,shared_from_this(), boost::asio::placeholders::error));
	}

	void clientSession::SendMessages( const char* stream, TCP_MSG_HEAD &header )
	{
		if ( NULL == stream || header.datasize <= 0 )
		{
			cout << "data error" << endl;
			return;
		}

		int dateSize = sizeof(TCP_MSG_HEAD) + header.datasize;

		std::shared_ptr<char> dataBuf(new char[dateSize], std::default_delete<char[]>());
		memcpy(&(*dataBuf), &header, sizeof(TCP_MSG_HEAD));
		memcpy(&(*dataBuf)+sizeof(TCP_MSG_HEAD), stream, header.datasize);

		//std::shared_ptr<std::string> data(new string(dataBuf));

		//boost::asio::async_write(m_socket,
		//	boost::asio::buffer(dataBuf.get(), dateSize),
		//	boost::bind(&clientSession::__handle_write,shared_from_this(), dataBuf, boost::asio::placeholders::error));

		//同步发送
		boost::system::error_code ec;
		write(m_socket, boost::asio::buffer(dataBuf.get(), dateSize), ec);
		if (ec != nullptr)
		{
			__HandleError(ec);
			return;
		}
	}

	boost::asio::ip::tcp::socket& clientSession::GetSocket()
	{
		return m_socket;
	}

	void clientSession::CloseSocket()
	{
		boost::system::error_code ec;
		m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);
		m_socket.close(ec);
	}

	void clientSession::SetConnId(int connId)
	{
		m_connId = connId;
	}

	int clientSession::GetConnId() const
	{
		return m_connId;
	}

	void clientSession::SetCallBackError(const std::function<void(int)>& f)
	{
		m_callbackError = f;
	}

	void clientSession::__Handle_Read(const boost::system::error_code& error)
	{
		if(!error)
		{
			//std::cout << m_MessageHead.datatype << " " << m_MessageHead.datasize  << std::endl;

			std::shared_ptr<char> messageBody(new char[m_MessageHead.datasize], std::default_delete<char[]>());

			boost::asio::async_read(m_socket,boost::asio::buffer(messageBody.get(), m_MessageHead.datasize),
				boost::bind(&clientSession::__Handle_Callback,shared_from_this(), messageBody, 
				boost::asio::placeholders::error));
		}
		else
		{
			__HandleError(error);
			return;
		}
	}

	void clientSession::__Handle_Callback(std::shared_ptr<char>& data, const boost::system::error_code& error)
	{

		if(!error)
		{
			m_ptrNotify->SetSessionPtr(GetSelf());

			TCP_MSG_HEAD header;
			memcpy(&header, &m_MessageHead, sizeof(TCP_MSG_HEAD));
			const char *ptrData = data.get();

			m_ptrNotify->OnMessageStream( ptrData, header);

			StartUp();
		}
		else
		{
			__HandleError(error);
			return;
		}

	}

	void clientSession::__handle_write(std::shared_ptr<char>& data, const boost::system::error_code& error)
	{
		if(error)
		{
			__HandleError(error);
			return;
		}

	}

	void clientSession::__HandleError( const boost::system::error_code& ec )
	{
		CloseSocket(); 
		cout << ec.message() << endl;
		if (m_callbackError)
			m_callbackError(m_connId);
	}



}
