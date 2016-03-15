#include "TcpSocket.h"
#include <sstream>
#include <Thread.h>
#include <string.h>

namespace itstation {

	bool TcpSocket::m_winstart_status = false;

	////////////////////////////////////////////////////////////////////////////////////

	TcpSocket::TcpSocket(struct event_base *p_base, ITcpMessageNotify* ptrNotify, ISocketDisconNotify* ptrDisconNotify)
		: m_p_base(p_base), m_ptrNotify(ptrNotify), m_ptrDisconNotify(ptrDisconNotify), m_bev(NULL), invalid_(true)
	{	
	}

	TcpSocket::~TcpSocket(void)
	{
		if (m_bev) { bufferevent_free(m_bev); }
	}

	bool TcpSocket::WinStartUp(std::string& err)
	{
#if defined(WIN32)  
		if (m_winstart_status) { return true; }

		WSADATA  Ws;  
		//Init Windows Socket  
		if (WSAStartup(MAKEWORD(2,2), &Ws) == 0)  
		{  
			m_winstart_status = true;
			return true;
		}
		err = "WSAStartup error";
		return false;
#else
		return true;
#endif 
	}

	void TcpSocket::WinClean()
	{
#if defined(WIN32)  
		if (!m_winstart_status) { return; }
		WSACleanup();  
		m_winstart_status = false;
#endif  
	}

	bool TcpSocket::GetAddressFrom(sockaddr_in *addr, const char *ip, int port, std::string& err)
	{
		if (ip == NULL || port <= 0)
		{
			err = "invalid address/port";
			return false;
		}
		memset(addr, 0, sizeof(sockaddr_in));  
		addr->sin_family = AF_INET;            /*地址类型为AF_INET*/  
		addr->sin_addr.s_addr = inet_addr(ip); 
		addr->sin_port = htons(port);   /*端口号*/  
		return true;
	}

	HSocket TcpSocket::SocketOpen(std::string& err)
	{
		int protocol = 0;  
		HSocket hs;  
#if defined(WIN32)  
		protocol=IPPROTO_TCP;  
#endif  
		hs = socket(AF_INET, SOCK_STREAM, protocol);  
		if (hs == NAN_SOCKET) 
		{
#if defined(WIN32) 
			int err_code = WSAGetLastError();
			switch(err_code)
			{			
			case WSAEAFNOSUPPORT:
				{
					err = "The specified address is not supported.";
					break;
				}			
			case WSAEINVAL:
				{
					err = "invalid argument was supplied.";
					break;
				}	
			default:
				{
					err = "invalid socket";
					break;
				}
			}
#else
			err = "invalid socket";
#endif  
		}
		//if (evutil_make_socket_nonblocking(hs) < 0) 
		//{
		//	evutil_closesocket(hs);
		//	err = "invalid socket";
		//}
		return hs;  
	}


	void TcpSocket::SocketClose(HSocket &handle)  
	{  
#if defined(WIN32)  
		closesocket(handle);  
#else
		close(handle);  
#endif  
	} 

	bool TcpSocket::ConnectServer(const char *ip, int port, std::string& err)
	{
		if (!WinStartUp(err)) { return false; }

		if (m_p_base == NULL)
		{
			err = "the libevent is null";
			return false;
		}

		struct sockaddr_in sin;
		if (!GetAddressFrom(&sin, ip, port, err)) { return false; }

		//当删除m_bev时会自动关闭套接字，所以不需要在此关闭
		//if (m_socket == INVALID_SOCKET) { 
		//	SocketClose(m_socket);
		//	m_socket = NAN_SOCKET; 
		//}

		m_socket = SocketOpen(err);
		if (m_socket == INVALID_SOCKET) { return false; }

		int ret = connect(m_socket,(struct sockaddr *)&sin,sizeof(sin));
		if (ret < 0) {
#if defined(WIN32)  
			int err_code = WSAGetLastError();
			switch(err_code)
			{
			case WSAEAFNOSUPPORT:
				{
					err = "Addresses in the specified family cannot be used with this socket.";
					break;
				}
			case WSAEFAULT:
				{
					err = "Addresses in the specified family cannot be used with this socket.";
					break;
				}
			case WSAEINVAL:
				{
					err = "The parameter is a listening socket.";
					break;
				}
			default:
				{
					err = "connect error";
					break;
				}
			}
#else
			err = "connect error";
#endif
			SocketClose(m_socket);
			m_socket = NAN_SOCKET; 
			return false;
		}

		if (!SetBufferevent(m_socket, err))
		{
			return false;
		}

		return true;
	}

