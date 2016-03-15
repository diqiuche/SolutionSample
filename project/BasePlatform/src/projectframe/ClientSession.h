////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Session class which processes request socket in a independent thread.
/// implement the state machine ClientSession.
///
/// @version $Id: ClientSession.h 1068 2012-02-14 07:46:19Z liuzq $
////////////////////////////////////////////////////////////////////////////////
#ifndef WORKPLAN_CLIENTSESSION_H_
#define WORKPLAN_CLIENTSESSION_H_
#include "TaskData.h"
#include "NetworkLibrary.h"

namespace itstation 
{
	class ClientSession 
		: public ITcpMessageNotify
	{
	public:

		explicit ClientSession( int nType=0 );
		virtual ~ClientSession();
	protected:

		virtual void OnMessageStream( const char* stream, TCP_MSG_HEAD &header );

	private:
		int m_nType;
	};
} // namespace WorkPlan
#endif
