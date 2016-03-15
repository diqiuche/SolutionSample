#include <stdlib.h>
#ifdef WIN32
#include <process.h>
#include <io.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/unistd.h>
#endif
#include "Applog.h"
#include <iostream>


namespace itstation {
	namespace common {

		static const char* LOG_LEVEL_STR[] = {
			"NULL",
			"(DEBUG_L)",
			"(INFO)",
			"(DEBUG)",
			"(WARNING)",
			"(ERROR)",
			"(CRITICAL)"
		};

		Applog* Applog::instance_ = NULL;
		int     Applog::log_level = LOG_DEBUG_LIB;
		Mutex   Applog::m_mutex;
		Applog::DestructorClass Applog::dest;

		Applog::Applog()
		{
		}

		Applog::~Applog()
		{
		}

		Applog* Applog::getInstance()
		{
			if ( instance_ == NULL )
			{
				MutexLocker lock(&m_mutex);
				if ( instance_ == NULL )
				{
					instance_ = new Applog();
				}
			}
			return instance_;
		}

		void Applog::SetLogLevel(unsigned int log_level)
		{
			Applog::log_level = log_level;
		}

		std::string Applog::GetTimeStr()
		{
			char date_str[25];
#ifdef WIN32
			SYSTEMTIME sys;
			GetLocalTime(&sys);
			sprintf(date_str,"%4d/%02d/%02d %02d:%02d:%02d.%03d", sys.wYear,sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
			struct timeval t_time;
			gettimeofday(&t_time, NULL);
			time_t t_date;
			time(&t_date);
			tm* local_t = localtime(&t_date);
			sprintf(date_str,"%4d/%02d/%02d %02d:%02d:%02d.%03ld", local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday, local_t->tm_hour, local_t->tm_min, local_t->tm_sec, t_time.tv_usec/1000);
#endif
			return date_str;    
		}

		void Applog::log(level elevel, const std::string& info, const std::string& msg)
		{
			MutexLocker lock(&m_mutex);
			if(1)
			{
				bool enable_file_log = true;
				bool enable_cout_log = true;
				int file_log_level = Applog::log_level;
				int cout_log_level = Applog::log_level;
				char* env_data;
				env_data = getenv("ENABLE_FILE_LOG");
				if(env_data)
				{
					std::string data = env_data;
					if(data == "1")
					{
						enable_file_log = true;
					}
					else
					{				
						enable_file_log = false;
					}
				}
				env_data = getenv("ENABLE_COUT_LOG");
				if(env_data)
				{
					std::string data = env_data;
					if(data == "1")
					{
						enable_cout_log = true;
					}
					else
					{				
						enable_cout_log = false;
					}
				}

				env_data = getenv("FILE_LOG_LEVEL");
				if(env_data)
				{
					std::istringstream t(env_data);
					t >> file_log_level;
				}
				env_data = getenv("COUT_LOG_LEVEL");
				if(env_data)
				{
					std::istringstream t(env_data);
					t >> cout_log_level;
				}

				if(enable_cout_log && elevel >= cout_log_level)
				{
					std::cout << msg << std::endl;
				}
				if(enable_file_log && elevel >= file_log_level)
				{
					std::string file = "running.log";
					{
						std::string log_file;
						char *logpath;
						logpath = getenv("LOG_PATH");
						if(logpath)
						{
							log_file = logpath;
						}
						else
						{
							log_file = "../log/";
						}
						int t;
#ifdef WIN32
						t = _mkdir(log_file.c_str());
#else
						t = mkdir(log_file.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif
						{
							char date_str[25];
#ifdef WIN32
							SYSTEMTIME sys;
							GetLocalTime(&sys);
							sprintf(date_str,"%4d%02d%02d", sys.wYear,sys.wMonth, sys.wDay);
#else
							struct timeval t_time;
							gettimeofday(&t_time, NULL);
							time_t t_date;
							time(&t_date);
							tm* local_t = localtime(&t_date);
							sprintf(date_str,"%4d%02d%02d", local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday);
#endif


							log_file += date_str;
#ifdef WIN32
							t = _mkdir(log_file.c_str());
#else
							t = mkdir(log_file.c_str(), S_IRWXU | S_IRWXG | S_IROTH);
#endif

						}
						log_file += "/";
						log_file += file;
						std::ofstream tt2(log_file.c_str(), std::ios::app);
						tt2 << GetTimeStr() << "/";
						tt2 << LOG_LEVEL_STR[elevel];
						tt2 << ":	";				
						tt2 << msg;
						tt2 << "	";
						unsigned int pos;
#ifdef WIN32
						pos = info.find_last_of('\\');
#else
						pos = info.find_last_of('/');
#endif
						if(pos >= 0 && pos < info.size())
						{
							std::string t2 = info.substr(pos+1, info.size());
							tt2  << "(" << t2 << ")";
						}
						else
						{
							tt2  << "(" << info << ")";
						}	
						tt2 << std::endl;
						tt2.close();

						if(elevel >= LOG_ERROR)
						{
							log_file += ".error.txt";
							std::ofstream tt2(log_file.c_str(), std::ios::app);
							tt2 << GetTimeStr() << "/";
							tt2 << LOG_LEVEL_STR[elevel];
							tt2 << ":	";				
							tt2 << msg;
							tt2 << "	";
							unsigned int pos;
#ifdef WIN32
							pos = info.find_last_of('\\');
#else
							pos = info.find_last_of('/');
#endif
							if(pos >= 0 && pos < info.size())
							{
								std::string t2 = info.substr(pos+1, info.size());
								tt2  << "(" << t2 << ")";
							}
							else
							{
								tt2  << "(" << info << ")";
							}	
							tt2 << std::endl;
							tt2.close();
						}
					}
				}		
			}
			return;
		}

	}
}