	bool TcpSocket::SetBufSize(HSocket socket, bool is_recv, int size, std::string& err)
	{
		if (socket == INVALID_SOCKET) { 
			err = "Invalid socket";
			return false; 
		}
		m_socket = socket;

		int opt_name = SO_RCVBUF;
		if (!is_recv) {
			opt_name = SO_SNDBUF;
		}

		int ret = 0;
#if defined(WIN32)  
		ret = setsockopt(m_socket, SOL_SOCKET, opt_name, (char*) &size, sizeof(size));
		if (ret == SOCKET_ERROR)
		{      
			int err_code = WSAGetLastError();
			std::stringstream ss;
			ss << "err code: " << err_code;
			err = ss.str();
			return false;
		}
#else
		ret = setsockopt(m_socket, SOL_SOCKET, opt_name, (const void*) &size, (socklen_t)sizeof(size));
		if (-1 == ret)
		{
			err = "error on setsockopt";
			return false;
		}
#endif

		int new_size = 0;
		int size_len = sizeof(new_size);
#if defined(WIN32)
		ret = getsockopt(m_socket, SOL_SOCKET, opt_name, (char*) &new_size, &size_len);
		if (ret == SOCKET_ERROR) { 
			err = "error on getsockopt";
			return false;
		}
#else
		ret = getsockopt(m_socket, SOL_SOCKET, opt_name, (void*) &new_size, (socklen_t*)&size_len);
		if (ret == -1) { 
			err = "error on getsockopt";
			return false;
		}
#endif

		if (new_size != size)
		{
			err = "SetBufSize error";
			return false;
		}

		return true;
	}

	bool TcpSocket::SetBufferevent(HSocket socket, std::string& err)
	{
		if (socket == INVALID_SOCKET) { 
			err = "Invalid socket";
			return false; 
		}

		if (m_p_base == NULL)
		{
			err = "event_base is null";
			return false;
		}

		if (NULL != m_bev) 
		{ 
			bufferevent_free(m_bev); 
			m_bev = NULL; 
		}

		m_bev = bufferevent_socket_new(m_p_base, socket, BEV_OPT_CLOSE_ON_FREE);
		if (!m_bev) {
			err = "bufferevent_socket_new error\n";
			return false;
		}

		bufferevent_setcb(m_bev, ReadCb, NULL, EventCb, (void*)this);
		if (bufferevent_enable(m_bev, EV_READ|EV_WRITE) != 0 )
		{
			err = "bufferevent_enable erro";
			bufferevent_free(m_bev);
			m_bev = NULL;
			return false;
		}

		return true;
	}

	bool TcpSocket::SendMessages( const char* stream, TCP_MSG_HEAD &header )
	{
		string err;
		if (NULL == m_bev)
		{
			err = "the socket is not open";
			return false;
		}

		if (NULL == stream || header.datasize <= 0)
		{
			err = "invalid argument";
			return false;
		}

		int dateSize = sizeof(TCP_MSG_HEAD) + header.datasize + 2;

		char *dataBuf = new char[dateSize];
		if ( dataBuf == NULL )
		{
			cout << "new memory error" << endl;
			return false;
		}
		memset(dataBuf, 0, dateSize);

		memcpy(dataBuf, &header, sizeof(TCP_MSG_HEAD));
		memcpy(dataBuf+sizeof(TCP_MSG_HEAD), stream, header.datasize);
		dataBuf[dateSize - 2] = '\r';
		dataBuf[dateSize - 1] = '\n';
#if 1
		if (bufferevent_write(m_bev, dataBuf, dateSize) != 0)
		{
			err = "bufferevent write error";
			delete [] dataBuf;
			dataBuf = NULL;
			return false;
		}
#endif
#if 0
		while(send(m_socket, dataBuf, dateSize, 0) <= 0)
		{
#if defined(WIN32) 
			int err_code = WSAGetLastError();
			printf("err code: %d\n", err_code);
			if (err_code == 10035)
			{
				//10-100
				//int num = rand() % 10;
				//num = num * 10 + 10;
				q_sleep(100); //毫秒
			}
			else
			{
				err = "TcpSocket::SendMessages error";
				delete dataBuf;
				dataBuf = NULL;
				return false;
			}
#else
			err = "TcpSocket::SendMessages error";
			delete dataBuf;
			dataBuf = NULL;
			return false;
#endif
		}
#endif
		delete dataBuf;
		dataBuf = NULL;

		return true;
	}

