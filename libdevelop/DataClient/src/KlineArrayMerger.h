#ifndef ITSTATION_DATASERVER_KLINE_ARRAY_GENERATOR_H_
#define ITSTATION_DATASERVER_KLINE_ARRAY_GENERATOR_H_

#include <vector>
#include "dataserver/KlineMerger.h"


namespace itstation {

class KlineArrayMerger : KlineMergerHandler
{
public:
	KlineArrayMerger(const KlineInfo& info);
	~KlineArrayMerger(void);

	virtual void OnAddKline(iKlineMerger* merger, const Kline* kline);
	virtual void OnUpdateKline(iKlineMerger* merger, const Kline* kline);

	void PushTick(BaseTick* tick);
	void PushKline(Kline* kline);

	std::vector<Kline>& Data() { return klines_; }

private:
	iKlineMerger* kline_merger_;
	std::vector<Kline> klines_;
	SimpleDateTime cur_data_time_;
};

}
#endif