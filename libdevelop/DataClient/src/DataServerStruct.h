#ifndef DATA_SERVER_STRUCT_H_
#define DATA_SERVER_STRUCT_H_

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <sstream>

namespace itstation {

#define COMM_TICK_DATA			'0'
#define COMM_TICK_SET			'1'
#define COMM_KLINE_DATA			'2'
#define COMM_KLINE_SET			'3'
typedef char CommDataType;		//通信数据类型

#define COMM_IDENTIFY '\xff'	//通信标志

#define DOT_DOT_ONE_POINT	'0'	//0.01
#define DOT_DOT_TWO_POINT	'1'	//0.02
#define DOT_DOT_FIVE_POINT	'2'	//0.05
#define DOT_ONE_POINT		'3'	//0.1
#define DOT_TWO_POINT		'4'	//0.2
#define DOT_FIVE_POINT		'5'	//0.5
#define ONE_POINT			'6'	//1
#define TWO_POINT			'7'	//2
#define FIVE_POINT			'8'	//5
typedef char MovePointType;		//最小跳动

#define EXCHANGE_SSE		'0'	// 上海证券交易所			 
#define EXCHANGE_SZE		'1'	// 深圳证券交易所			 
#define EXCHANGE_CFFEX		'2'	// 中国金融期货交易所		 
#define EXCHANGE_DCE		'3'	// 大连商品交易所			 
#define EXCHANGE_CZCE		'4'	// 中国郑州商品交易所		 
#define EXCHANGE_SHFE		'5'	// 上海期货交易所
#define EXCHANGE_HK			'6'
#define EXCHANGE_OTHER		'7'
typedef char ExchangeIdType;	//交易所类型

#define PRODUCT_STOCK		'0'
#define PRODUCT_FUTURE		'1'
#define PRODUCT_OPTION		'2'
#define PRODUCT_BOND		'3'
#define PRODUCT_FUND		'4'
#define PRODUCT_INDEX		'5' 
#define PRODUCT_OTHER		'6' 
typedef char ProdectIdType;		//品种类型

#define DIMENSION_SECOND	'0'
#define DIMENSION_MINUTE	'1'	
#define DIMENSION_HOUR		'2'	
#define DIMENSION_DAY		'3'
#define DIMENSION_WEEK		'4'
#define DIMENSION_MONTH		'5'
#define DIMENSION_YEAR		'6'
typedef char  KlineDimensionType;

typedef char TimeFieldType;	//时间域，为年份时表示和1900年只差

typedef char InstrumentIdType[23];
typedef char InstrumentNameType[24];

#define NAN_PRICE_UNIT '\x80'	//代表空值
typedef signed char PriceMoveUnit;	//买、卖5档相对于最新价的偏移单位（以最小跳动为单位），卖盘为大于0，买盘为小于0

#define FLOAT_NAN -999999
#define is_float_nan(num) ((num) < FLOAT_NAN + 0.000001) && ((num) > FLOAT_NAN - 0.000001)

enum{
	PRICE_LEVEL_NUM = 5
};

#pragma pack(1)
//通信数据头 (2B)
struct CommDataHead {
	char identify;
	CommDataType type;
	CommDataHead() {
		identify = COMM_IDENTIFY;
		type = COMM_TICK_DATA;
	}
};
//交易品种 (10B)
struct Symbol {
	ProdectIdType product;
	ExchangeIdType exchange;
	InstrumentIdType instrument;
	Symbol(char prod = PRODUCT_STOCK, char exch = EXCHANGE_SSE, char* inst = "") : product(prod), exchange(exch) { memcpy(instrument, inst, sizeof(InstrumentIdType)); }
	Symbol(const Symbol& other) : product(other.product), exchange(other.exchange) { memcpy(instrument, other.instrument, sizeof(InstrumentIdType)); }
	Symbol& operator=(const Symbol& other) {
		if (this != &other) {
			product = other.product;
			exchange = other.exchange;
			memcpy(instrument, other.instrument, sizeof(InstrumentIdType));
		}
		return *this;
	}

	explicit Symbol(char* str){
		product = str[0];
		exchange = str[1];
		memcpy(instrument, str + 3, sizeof(InstrumentIdType));
	}