	bool TcpSocket::GetInvalid() 
	{
		common::Locker locker(&inva_mtx_);
		return invalid_;
	}

	void TcpSocket::SetInvalid(bool arg) 
	{
		common::Locker locker(&inva_mtx_);
		invalid_ = arg;
	}

	HSocket TcpSocket::GetSocket()
	{
		return m_socket;
	}

	void TcpSocket::ReadBuf( const char* stream, TCP_MSG_HEAD &header )
	{
		if (NULL != m_ptrNotify && GetInvalid()) 
		{ 
			m_ptrNotify->OnMessageStream( stream, header); 
		}
	}

	void TcpSocket::ReadCb(struct bufferevent *bev, void *user_data)
	{
		TcpSocket *this_ptr = (TcpSocket*)user_data;

		struct evbuffer *input, *output;  
		char *request_line;  
		size_t len, out_len;
		input = bufferevent_get_input(bev);//其实就是取出bufferevent中的input  
		output = bufferevent_get_output(bev);//其实就是取出bufferevent中的output

		while (len = evbuffer_get_length(input))
		{
			request_line = evbuffer_readln(input, &out_len, EVBUFFER_EOL_CRLF_STRICT);
			if (request_line == NULL)
			{
				//cout << "readcd erro" << endl;
				free(request_line);
				break;
			}

			TCP_MSG_HEAD header;
			memcpy(&header, request_line, sizeof(TCP_MSG_HEAD));
			const char *ptrData = request_line + sizeof(TCP_MSG_HEAD);

			this_ptr->ReadBuf( ptrData, header);

			free(request_line);
			request_line = NULL;
		}
	}

	void TcpSocket::EventCb(struct bufferevent *bev, short events, void *user_data)
	{
	/*
        回调和水位
        每个bufferevent有两个数据相关的回调：一个读取回调和一个写入回调。默认情况下，从底层传输端口读取了任意量的数据之后会调用读取回调；输出缓冲区中足够量的数据被清空到底层传输端口后写入回调会被调用。通过调整bufferevent的读取和写入“水位（watermarks）”可以覆盖这些函数的默认行为。
        每个bufferevent有四个水位：
        l 读取低水位：读取操作使得输入缓冲区的数据量在此级别或者更高时，读取回调将被调用。默认值为0，所以每个读取操作都会导致读取回调被调用。
        l 读取高水位：输入缓冲区中的数据量达到此级别后，bufferevent将停止读取，直到输入缓冲区中足够量的数据被抽取，使得数据量低于此级别。默认值是无限，所以永远不会因为输入缓冲区的大小而停止读取。
        l 写入低水位：写入操作使得输出缓冲区的数据量达到或者低于此级别时，写入回调将被调用。默认值是0，所以只有输出缓冲区空的时候才会调用写入回调。
        l 写入高水位：bufferevent没有直接使用这个水位。它在bufferevent用作另外一个bufferevent的底层传输端口时有特殊意义。请看后面关于过滤型bufferevent的介绍。
        bufferevent也有“错误”或者“事件”回调，用于向应用通知非面向数据的事件，如连接已经关闭或者发生错误。定义了下列事件标志：
        l BEV_EVENT_READING：读取操作时发生某事件，具体是哪种事件请看其他标志。
        l BEV_EVENT_WRITING：写入操作时发生某事件，具体是哪种事件请看其他标志。
        l BEV_EVENT_ERROR：操作时发生错误。关于错误的更多信息，请调用EVUTIL_SOCKET_ERROR()。
        l BEV_EVENT_TIMEOUT：发生超时。
        l BEV_EVENT_EOF：遇到文件结束指示。
        l BEV_EVENT_CONNECTED：请求的连接过程已经完成。
        
    */
		TcpSocket *this_ptr = (TcpSocket*)user_data;
		this_ptr->SetInvalid(false);

		printf("Connection closed.\n");

		if (NULL != this_ptr->m_ptrDisconNotify )
		{
			this_ptr->m_ptrDisconNotify->OnDisconNotify(this_ptr);
		}

	}

}
