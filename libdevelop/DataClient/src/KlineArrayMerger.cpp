#include "dataserver/KlineArrayMerger.h"

namespace itstation {
KlineArrayMerger::KlineArrayMerger(const KlineInfo& info)
{
	kline_merger_ = new CommonKlineMerger(info, this);
	//TODO:从SymbolInfoSet初始化来设置
	kline_merger_->InitSectionTime();
}


KlineArrayMerger::~KlineArrayMerger(void)
{
	delete kline_merger_;
}

void KlineArrayMerger::OnAddKline(iKlineMerger* merger, const Kline* kline)
{
	if (cur_data_time_ > kline->e_time) { return; }
	klines_.push_back(*kline);
}

void KlineArrayMerger::OnUpdateKline(iKlineMerger* merger, const Kline* kline)
{
	klines_.back() = *kline;
}

void KlineArrayMerger::PushTick(BaseTick* tick)
{
	cur_data_time_ = tick->date_time;
	kline_merger_->PushTick(tick);
}

void KlineArrayMerger::PushKline(Kline* kline)
{
	cur_data_time_ = kline->e_time;
	kline_merger_->PushKline(kline);
}

}