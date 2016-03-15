#ifndef ITSTATION_DATASERVER_SUB_KLINE_API_H_
#define ITSTATION_DATASERVER_SUB_KLINE_API_H_

#include <string>
#include <vector>
#include <list>
#include "common/SpinLock.h"
#include "dataserver/HisDataApi.h"
#include "dataserver/RuntimeDataApi.h"
#include "dataserver/Bars.h"
#include "dataserver/KlineMerger.h"
#include "dataserver/SymbolContainer.h"

namespace itstation {
using namespace common;

class SubBarsSpi {
public:
	virtual void OnUpdateKline(const common::Bars *bars, bool is_new) = 0;
	virtual void OnInitKline(const common::Bars *bars) = 0;
};

class RuntimeBarsObj : public KlineMergerHandler {
public:
	friend class BarsReqObj;
	RuntimeBarsObj(const HisKlineRequtInfo& info, SubBarsSpi* spi, int unit_len = BaseBars::DEFAULT_UNIT_LEN, bool fixed = false);
	~RuntimeBarsObj();

	virtual void OnAddKline(iKlineMerger* merger, const Kline* kline);
	virtual void OnUpdateKline(iKlineMerger* merger, const Kline* kline);

	void InitHisKline(RspHisKline* kline);
	void UpdateTick(BaseTick* tick);

private:
	static void KlineToBar(Bar& bar, const Kline& kline);
private:
	BaseBars *base_bars_;
	Bars *bars_;
	iKlineMerger* kline_merger_;
	bool is_kline_init_;
	SubBarsSpi* spi_;
	HisKlineRequtInfo req_info_;

	SpinLock mutex_;
	std::vector<BaseTick*> temp_ticks_;
};
//以Symbol为单位的K线订阅列表，用以避免重复订阅
class BarsReqObj
{
public:
	BarsReqObj() {}
	~BarsReqObj();

	Bars*  Append(const HisKlineRequtInfo& info, SubBarsSpi* spi, int unit_len, bool fixed);
	void Remove(const HisKlineRequtInfo& info, SubBarsSpi* spi);
	void Remove(Bars* bars);
	int Size();

	void InitHisKline(RspHisKline* kline);
	void UpdateTick(BaseTick* tick);

private:
	std::list<RuntimeBarsObj*> run_bars_obj_;
	SpinLock mutex_;
};

typedef SymbolContainer<BarsReqObj> BarsReqObjContainer;

class SubBarsApi : public RuntimeDataSpi, public HisKlineSpi
{
public:
	SubBarsApi();
	~SubBarsApi();

	static SubBarsApi* GetInstance() { 
		if (NULL == self_)
		{
			self_ = new SubBarsApi();
		}
		return self_; 
	}

	virtual void OnTick(BaseTick* tick);
	virtual void OnKline(RspHisKline* kline);

	bool Init(const std::string& conf_path, std::string& err);

	std::string GetLastError() { return error_; }
	Bars* SubBars(const HisKlineRequtInfo& info, SubBarsSpi* spi, int unit_len = BaseBars::DEFAULT_UNIT_LEN, bool fixed = false);
	bool UnSubBars(const HisKlineRequtInfo& info, SubBarsSpi* spi);
	bool UnSubBars(Bars* bars);	//TODO:未测试

	bool SubBars(const std::vector<HisKlineRequtInfo>& infos, SubBarsSpi* spi, int unit_len = BaseBars::DEFAULT_UNIT_LEN, bool fixed = false);
	bool UnSubBars(const std::vector<HisKlineRequtInfo>& infos, SubBarsSpi* spi);

private:
	bool InitContainer(std::string& err);
	void FreeContainer();
	BarsReqObj* GetTickRegist(const Symbol& sym);
	RuntimeDataApi* GetRunApi(const Symbol& sym);

private:
	static SubBarsApi* self_;

	bool is_init_;
	RuntimeDataApi* wjf_api_;
	RuntimeDataApi* ctp_api_;
	HisDataApi* his_api_;
	std::string error_;

	BarsReqObjContainer* stock_container_;
	BarsReqObjContainer* index_container_;
	BarsReqObjContainer* fufure_container_;
};

}
#endif
