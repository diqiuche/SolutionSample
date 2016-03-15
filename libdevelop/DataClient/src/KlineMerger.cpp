#include "dataserver/KlineMerger.h"
#include <sstream>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QDate>
#include <QSettings>
#include "common/Global.h"
#include "common/AppLog.h"

namespace itstation {

using namespace common;

iKlineMerger::iKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl)
	: m_info(info), m_hdl(hdl), m_sum_vol(0), m_kline(Kline())
{
	m_kline.volume = -2;	//小于-1表示从交易日第一根开始接
	m_pre_kline.volume = -2;	//小于-1表示从交易日第一根开始接
	m_day_kline.volume = -2;	//小于-1表示从交易日第一根开始接
	//InitSectionTime();
}

std::string iKlineMerger::ExchangeName(ExchangeIdType exchange)
{
	switch(exchange)
	{
	case EXCHANGE_SSE:
		return "SSE";
	case EXCHANGE_SZE:
		return "SZE";
	case EXCHANGE_CFFEX:
		return "CFFEX";
	case EXCHANGE_DCE:
		return "DCE";
	case EXCHANGE_CZCE:
		return "CZCE";
	case EXCHANGE_SHFE:
		return "SHFE";
	case EXCHANGE_HK:
		return "HK";
	default:
		return "OTHER";
	}
}

void iKlineMerger::InitSectionTime()
{
	std::string fnd_home = Global::GetInstance()->fnd_home;
	std::string conf_path = fnd_home + "\\config\\TradingTime.ini";
	
	if (!QFile::exists(conf_path.c_str()))
	{
		APP_LOG(LOG_LEVEL_ERROR) << conf_path << " is not exist";
		throw (conf_path + " is not exist");
	}

	QSettings settings(conf_path.c_str(), QSettings::IniFormat);
	std::string exchange_name = ExchangeName(m_info.symbol.exchange);
	if ("SHFE" == exchange_name)
	{
		char code_prefix[3];
		code_prefix[2] = '\0';
		memcpy(code_prefix, m_info.symbol.instrument, 2);
		QString inst_str = settings.value("SHFE.NIGHT1/InstrumentID").toString();
		QStringList insts = inst_str.split(",", QString::SkipEmptyParts);
		if (insts.contains(code_prefix))
		{
			exchange_name = "SHFE.NIGHT1";
		}
		else 
		{
			inst_str = settings.value("SHFE.NIGHT2/InstrumentID").toString();
			insts = inst_str.split(",", QString::SkipEmptyParts);
			if (insts.contains(code_prefix))
			{
				exchange_name = "SHFE.NIGHT2";
			}
			else
			{
				exchange_name = "SHFE.DAY";
			}
		}
	}

	std::string key_name = exchange_name + "/TradingPeriodNum";
	int TradingPeriodNum = settings.value(key_name.c_str()).toInt();
	if (0 >= TradingPeriodNum)
	{
		APP_LOG(LOG_LEVEL_ERROR) << "Invalid SectionNum";
		throw "Invalid SectionNum";
	}

	for (int i=0; i<TradingPeriodNum; ++i)
	{
		std::string start_time_str = settings.value(QObject::tr("%1/TradingPeriod%2Start").arg(exchange_name.c_str()).arg(i+1)).toString().toStdString();
		std::string end_time_str = settings.value(QObject::tr("%1/TradingPeriod%2End").arg(exchange_name.c_str()).arg(i+1)).toString().toStdString();		

		if ("" == start_time_str || "" == end_time_str)
		{
			APP_LOG(LOG_LEVEL_ERROR) << "Invalid config for section time";
			throw ("Invalid config for section time");
		}

		TradeSectionTime section_time;

		stringstream ss3;
		int pos = start_time_str.find(":", 0);
		ss3<<start_time_str.substr(0, pos).c_str();
		int hour; 
		ss3>>hour;
		if (hour > 16)
		{
			hour -= 24;
		}
		ss3.clear();
		ss3<<start_time_str.substr(pos+1).c_str();
		int minute;
		ss3>>minute;
		section_time.begin = SimpleTime(hour, minute, 0);

		pos = end_time_str.find(":", 0);
		ss3.clear();
		ss3<<end_time_str.substr(0, pos).c_str();
		ss3>>hour;
		ss3.clear();
		ss3<<end_time_str.substr(pos+1).c_str();
		ss3>>minute;
		section_time.end = SimpleTime(hour, minute, 0);

		if (section_time.begin >= section_time.end)
		{
			APP_LOG(LOG_LEVEL_ERROR) << "elegal section time";
			throw ("elegal section time");
		}

		m_section_time.push_back(section_time);
	}

	for (int i=0; i < TradingPeriodNum - 1; ++i)
	{
		if (m_section_time[i].end >= m_section_time[i+1].begin)
		{
			APP_LOG(LOG_LEVEL_ERROR) << "elegal section time";
			throw ("elegal section time");
		}
	}
}

