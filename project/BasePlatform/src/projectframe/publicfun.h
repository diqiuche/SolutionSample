#ifndef WORKPLAN_PUBLIC_FUNCTION_
#define WORKPLAN_PUBLIC_FUNCTION_

#ifdef WIN32
#include <float.h>

#define is_nan(x) _isnan(x)

#ifndef NAN
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const double *) __nan)
#endif

#elif defined __GNUG__
#include <math.h>
#define is_nan(x) isnan(x)
#endif

const float EPSINON = 0.00001f;
//正数的四舍五入宏定义：如果需要计算负数，计算方法是：-DOUBLE_ROUND(-dValue);
#define DOUBLE_ROUND(dValue) (double)((int)(((dValue) * 100) + 0.5) / 100.0);

#define GETBUF(szDataBuf, nNewSize,nOldSize) \
	if ( nNewSize>nOldSize ) \
	{\
		nOldSize = nNewSize;\
		delete m_szReplyBuf;\
		m_szReplyBuf = new char[nNewSize];\
		if (m_szReplyBuf==NULL )\
		{\
			printf( "Memory exhausted!" );\
			exit( -444 );\
		}\
	}\
	szDataBuf = m_szReplyBuf;

namespace PUBLIC_FUN
{
	void GetMysqlConnectSetting( std::string &ip, std::string &username, std::string &password, std::string &dbname, int &dbport );

	//void GetMessageQueueSetting( std::string &srv_address, std::string &produer_queue_name, std::string &consumer_queue_name );

	void GetLocalDateTime( int &date, int &time );

	//宽字符(wchar_t类型)和多字节字符
	bool ByteToWchar(unsigned char *dest, const char *src);
	bool WcharToByte(char *dest, const unsigned char *src);

	void MemoryExhausted();
}

#endif