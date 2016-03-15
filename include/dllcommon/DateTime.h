#ifndef EYEGLE_COMMON_DATE_TIME_H_
#define EYEGLE_COMMON_DATE_TIME_H_

#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <sys/time.h>
#endif


namespace itstation {
	namespace common {

#pragma pack(1)
struct Date {
	int year;
	int month;
	int day;

	Date() : year(1900), month(1), day(1) {}

	Date(int y, int m, int d) : year(y), month(m), day(d) {}

	Date(char* date_str) {
		if (strlen(date_str) != 8) {
			year = 1900;
			month = 1;
			day = 1;
			return;
		}

		sscanf(date_str, "%4d%2d%2d", &year, &month, &day);
	}

	Date(const Date& date) : year(date.year), month(date.month), day(date.day) {}

	Date& operator=(const Date& date) {
		if (this != &date) {
			year = date.year;
			month = date.month;
			day = date.day;
		}

		return *this;
	}

	bool operator==(const Date& date) const
	{
		if(year == date.year && month == date.month && day == date.day) { return true; }

		return false;
	}

	bool operator>(const Date& date) const
	{
		if(year > date.year || (year == date.year && month > date.month) || (year == date.year && month == date.month && day > date.day)) {
			return true;
		}
			
		return false;
	}

	bool operator<(const Date& date) const
	{
		if(year < date.year || (year == date.year && month < date.month) || (year == date.year && month == date.month && day < date.day)) {
			return true;
		}

		return false;
	}

	bool operator<=(const Date& date) const
	{
		return !(*this > date);
	}

	bool operator>=(const Date& date) const
	{
		return !(*this < date);
	}

	bool operator!=(const Date& date) const
	{
		return !(*this == date);
	}

	std::string Str() const {
		std::stringstream ss;
		ss<<year<<"-"
			<<std::setfill('0')<<std::setw(2)<<month<<"-"
			<<std::setfill('0')<<std::setw(2)<<day;
		return ss.str();
	}
};

struct Time {
	int hour;
	int minute;
	int sec;
	int milsec;

	Time() : hour(0), minute(0), sec(0), milsec(0) {}

	Time(int h, int m, int s, int ms = 0) : hour(h), minute(m), sec(s), milsec(ms) {}

	Time(const Time& t) : hour(t.hour), minute(t.minute), sec(t.sec), milsec(t.milsec) {}

	Time(char* time_str, int mil) {
		milsec = mil;
		if (strlen(time_str) != 8) {
			hour = 0;
			minute = 0;
			sec = 0;
			return;
		}

		sscanf(time_str, "%2d:%2d:%2d", &hour, &minute, &sec);
	}

	Time& operator=(const Time& t) {
		if (this != &t) {
			hour = t.hour;
			minute = t.minute;
			sec = t.sec;
			milsec = t.milsec;
		}

		return *this;
	}

	bool operator==(const Time& t) const
	{
		if(hour == t.hour && minute == t.minute && sec == t.sec && milsec == t.milsec) { return true; }

		return false;
	}

	bool operator>(const Time& t) const
	{
		if(hour > t.hour || (hour == t.hour && minute > t.minute) || (hour == t.hour && minute == t.minute && sec > t.sec)
				|| (hour == t.hour && minute == t.minute && sec == t.sec && milsec > t.milsec)) {
			return true;
		}

		return false;
	}

	bool operator<(const Time& t) const
	{
		if(hour < t.hour || (hour == t.hour && minute < t.minute) || (hour == t.hour && minute == t.minute && sec < t.sec)
				|| (hour == t.hour && minute == t.minute && sec == t.sec && milsec < t.milsec)) {
			return true;
		}

		return false;
	}

	bool operator<=(const Time& t) const
	{
		return !(*this > t);
	}

	bool operator>=(const Time& t) const
	{
		return !(*this < t);
	}

	bool operator!=(const Time& t) const
	{
		return !(*this == t);
	}