void iKlineMerger::SetSectionTime(const std::vector<TradeSectionTime>& section_time)
{
	m_section_time.clear();
	for (int i=0; i<section_time.size(); ++i)
	{
		m_section_time.push_back(section_time[i]);
	}
}

void iKlineMerger::ClenaTick(BaseTick* tick)
{
	if (tick->date_time.time < m_section_time[0].begin)
	{
		tick->date_time.time = m_section_time[0].begin;
	}
	else if (tick->date_time.time >= m_section_time.back().end)
	{
		tick->date_time.time = m_section_time.back().end;
		tick->date_time.time.AddSec(-1);
	}
	else
	{
		for (int i=0; i < m_section_time.size() - 1; ++i)
		{
			//在交易小节时间外
			if (tick->date_time.time >= m_section_time[i].end && tick->date_time.time < m_section_time[i+1].begin)
			{
				//偏离前一个小节一分钟以内，归为前一个小节，否则归为后一个小节
				SimpleTime tmp_time = m_section_time[i].end;
				tmp_time.AddMin(1);
				if (tick->date_time.time <= tmp_time)
				{
					tick->date_time.time = m_section_time[i].end;
					tick->date_time.time.AddSec(-1);
				} 
				else
				{
					tick->date_time.time = m_section_time[i+1].begin;
				}
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CommonKlineMerger::CommonKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl) : iKlineMerger(info, hdl), push_kline_num_(0)
{
	//if (info.symbol.exchange == EXCHANGE_SHFE)
	//{
	//	std::string fnd_home = getenv("FND_HOME");
	//	std::string conf_path = fnd_home + "/config/TradingTime.ini";
	//	if (!QFile::exists(conf_path.c_str()))
	//	{
	//		//APP_LOG(LOG_LEVEL_ERROR) << conf_path << " is not exist";
	//		//throw (conf_path + " is not exist");
	//		cout << conf_path << " is not exist"<<endl;
	//	}

	//	char code_prefix[3];
	//	code_prefix[2] = '\0';
	//	memcpy(code_prefix, info.symbol.instrument, 2);
	//	QSettings settings(conf_path.c_str(), QSettings::IniFormat);
	//	QString inst_str = settings.value("SHFE.DAY/InstrumentID").toString();
	//	QStringList insts = inst_str.split(",", QString::SkipEmptyParts);
	//	if (!insts.contains(code_prefix))
	//	{
	//		//APP_LOG(LOG_LEVEL_ERROR) << "不支持合成夜盘的K线数据:" << info.symbol.code;
	//		cout << conf_path << " is not exist"<<endl;
	//		throw ("不支持合成夜盘的K线数据");
	//	}
	//}
}

void CommonKlineMerger::PushTick(BaseTick* tick)
{
	if (tick->symbol != m_info.symbol)
	{
		APP_LOG(LOG_LEVEL_ERROR) << "receive "<<tick->symbol.instrument << " while except " << m_info.symbol.instrument;
		return;
	}

	//清除脏数据,保证tck交易时间段正常
	ClenaTick(tick);

	//if (m_kline.volume < -1)	//交易日第一根
	//{
	//	m_kline.clear();
	//	m_kline.b_time = tick->date_time.date;
	//	m_kline.b_time.time = m_section_time[0].begin;
	//	AddWithTick(tick, false);
	//	return;
	//}

	while (tick->date_time >= m_kline.e_time)
	{
		AddWithTick(tick);
	}
	UpdateWithTick(tick);

	m_day_kline.open = tick->today_open;
	m_day_kline.volume = tick->volume;
	m_day_kline.high = tick->today_high;
	m_day_kline.close = tick->last_price;
	m_day_kline.low = tick->today_low;
	m_day_kline.b_time.date = tick->date_time.date;
	m_day_kline.e_time.date = tick->date_time.date;
	m_day_kline.b_time.time = SimpleTime(9, 30, 0);
	m_day_kline.e_time.time = SimpleTime(15, 0, 0);
}

void CommonKlineMerger::NextStartTime(SimpleDateTime* date_time)
{
	if (date_time->date != m_kline.e_time.date)	//下一个交易日
	{
		m_sum_vol = 0;
		m_kline.b_time = date_time->date;
		m_kline.b_time.time = m_section_time[0].begin;
	}
	else if (m_section_time.back().end > m_kline.e_time.time)
	{
		m_kline.b_time = m_kline.e_time;
		//NextEndTime()保证m_kline.e_time在交易时间以内
		for (int i=0; i<m_section_time.size() - 1; ++i)
		{
			if (m_kline.b_time.time == m_section_time[i].end)
			{
				m_kline.b_time.time = m_section_time[i+1].begin;
				break;
			}
		}
	}
	else
	{
		APP_LOG(LOG_LEVEL_ERROR) << "out of the trade section";
		throw "out of the trade section";
	}
}

void CommonKlineMerger::NextEndTime()
{
	//tick只用于合成当天的数据或者历史1分钟及秒K线
	if (m_info.dimension >= DIMENSION_DAY)
	{
		m_kline.e_time.date = m_kline.b_time.date;
		m_kline.e_time.time = m_section_time.back().end;
	}
	else if (m_info.dimension == DIMENSION_HOUR)
	{
		m_kline.e_time.date = m_kline.b_time.date;
		SimpleTime e_time = SimpleTime(m_kline.b_time.time.hour + m_info.dimen_cnt, 0, 0);
		if (e_time > m_section_time.back().end)
		{
			e_time = m_section_time.back().end;
		}
		else {
			for (int i=0; i<m_section_time.size()-1; ++i)
			{
				if (e_time > m_section_time[i].end && e_time <= m_section_time[i+1].begin)
				{
					e_time = m_section_time[i].end;
					break;
				}
			}
		}
		m_kline.e_time.time = e_time;
	}
	else if (m_info.dimension == DIMENSION_MINUTE)
	{
		if (m_kline.e_time.date != m_kline.b_time.date)
		{
			m_kline.e_time.date = m_kline.b_time.date;
		}
		SimpleTime e_time = m_kline.b_time.time;
		int sec_idx = -1;
		//找到交易节段
		for (int i=0; i<m_section_time.size(); ++i)
		{
			if (e_time < m_section_time[i].end)
			{
				sec_idx = i;
				break;
			}
		}
		if (sec_idx == -1)
		{
			APP_LOG(LOG_LEVEL_ERROR) << "kline time is out of the trading time";
			throw "kline time is out of the trading time";
		}

		SimpleTime pre_time = e_time;
		int plus_cont = m_info.dimen_cnt;
		e_time.AddMin(plus_cont);
		for (int i=sec_idx; i<m_section_time.size() - 1; ++i)
		{
			if (e_time <= m_section_time[i].end) { break; }	//确定没有跳出该小节

			int sec_min = (m_section_time[i].end.hour - pre_time.hour) * 60 + m_section_time[i].end.minute - pre_time.minute;
			plus_cont -= sec_min;
			e_time = m_section_time[i + 1].begin;
			e_time.AddMin(plus_cont);
			pre_time = m_section_time[i + 1].begin;
		}

		if (e_time > m_section_time.back().end)
		{
			e_time = m_section_time.back().end;
		}

		m_kline.e_time.time = e_time;
	}
	else if (m_info.dimension == DIMENSION_SECOND)
	{
		if (m_kline.e_time.date != m_kline.b_time.date)
		{
			m_kline.e_time.date = m_kline.b_time.date;
		}
		SimpleTime e_time = m_kline.b_time.time;
		int sec_idx = -1;
		//找到交易节段
		for (int i=0; i<m_section_time.size(); ++i)
		{
			if (e_time < m_section_time[i].end)
			{
				sec_idx = i;
				break;
			}
		}
		if (sec_idx == -1)
		{
			APP_LOG(LOG_LEVEL_ERROR) << "kline time is out of the trading time";
			throw "kline time is out of the trading time";
		}

		SimpleTime pre_time = e_time;
		int plus_cont = m_info.dimen_cnt;
		e_time.AddSec(plus_cont);
		for (int i=sec_idx; i<m_section_time.size() - 1; ++i)
		{
			if (e_time <= m_section_time[i].end) { break; }	//确定没有跳出该小节

			int sec_sec = ((m_section_time[i].end.hour - pre_time.hour) * 60 + (m_section_time[i].end.minute - pre_time.minute)) * 60 +
				m_section_time[i].end.sec - pre_time.sec;

			plus_cont -= sec_sec;
			e_time = m_section_time[i + 1].begin;
			e_time.AddSec(plus_cont);
			pre_time = m_section_time[i + 1].begin;
		}

		if (e_time > m_section_time.back().end)
		{
			e_time = m_section_time.back().end;
		}

		m_kline.e_time.time = e_time;
	}
	else {
		APP_LOG(LOG_LEVEL_ERROR) << "unknown kline dimension";
		throw "unknown kline dimension";
	}
}

void CommonKlineMerger::UpdateWithTick(BaseTick* tick)
{
	if (m_kline.high < tick->last_price)
	{
		m_kline.high = tick->last_price;
	}
	if (m_kline.low > tick->last_price)
	{
		m_kline.low = tick->last_price;
	}
	m_kline.close = tick->last_price;
	m_kline.volume += tick->volume - m_sum_vol;
	m_sum_vol = tick->volume;
	
	if (m_hdl)
	{
		m_hdl->OnUpdateKline(this, &m_kline);
	}
}

void CommonKlineMerger::AddWithTick(BaseTick* tick)
{
	m_pre_kline = m_kline;
	m_kline.clear();
	NextStartTime(&tick->date_time);
	NextEndTime();
	m_kline.open = tick->last_price;
	m_kline.high = tick->last_price;
	m_kline.low = tick->last_price;
	m_kline.close = tick->last_price;
	if (tick->date_time < m_kline.e_time)
	{
		m_kline.volume = tick->volume - m_sum_vol;
		m_sum_vol = tick->volume;
		//m_kline.amount = tick->amount;
	}
	
	/*if (tick->symbol_class == SYMBOL_FUTURE)
	{
		FutureTick *future_tick = (FutureTick*)tick;
		m_kline.position = future_tick->position;
	}*/
	if (m_hdl)
	{
		m_hdl->OnAddKline(this, &m_kline);
	}
}

void CommonKlineMerger::PushKline(Kline* kline)
{
	if (IsNewKline(kline->e_time))
	{
		m_kline = *kline;
		if (m_hdl)
		{
			m_hdl->OnAddKline(this, &m_kline);
		}
	}
	else
	{
		if (m_kline.high < kline->high)
		{
			m_kline.high = kline->high;
		}
		if (m_kline.low > kline->low)
		{
			m_kline.low = kline->low;
		}
		m_kline.close = kline->close;
		m_kline.volume += kline->volume;
		//m_kline.amount = kline->amount;
		//m_kline.position = kline->position;
		m_kline.e_time = kline->e_time;
		if (m_hdl)
		{
			m_hdl->OnUpdateKline(this, &m_kline);
		}
	}
}

bool CommonKlineMerger::IsNewKline(const SimpleDateTime& date_time)
{
	//年线、月线和周线维数只有1
	if (DIMENSION_YEAR == m_info.dimension) 
	{
		return date_time.date.year != m_kline.e_time.date.year;
	}
	else if (DIMENSION_MONTH == m_info.dimension) 
	{
		return date_time.date.year != m_kline.e_time.date.year || date_time.date.month != m_kline.e_time.date.month;
	}
	else if (DIMENSION_WEEK == m_info.dimension)
	{
		QDate date1(date_time.date.year, date_time.date.month, date_time.date.day);
		QDate date2(m_kline.e_time.date.year, m_kline.e_time.date.month, m_kline.e_time.date.day);
		return date1.weekNumber() != date2.weekNumber();
	}
	else if (DIMENSION_DAY == m_info.dimension && 1 < m_info.dimen_cnt)	//只能从1日线去合成多日线
	{
		if (push_kline_num_ == 0)	//第一根K线
		{
			push_kline_num_ = 1;
			return true;
		}
		push_kline_num_++;
		if (push_kline_num_ > m_info.dimen_cnt)
		{
			push_kline_num_ = 1;
			return true;
		}
		return false;
	}
	else if (DIMENSION_DAY == m_info.dimension && 1 == m_info.dimen_cnt)	//用1分钟线去合成1日线
	{
		return date_time.date != m_kline.e_time.date;
	}
	else if (DIMENSION_HOUR == m_info.dimension)	//小时线维数也只能为1
	{
		SimpleTime pre_time1 = date_time.time;
		pre_time1.AddSec(-1);
		SimpleTime pre_time2 = m_kline.e_time.time;
		pre_time2.AddSec(-1);
		return date_time.date != m_kline.e_time.date || pre_time1.hour != pre_time2.hour;
	}
	else if (DIMENSION_MINUTE == m_info.dimension && 1 < m_info.dimen_cnt)	//只能从1分钟线去合成
	{
		if (push_kline_num_ == 0)	//第一根K线
		{
			push_kline_num_ = 1;
			return true;
		}
		push_kline_num_++;
		if (date_time.date != m_kline.e_time.date)	//下一个交易日，重新开始
		{
			push_kline_num_ = 1;
			return true;
		}
		if (push_kline_num_ > m_info.dimen_cnt)
		{
			push_kline_num_ = 1;
			return true;
		}
		
		return false;
	}
	else
	{
		APP_LOG(LOG_LEVEL_ERROR) << "Kline merger on PushKline don't suport the dimension:("<<m_info.dimension<<", "<<m_info.dimen_cnt<<")";
		throw  "Kline merger on PushKline don't suport the dimension";
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

NightKlineMerger::NightKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl) : iKlineMerger(info, hdl)
{
	if (info.symbol.exchange == EXCHANGE_SHFE)
	{
		std::string fnd_home = Global::GetInstance()->fnd_home;
		std::string conf_path = fnd_home + "/config/TradingTime.ini";
		if (!QFile::exists(conf_path.c_str()))
		{
			APP_LOG(LOG_LEVEL_ERROR) << conf_path << " is not exist";
			throw (conf_path + " is not exist");
		}

		char code_prefix[3];
		code_prefix[2] = '\0';
		memcpy(code_prefix, info.symbol.instrument, 2);
		QSettings settings(conf_path.c_str(), QSettings::IniFormat);
		QString inst_str = settings.value("SHFE.DAY/InstrumentID").toString();
		QStringList insts = inst_str.split(",", QString::SkipEmptyParts);
		if (insts.contains(code_prefix))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "只支持合成夜盘的K线数据 : " << info.symbol.instrument;
			throw ("只支持合成夜盘的K线数据");
		}
	}
	else
	{
		APP_LOG(LOG_LEVEL_ERROR) << "只支持合成夜盘的K线数据";
		throw ("只支持合成夜盘的K线数据");
	}
}

void NightKlineMerger::PushTick(BaseTick* tick)
{

}

void NightKlineMerger::PushKline(Kline* kline)
{

}

}

