#ifndef ITSTATION_DATASERVER_KLINE_GENERATOR_H_
#define ITSTATION_DATASERVER_KLINE_GENERATOR_H_

#include <vector>
#include "dataserver/DataServerStruct.h"
#include "common/Global.h"


namespace itstation {

struct TradeSectionTime {
	SimpleTime begin;
	SimpleTime end;
};

class iKlineMerger;

class KlineMergerHandler {
public:
	virtual void OnAddKline(iKlineMerger* merger, const Kline* kline) = 0;
	virtual void OnUpdateKline(iKlineMerger* merger, const Kline* kline) = 0;
};

class iKlineMerger
{
public:
	iKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl = NULL);
	~iKlineMerger() {}

	void Reset(const Kline& kline, float vol)
	{
		m_kline = kline;
		m_sum_vol = vol;
	}

	virtual void PushTick(BaseTick* tick) = 0;
	virtual void PushKline(Kline* kline) = 0;
	virtual void AdjustEndTime() = 0;
	Kline* GetKline()
	{
		if (m_kline.volume < -1) { return NULL; }
		return &m_kline;
	}
	Kline* GetPreKline()
	{
		if (m_pre_kline.volume < -1) { return NULL; }
		return &m_pre_kline;
	}
	Kline* GetDayKline()
	{
		if (m_day_kline.volume < -1) { return NULL; }
		return &m_day_kline;
	}
	KlineInfo& GetInfo()
	{
		return m_info;
	}

	void InitSectionTime();	//从配置文件中读取交易信息
	void SetSectionTime(const std::vector<TradeSectionTime>& section_time);

protected:
	std::string ExchangeName(ExchangeIdType exchange);
	void ClenaTick(BaseTick* tick);

protected:
	KlineInfo m_info;
	Kline m_kline;
	Kline m_pre_kline;
	Kline m_day_kline;
	std::vector<TradeSectionTime> m_section_time;
	float m_sum_vol;
	KlineMergerHandler* m_hdl;
};

class CommonKlineMerger : public iKlineMerger
{
public:
	CommonKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl);
	~CommonKlineMerger() {}

	virtual void PushTick(BaseTick* tick);
	virtual void PushKline(Kline* kline);
	virtual void AdjustEndTime() { NextEndTime(); }

private:
	void NextStartTime(SimpleDateTime* date_time);
	void NextEndTime();
	void UpdateWithTick(BaseTick* tick);
	void AddWithTick(BaseTick* tick);
	bool IsNewKline(const SimpleDateTime& date_time);

private:
	int push_kline_num_;
};

//针对夜盘单独处理
class NightKlineMerger : public iKlineMerger
{
public:
	NightKlineMerger(const KlineInfo& info, KlineMergerHandler* hdl);
	~NightKlineMerger() {}

	virtual void PushTick(BaseTick* tick);
	virtual void PushKline(Kline* kline);
	virtual void AdjustEndTime() {}
};

}

#endif
