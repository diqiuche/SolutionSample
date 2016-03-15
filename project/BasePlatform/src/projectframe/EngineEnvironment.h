////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2011, Shanghai Flagship Prime Brokerage Technology LTD
/// All rights reserved.
///
/// Environment Class, responsible for environment variable value get;
///
/// @version $Id: EngineEnvironment.h 1068 2012-02-14 07:46:19Z liuzq $
////////////////////////////////////////////////////////////////////////////////

#ifndef WORKPLAN_ENGINEENVIRONMENT_H_
#define WORKPLAN_ENGINEENVIRONMENT_H_

#include <string>
#include <list>

using namespace std;


namespace itstation 
{
	class EngineEnvironment
	{
	private:      
		string config_file_path;

		//Singlton instance, static object here, 
		//because it only hold environment variable value of engine.
		static EngineEnvironment instance_;

	public:
		static EngineEnvironment* getInstance();

	protected:
		explicit EngineEnvironment();

	public:
		virtual ~EngineEnvironment();

		const string get_config_file_path() const;   

	};

	inline const string EngineEnvironment::get_config_file_path() const 
	{
		return config_file_path;
	}

} // namespace itstation
#endif
