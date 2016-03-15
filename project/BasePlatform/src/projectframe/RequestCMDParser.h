////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Request XML parser, to create a command including task list in request.
///
/// @version $Id$
////////////////////////////////////////////////////////////////////////////////
#ifndef WORKPLAN_REQUESTCMDPARSER_H_
#define WORKPLAN_REQUESTCMDPARSER_H_

#include "RequestCommand.h"
namespace itstation 
{
	class RequestCMDParser 
	{
	public:
		// Create Request command parser.
		RequestCMDParser();
		virtual ~RequestCMDParser();

		int parseTaskGroup(ASSET_MESSAGE_STRUCT Message, RequestCommand& cmd, session_ptr ptrSocket);
	private:

	};
} // namespace WorkPlan
#endif