	std::string Str() {
		char str[12] = {0};
		sprintf(str, "%c%c_%s", product, exchange, instrument);
		return str;
	}

	bool operator==(const Symbol& other) const {
		return (product == other.product) && (exchange == other.exchange) && (strcmp(instrument, other.instrument) == 0);
	}
	bool operator!=(const Symbol& other) const {
		return !(*this == other);
	}
	bool operator<(const Symbol& other) const {
		return product < other.product  || (product == other.product && exchange < other.exchange) ||
			(product == other.product && exchange == other.exchange && strcmp(instrument, other.instrument) < 0);
	}
	bool operator>(const Symbol& other) const {
		return product > other.product  || (product == other.product && exchange > other.exchange) ||
			(product == other.product && exchange == other.exchange && strcmp(instrument, other.instrument) > 0);
	}
	bool operator<=(const Symbol& other) const { return !(*this > other); }
	bool operator>=(const Symbol& other) const { return !(*this < other); }
};
//
struct SymbolEx : public Symbol {
	InstrumentNameType name;
};
//
struct SimpleDate {
	short year;
	char month;
	char day;
	explicit SimpleDate(short y=0, char m=0, char d=0) : year(y), month(m), day(d) {}

	bool operator==(const SimpleDate& other) const {
		return year == other.year && month == other.month && day == other.day;
	}
	bool operator!=(const SimpleDate& other) const {
		return !(*this == other);
	}
	bool operator<(const SimpleDate& other) const {
		return year < other.year || (year == other.year && month < other.month) || 
			(year == other.year && month == other.month && day < other.day);
	}
	bool operator>(const SimpleDate& other) const {
		return year > other.year || (year == other.year && month > other.month) || 
			(year == other.year && month == other.month && day > other.day);
	}
	bool operator<=(const SimpleDate& other) const {
		return !(*this > other);
	}
	bool operator>=(const SimpleDate& other) const {
		return !(*this < other);
	}
};
//
struct SimpleTime {
	char hour;
	char minute;
	char sec;
	short mil_sec;
	explicit SimpleTime(char h=0, char M=0, char s=0, short ms = 0) : hour(h), minute(M), sec(s), mil_sec(ms) {};

	bool operator==(const SimpleTime& other) const {
		return hour == other.hour && minute == other.minute && sec == other.sec && mil_sec == other.mil_sec;
	}
	bool operator!=(const SimpleTime& other) const {
		return !(*this == other);
	}
	bool operator<(const SimpleTime& other) const {
		return hour < other.hour || (hour == other.hour && minute < other.minute) || 
			(hour == other.hour && minute == other.minute && sec < other.sec) ||
			(hour == other.hour && minute == other.minute && sec == other.sec && mil_sec < other.mil_sec);
	}
	bool operator>(const SimpleTime& other) const {
		return hour > other.hour || (hour == other.hour && minute > other.minute) || 
			(hour == other.hour && minute == other.minute && sec > other.sec) ||
			(hour == other.hour && minute == other.minute && sec == other.sec && mil_sec > other.mil_sec);
	}
	bool operator<=(const SimpleTime& other) const {
		return !(*this > other);
	}
	bool operator>=(const SimpleTime& other) const {
		return !(*this < other);
	}

	int AddMin(TimeFieldType min) {
		//assert(min >= 0);	//目前只处理大于0的情况
		minute += min;
		if (minute >= 60)
		{
			hour += minute / 60;
			minute %= 60;
			if (hour > 24)
			{
				int day = hour / 24;
				hour %= 24;
				return day;
			}
		}
		else if (minute < 0)
		{
			hour += minute / 60 - 1;
			minute %= 60;
			if (hour < 0)
			{
				int day = hour / 24 - 1;
				hour %= 24;
				return day;
			}
		}
		return 0;
	}

	int AddSec(int s)
	{
		sec += s;
		if (sec >= 60)
		{
			int min = sec / 60;
			sec %= 60;
			return AddMin(min);
		}
		else if (sec < 0)
		{
			int min = sec / 60 - 1;
			sec %= 60;
			return AddMin(min);
		}
		return 0;
	}
};
//时间 (6B)
struct SimpleDateTime {
	SimpleDate date;
	SimpleTime time;
	SimpleDateTime(SimpleDate d = SimpleDate(), SimpleTime t = SimpleTime()) : date(d), time(t) {};

