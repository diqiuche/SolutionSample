#ifndef TCP_MSG_HEAD_H_
#define TCP_MSG_HEAD_H_

typedef struct TcpMsgHead
{
	int datatype;
	int datasize;

	TcpMsgHead()
		:datatype(0),datasize(0) 
	{
	}
}TCP_MSG_HEAD;

#endif
