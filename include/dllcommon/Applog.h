#ifndef EYEGLE_COMMON_APPLOG_H
#define EYEGLE_COMMON_APPLOG_H

#include <fstream>
#include <sstream>
#include <string>
#include "Mutex.h"


#define APP_LOG(LEVEL) itstation::common::AppLogFinisher() = itstation::common::AppLogInput(LEVEL, __FILE__, __LINE__)

namespace itstation {
	namespace common {

		class COMMON_API Applog
		{
		public:
			enum level{
				LOG_DEBUG_LIB = 1,
				LOG_INFO,
				LOG_DEBUG,
				LOG_WARNING,
				LOG_ERROR,
				LOG_CRITICAL
			};

			// standard interface for user
			static void SetLogLevel(unsigned int loglevel);
			std::string GetTimeStr();

			void log(level elevel, const std::string& info, const std::string& msg);

			static Applog* getInstance();

			class DestructorClass //它的唯一工作就是在析构函数中删除CSingleton的实例 
			{ 
			public: 
				~DestructorClass() 
				{ 	
					if(Applog::instance_) 
					{
						MutexLocker lock(&m_mutex);
						if ( Applog::instance_)
						{
							delete Applog::instance_;
							Applog::instance_ = NULL;
						}
					}
				} 
			}; 
			static DestructorClass dest; //定义一个静态成员变量，程序结束时，系统会自动调用它的析构函数
		private:
			Applog();
			virtual ~Applog();

			static Applog* instance_;   

			static int log_level;

			static Mutex m_mutex;
		};

		class COMMON_API AppLogInput
		{    
		private:
			std::stringstream info;
			std::stringstream message_stream;
			Applog::level m_elevel;
		public:
			AppLogInput(Applog::level elevel, const char* filename, int line):m_elevel(elevel) 
			{   
				info << filename << ":" << line;
			};

			friend class AppLogFinisher;
			AppLogInput& operator<<(char c)                 {message_stream<< c; return *this;};
			AppLogInput& operator<<(short s)                {message_stream<< s; return *this;};
			AppLogInput& operator<<(int n)                  {message_stream<< n; return * this;};
			AppLogInput& operator<<(unsigned int un)        {message_stream<< un; return * this;};
			AppLogInput& operator<<(float f)                {message_stream<< f; return * this;};
			AppLogInput& operator<<(double d)               {message_stream<< d; return * this;};
			AppLogInput& operator<<(long l)                 {message_stream<< l; return * this;};
			AppLogInput& operator<<(const char* msg)        {message_stream<<msg; return *this;};
			AppLogInput& operator<<(const std::string& msg) {message_stream<<msg; return *this;};

		};

		class COMMON_API AppLogFinisher
		{
		public:
			void operator=(AppLogInput& input) 
			{
				Applog::getInstance()->log(input.m_elevel, input.info.str(), 
					input.message_stream.str());
			};
		};

	}
}

#endif