	explicit SimpleDateTime(time_t t)
	{
		struct tm *tp = localtime(&t);
		date.year = tp->tm_year + 1900;
		date.month = tp->tm_mon + 1;
		date.day = tp->tm_mday;
		time.hour = tp->tm_hour;
		time.minute = tp->tm_min;
		time.sec = tp->tm_sec;
		time.mil_sec = 0;
	}

	bool operator==(const SimpleDateTime& other) {
		return date == other.date && time == other.time;
	}
	bool operator!=(const SimpleDateTime& other) {
		return !(*this == other);
	}
	bool operator<(const SimpleDateTime& other) {
		return date < other.date || (date == other.date && time < other.time);
	}
	bool operator>(const SimpleDateTime& other) {
		return date > other.date || (date == other.date && time > other.time);
	}
	bool operator<=(const SimpleDateTime& other) {
		return !(*this > other);
	}
	bool operator>=(const SimpleDateTime& other) {
		return !(*this < other);
	}

	std::string Str() {
		std::stringstream ss;
		ss << date.year << "-" << int(date.month) << "-" << int(date.day)
			<<" " << int(time.hour) <<":" << int(time.minute) << ":" <<int(time.sec) << "." <<int(time.mil_sec);
		return ss.str();
	}
};

//基本的TICK数据 (64B)
struct BaseTick : CommDataHead {
	Symbol symbol;						//25B
	SimpleDateTime date_time;			//9B
	float last_price;
	float volume;	//当天的总成交量
	float amount;	//当天的总成交额
	//为了行情体系的简单，添加以下字段
	float pre_close;
	float today_open;
	float today_high;
	float today_low;
};
//股票TICK数据 (114B)
struct StockTick : public BaseTick {
	//偏移量为0.01
	PriceMoveUnit buy_price[PRICE_LEVEL_NUM];	//买1~5价偏移
	PriceMoveUnit sell_price[PRICE_LEVEL_NUM];	//卖1~5价偏移
	float buy_volume[PRICE_LEVEL_NUM];	//1~5档的买盘委托量
	float sell_volume[PRICE_LEVEL_NUM];	//1~5档的卖盘委托量
};
//期货TICK数据 (88B)
struct FutureTick : public BaseTick {
	float position;	//持仓量
	float pre_settlement;	//昨结算价
	float buy_volume;	
	float sell_volume;
	float buy_price;	
	float sell_price;
};
//指数TICK数据 (72B)
struct IndexTick : public BaseTick {
	int advance_num;	//上涨家数
	int decline_num;	//下跌家数
};
//TICK数据集
struct TickSet : CommDataHead  {
	ProdectIdType product;
	unsigned int num;
	void* data;
};
//K线信息
struct KlineInfo {
	Symbol symbol;
	KlineDimensionType dimension;
	unsigned short dimen_cnt;
};
//K线数据
struct Kline {
	SimpleDateTime b_time;
	SimpleDateTime e_time;
	float open;
	float high;
	float low;
	float close;
	float volume;

	Kline() : open(0), high(0), low(0), close(0), volume(0), b_time(SimpleDateTime()), e_time(SimpleDateTime()) {}
	void clear() { open = 0; high = 0; low = 0; close = 0; volume = 0; }
};
//
struct KlineBuffer {
	KlineInfo info;
	unsigned int num;
	Kline* data;
};
//临时
typedef struct
{
	SimpleDateTime	m_time;				 //UCT

	float	m_fOpen;			 //开盘
	float	m_fHigh;			 //最高
	float	m_fLow;				 //最低
	float	m_fClose;			 //收盘
	float	m_fVolume;			 //量
	float	m_fAmount;			 //额

	short  m_wAdvance;			 //涨数,仅大盘有效
	short  m_wDecline;			 //跌数,仅大盘有效
}DayKline; //日K线
typedef struct  
{
	SimpleDateTime	m_time;				// UCT
	float	m_fPrice;
	float	m_fVolume;
	float	m_fAmount;
}MinuteKline; //分时数据
#pragma pack()

}

#endif