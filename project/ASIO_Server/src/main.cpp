#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
using boost::asio::ip::tcp;
#define max_len 1024
class clientSession
	:public boost::enable_shared_from_this<clientSession>
{
public:
	clientSession(boost::asio::io_service& ioservice)
		:m_socket(ioservice)
	{
		memset(data_,'\0',sizeof(data_));
	}
	~clientSession()
	{}
	tcp::socket& socket()
	{
		return m_socket;
	}
	void start()
	{
		boost::asio::async_write(m_socket,
			boost::asio::buffer("link successed!"),
			boost::bind(&clientSession::handle_write,shared_from_this(),
			boost::asio::placeholders::error));

		/*async_read跟客户端一样，还是不能进入handle_read函数,如果你能找到问题所在，请告诉我，谢谢*/

		// --已经解决，boost::asio::async_read(...)读取的字节长度不能大于数据流的长度，否则就会进入
		// ioservice.run()线程等待，read后面的就不执行了。
		//boost::asio::async_read(m_socket,boost::asio::buffer(data_,max_len),

		//         boost::bind(&clientSession::handle_read,shared_from_this(),

		//         boost::asio::placeholders::error));
		//max_len可以换成较小的数字，就会发现async_read_some可以连续接收未收完的数据

		m_socket.async_read_some(boost::asio::buffer(data_,max_len),
			boost::bind(&clientSession::handle_read,shared_from_this(),
			boost::asio::placeholders::error));
	}
private:
	void handle_write(const boost::system::error_code& error)
	{

		if(error)
		{
			m_socket.close();
		}

	}
	void handle_read(const boost::system::error_code& error)
	{

		if(!error)
		{
			std::cout << data_ << std::endl;
			//boost::asio::async_read(m_socket,boost::asio::buffer(data_,max_len),

			//     boost::bind(&clientSession::handle_read,shared_from_this(),

			//     boost::asio::placeholders::error));

			m_socket.async_read_some(boost::asio::buffer(data_,max_len),
				boost::bind(&clientSession::handle_read,shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			m_socket.close();
		}

	}
private:
	tcp::socket m_socket;
	char data_[max_len];
};

class serverApp
{
	typedef boost::shared_ptr<clientSession> session_ptr;
public:
	serverApp(boost::asio::io_service& ioservice,tcp::endpoint& endpoint)
		:m_ioservice(ioservice),
		acceptor_(ioservice,endpoint)
	{
		session_ptr new_session(new clientSession(ioservice));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&serverApp::handle_accept,this,boost::asio::placeholders::error,
			new_session));
	}
	~serverApp()
	{
	}
private:
	void handle_accept(const boost::system::error_code& error,session_ptr& session)
	{
		if(!error)
		{
			std::cout << "get a new client!" << std::endl;
			//实现对每个客户端的数据处理

			session->start();
			//在这就应该看出为什么要封session类了吧，每一个session就是一个客户端

			session_ptr new_session(new clientSession(m_ioservice));
			acceptor_.async_accept(new_session->socket(),
				boost::bind(&serverApp::handle_accept,this,boost::asio::placeholders::error,
				new_session));
		}
	}
private:
	boost::asio::io_service& m_ioservice;
	tcp::acceptor acceptor_;
};

int main(int argc , char* argv[])
{
	boost::asio::io_service myIoService;
	short port = 8100/*argv[1]*/;
	//我们用的是inet4

	tcp::endpoint endPoint(tcp::v4(),port);
	//终端（可以看作sockaddr_in）完成后，就要accept了

	serverApp sa(myIoService,endPoint);
	//数据收发逻辑

	myIoService.run();
	return 0;
}