	friend Time operator+(const Time& t1, const Time& t2) {
		long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
		long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
		long ret_milsecs = t1_milsecs + t2_milsecs;
		int ret_hour = ret_milsecs / (60 * 60 * 1000);
		int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
		int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
		int ret_milsec = ret_milsecs % 1000;

		return Time(ret_hour, ret_min, ret_sec, ret_milsec);
	}

	friend Time operator-(const Time& t1, const Time& t2) {
		long t1_milsecs = ((t1.hour * 60 + t1.minute) * 60 + t1.sec) * 1000 + t1.milsec;
		long t2_milsecs = ((t2.hour * 60 + t2.minute) * 60 + t2.sec) * 1000 + t2.milsec;
		long ret_milsecs = t1_milsecs - t2_milsecs;
		int ret_hour = ret_milsecs / (60 * 60 * 1000);
		int ret_min = (ret_milsecs % (60 * 60 * 1000)) / (60 * 1000);
		int ret_sec = (ret_milsecs % (60 * 1000)) / 1000;
		int ret_milsec = ret_milsecs % 1000;

		return Time(ret_hour, ret_min, ret_sec, ret_milsec);
	}

	std::string Str() const {
		std::stringstream ss;
		ss<<std::setfill('0')<<std::setw(2)<<hour<<":"
			<<std::setfill('0')<<std::setw(2)<<minute<<":"
			<<std::setfill('0')<<std::setw(2)<<sec<<"."
			<<std::setfill('0')<<std::setw(3)<<std::setiosflags(std::ios::right)<<milsec;
		return ss.str();
	}
};

struct DateTime {
	Date m_date;
	Time m_time;

	DateTime() {}

	//获取当前时间
	DateTime(void* p) {
#ifdef WIN32
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		m_date = Date(sys.wYear,sys.wMonth, sys.wDay);
		m_time = Time(sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
#else
		struct timeval t_time;
		gettimeofday(&t_time, NULL);
		time_t t_date;
		time(&t_date);
		tm* local_t = localtime(&t_date);
		m_date = Date(local_t->tm_year+1900, local_t->tm_mon+1, local_t->tm_mday);
		m_time = Time(local_t->tm_hour, local_t->tm_min, local_t->tm_sec, t_time.tv_usec/1000);
#endif
	}

	DateTime(const Date& d, const Time& t) : m_date(d), m_time(t) {}

	DateTime(const DateTime& date_time) :m_date(date_time.m_date), m_time(date_time.m_time) {}

	DateTime& operator=(const DateTime& date_time) {
		if (this != &date_time) {
			m_date = date_time.m_date;
			m_time = date_time.m_time;
		}

		return *this;
	}

	bool operator==(const DateTime& date_time) const
	{
		if(m_date == date_time.m_date && m_time == date_time.m_time) { return true; }

		return false;
	}

	bool operator>(const DateTime& date_time) const
	{
		if(m_date > date_time.m_date || (m_date == date_time.m_date && m_time > date_time.m_time)) { return true; }

		return false;
	}

	bool operator<(const DateTime& date_time) const
	{
		if(m_date < date_time.m_date || (m_date == date_time.m_date && m_time < date_time.m_time)) { return true; }

		return false;
	}

	bool operator!=(const DateTime& date_time) const {
		return !(*this == date_time);
	}

	bool operator>=(const DateTime& date_time) const {
		return !(*this < date_time);
	}

	bool operator<=(const DateTime& date_time) const {
		return !(*this > date_time);
	}

	friend DateTime operator+(const DateTime& t1, const Time& t2) {
		Time t3 = t1.m_time + t2;
		return DateTime(t1.m_date, t3);
	}

	friend DateTime operator+(const Time& t1, const DateTime& t2) {
		return t2 + t1;
	}

	friend DateTime operator-(const DateTime& t1, const Time& t2) {
		Time t3 = t1.m_time - t2;
		return DateTime(t1.m_date, t3);
	}

	std::string Str() const {
		std::stringstream ss;
		ss<<m_date.Str()<<" "<<m_time.Str();
		return ss.str();
	}
};
#pragma pack()

}
}
#endif	//EYEGLE_COMMON_DATE_TIME_H_
