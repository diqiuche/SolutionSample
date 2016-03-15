////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Request command data class. Holding task list which will execute
///
/// @version $Id: RequestCommand.h 1068 2012-02-14 07:46:19Z liuzq $
////////////////////////////////////////////////////////////////////////////////

#ifndef WORKPLAN_REQUESTCOMMAND_H
#define WORKPLAN_REQUESTCOMMAND_H

#include "EngineTask.h"
#include <memory>
#include <map>

using namespace std;

namespace itstation 
{

	class RequestCommand {
	public:
		RequestCommand();
		virtual ~RequestCommand();

		void exeCommandImpl();

		map<int, std::shared_ptr<EngineTask> > tasks_;

	private:

	};


} // namespace itstation 
#endif
