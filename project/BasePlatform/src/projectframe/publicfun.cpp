
#include <string>
#include <string.h>
#include "EngineEnvironment.h"
#include "Properties.h"
#include "Applog.h"

#ifndef _WIN32
	#include <sys/time.h>
#endif

namespace PUBLIC_FUN
{
	void GetMysqlConnectSetting( std::string &ip, std::string &username, std::string &password, std::string &dbname, int &dbport )
	{
		std::string db_file_path = itstation::EngineEnvironment::getInstance()->get_config_file_path();

		itstation::common::Properties  cfgfile;

		try
		{
			cfgfile.Load( db_file_path );
			ip			= cfgfile.GetProperties( "MYSQL", "ip" );
			username	= cfgfile.GetProperties( "MYSQL", "username" );
			password	= cfgfile.GetProperties( "MYSQL", "password" );
			dbname		= cfgfile.GetProperties( "MYSQL", "dbname" );
			dbport		= cfgfile.GetPropertiesAsInt( "MYSQL", "dbport" );
		}
		catch(...)
		{
			printf( "Exception @ GetMysqlConnectSetting\n" );
		}
	}

	//void GetMessageQueueSetting( std::string &srv_address, std::string &queue_name )
	//{
	//	std::string db_file_path = WorkPlan::EngineEnvironment::getInstance()->get_mq_conf_file_();

	//	Q_COMMON::Properties  cfgfile;
	//	cfgfile.Load( db_file_path );
	//	srv_address			= cfgfile.GetProperties( "ActiveMQ", "Connstring" );

	//	queue_name	= cfgfile.GetProperties( "MONITOR", "METoMonitor" );
	//}

	void GetLocalDateTime( int &nDate, int &nTime )
	{
#ifdef _WIN32
		SYSTEMTIME st;
		GetLocalTime(&st);
		nDate = st.wYear * 10000 + st.wMonth * 100 + st.wDay;
		nTime = st.wHour * 10000 + st.wMinute * 100 + st.wSecond;
#else
		time_t t_date = time(NULL);
		tm* local_t = localtime(&t_date);

		nDate = (local_t->tm_year+1900) * 10000 + (local_t->tm_mon+1) * 100 + local_t->tm_mday;
		nTime = local_t->tm_hour * 10000 + local_t->tm_min * 100 + local_t->tm_sec;
#endif
	}

	bool ByteToWchar(unsigned char *dest, const char *src)
	{
#ifdef _WIN32
		setlocale(LC_ALL, "chinese");
		int len = mbstowcs(NULL, src, 0) + 1;
		mbstowcs((wchar_t *)dest, src, len);
#else
		setlocale(LC_ALL, "zh_CN.gb2312");
		int len = mbstowcs(NULL, src, 0) + 1;
		mbstowcs((wchar_t *)dest, src, len);
#endif
		return true;
	}

	bool WcharToByte(char *dest, const unsigned char *src)
	{
#ifdef _WIN32
		setlocale(LC_ALL, "chinese");
		int len = wcstombs(NULL, (wchar_t*)src, 0) + 1;
		wcstombs(dest, (wchar_t*)src, len);
		dest[len-1] = '\0';
#else
		unsigned char temp[512] = {0};
		unsigned char temp2[2] = {0};
		for (int i = 0; i < 128; i++) 
		{
			memcpy(temp+i*4, src+i*2, 2);
			memcpy(temp+i*4+2, temp2, 2);
		}
		setlocale(LC_ALL, "zh_CN.gb2312");
		int len = wcstombs(NULL, (wchar_t*)temp, 0) + 1;
		wcstombs(dest, (wchar_t*)temp, len);
		dest[len-1] = '\0';
#endif
		return true;
	}
	
	void MemoryExhausted()
	{
		APP_LOG(itstation::common::Applog::LOG_ERROR) << "Memory exhausted!";
		exit( -444 );
	}
}
