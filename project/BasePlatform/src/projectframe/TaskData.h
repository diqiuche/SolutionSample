//////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Task.h
/// Object of Task
///
/// @version: 1.0.0
/// @author: ZZY
/// @email: zheng.zengyuan@fuanda.co
/// @date: 2012/01/30
/// @version $Id: Task.h 1076 2012-02-15 05:55:17Z liuzq $ 
//////////////////////////////////////////////////////////////////////////////////

#ifndef WORKPLAN_TASK_DATA_H_
#define WORKPLAN_TASK_DATA_H_
#include <string>
#include "TcpMsgHead.h"

typedef struct ASSET_MESSAGE_STRUCT
{
	TCP_MSG_HEAD header;
	void *ptrData;

}ASSET_MESSAGE_STRUCT;


// Data read object.
class TaskData
{
public:
    // Default constructor
    TaskData() { }
	virtual ~TaskData() { }

	void set_task_message( ASSET_MESSAGE_STRUCT &msg );
	void get_task_message( ASSET_MESSAGE_STRUCT &msg );
private:
	ASSET_MESSAGE_STRUCT m_Data;
};

inline void TaskData::set_task_message( ASSET_MESSAGE_STRUCT &msg )	
{
	m_Data.header	= msg.header;
	m_Data.ptrData	= msg.ptrData;
}

inline void TaskData::get_task_message( ASSET_MESSAGE_STRUCT &msg )	
{
	msg.header	= m_Data.header;
	msg.ptrData = m_Data.ptrData;
}

#endif
