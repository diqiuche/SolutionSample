#include "dataserver/SubBarsApi.h"
#include "dataserver/SymbolInfoSet.h"
#include "common/AppLog.h"
#include <QSettings>
#include <QFile>
#include <algorithm>

namespace itstation {
using namespace common;

RuntimeBarsObj::RuntimeBarsObj(const HisKlineRequtInfo& req_info, SubBarsSpi* spi, int unit_len, bool fixed)
	: is_kline_init_(false), spi_(spi), req_info_(req_info)
{
	base_bars_ = new BaseBars(unit_len, fixed);
	bars_ = new Bars(base_bars_);
	bars_->symbol = req_info.sym;
	bars_->Dimension = req_info.dim;
	bars_->DimCount = req_info.dim_count;

	//TODO:根据symbol判断是否夜盘，创建适当的K线合成对象
	KlineInfo kline_info;
	kline_info.symbol = req_info.sym;
	kline_info.dimension = req_info.dim;
	kline_info.dimen_cnt = req_info.dim_count;
	kline_merger_ = new CommonKlineMerger(kline_info, this);
	kline_merger_->InitSectionTime();
}

RuntimeBarsObj::~RuntimeBarsObj()
{
	delete kline_merger_;
	delete bars_;
	delete base_bars_;
}

void RuntimeBarsObj::KlineToBar(Bar& bar, const Kline& kline)
{
	Bars::ChangeDateTime(bar.begin, kline.b_time);
	Bars::ChangeDateTime(bar.end, kline.e_time);
	bar.open = kline.open;
	bar.high = kline.high;
	bar.low = kline.low;
	bar.close = kline.close;
	bar.volume = kline.volume;
}

void RuntimeBarsObj::OnAddKline(iKlineMerger* merger, const Kline* kline)
{
	Bar bar;
	KlineToBar(bar, *kline);
	base_bars_->Append(bar);	
	bars_->IsAdded = true;
	if (is_kline_init_ && spi_ != NULL) { spi_->OnUpdateKline(bars_, true); }
}

void RuntimeBarsObj::OnUpdateKline(iKlineMerger* merger, const Kline* kline)
{
	Bar bar;
	KlineToBar(bar, *kline);
	base_bars_->Update(bar);
	bars_->IsAdded = false;
	if (is_kline_init_ && spi_ != NULL) { spi_->OnUpdateKline(bars_, false); }
}

void RuntimeBarsObj::InitHisKline(RspHisKline* kline)
{
	if (kline->info != req_info_) { return; }
	if (is_kline_init_) { return; }

	float sum_vol = 0;
	SimpleDateTime now(time(NULL));
	for (int i=0; i<kline->num; ++i)
	{
		Bar bar;
		KlineToBar(bar, kline->data[i]);
		base_bars_->Append(bar);
		if (kline->data[i].b_time.date == now.date) {
			sum_vol += kline->data[i].volume;
		}
	}

	if (kline->num > 0)
	{
		kline_merger_->Reset(kline->data[kline->num - 1], sum_vol);
		kline_merger_->AdjustEndTime();
	}

	Locker locker(&mutex_);
	for (int i=0; i<temp_ticks_.size(); ++i)
	{
		BaseTick* tick = temp_ticks_[i];
		DateTime update_time;
		Bars::ChangeDateTime(update_time, tick->date_time);
		if (update_time > bars_->UpdateTime)
		{
			kline_merger_->PushTick(tick);
			bars_->UpdateTick(tick);
		}
		delete tick;
	}
	temp_ticks_.clear();

	is_kline_init_ = true;
	spi_->OnInitKline(bars_);
}

void RuntimeBarsObj::UpdateTick(BaseTick* tick)
{
	Locker locker(&mutex_);
	if (!is_kline_init_)
	{
		BaseTick* cpy_tick = NULL;
		switch (tick->symbol.product)
		{
		case PRODUCT_STOCK:
			cpy_tick = new StockTick(*((StockTick*)tick));
			break;
		case PRODUCT_INDEX:
			cpy_tick = new IndexTick(*((IndexTick*)tick));
			break;
		case PRODUCT_FUTURE:
			cpy_tick = new FutureTick(*((FutureTick*)tick));
			break;
		default:
			return;
		}
		temp_ticks_.push_back(cpy_tick);
		return;
	}

	DateTime update_time;
	Bars::ChangeDateTime(update_time, tick->date_time);
	if (update_time > bars_->UpdateTime)
	{
		bars_->UpdateTick(tick);
		kline_merger_->PushTick(tick);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
BarsReqObj::~BarsReqObj()
{
	std::list<RuntimeBarsObj*>::iterator iter;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		RuntimeBarsObj *obj = *iter;
		delete obj;
	}
	run_bars_obj_.clear();
}

Bars*  BarsReqObj::Append(const HisKlineRequtInfo& info, SubBarsSpi* spi, int unit_len, bool fixed)
{
	Locker locker(&mutex_);
	std::list<RuntimeBarsObj*>::iterator iter;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		if (info == (*iter)->req_info_ && spi == (*iter)->spi_)
		{
			return (*iter)->bars_;
		}
	}

	RuntimeBarsObj* obj = new RuntimeBarsObj(info, spi, unit_len, fixed);
	run_bars_obj_.push_back(obj);
	return obj->bars_;
}

void BarsReqObj::Remove(const HisKlineRequtInfo& info, SubBarsSpi* spi)
{
	Locker locker(&mutex_);
	std::list<RuntimeBarsObj*>::iterator iter;
	bool is_exist = false;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		RuntimeBarsObj *obj = *iter;
		if (info == obj->req_info_ && spi == obj->spi_)
		{
			delete obj;
			run_bars_obj_.erase(iter);
			is_exist = true;
			break;
		}
	}

	if (!is_exist)
	{
		APP_LOG(LOG_LEVEL_WARN) << "UnSubBars nothing";
	}
}

void BarsReqObj::Remove(Bars* bars)
{
	Locker locker(&mutex_);
	std::list<RuntimeBarsObj*>::iterator iter;
	bool is_exist = false;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		RuntimeBarsObj *obj = *iter;
		if (bars == obj->bars_)
		{
			delete obj;
			run_bars_obj_.erase(iter);
			is_exist = true;
			break;
		}
	}

	if (!is_exist)
	{
		APP_LOG(LOG_LEVEL_WARN) << "UnSubBars nothing";
	}
}

int BarsReqObj::Size()
{
	Locker locker(&mutex_);
	return run_bars_obj_.size();
}

void BarsReqObj::InitHisKline(RspHisKline* kline)
{
	Locker locker(&mutex_);
	std::list<RuntimeBarsObj*>::iterator iter;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		(*iter)->InitHisKline(kline);
	}
}

void BarsReqObj::UpdateTick(BaseTick* tick)
{
	Locker locker(&mutex_);
	std::list<RuntimeBarsObj*>::iterator iter;
	for (iter = run_bars_obj_.begin(); iter != run_bars_obj_.end(); ++iter)
	{
		(*iter)->UpdateTick(tick);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
SubBarsApi* SubBarsApi::self_ = NULL;

SubBarsApi::SubBarsApi() : is_init_(false), wjf_api_(NULL), ctp_api_(NULL), his_api_(NULL)
{
	stock_container_ = new StockContainer<BarsReqObj>(SearchIndex::FIELD_A);
	index_container_ = new StockContainer<BarsReqObj>(SearchIndex::FIELD_IDX);
	fufure_container_ = new FutureContainer<BarsReqObj>();
}

SubBarsApi::~SubBarsApi()
{
	if (NULL != wjf_api_) { delete wjf_api_; }
	if (NULL != ctp_api_) { delete ctp_api_; }
	if (NULL != his_api_) { delete his_api_; }

	FreeContainer();
	delete stock_container_;
	delete index_container_;
	delete fufure_container_;
}

bool SubBarsApi::Init(const std::string& conf_path, std::string& err)
{
	if (is_init_) { return true; }

	if (!InitContainer(err))
	{
		return false;
	}

	if (!QFile::exists(conf_path.c_str()))
	{
		err = "";
		err += conf_path + " is not exist";
		return false;
	}

	if (NULL == wjf_api_)
	{
		QSettings settings(conf_path.c_str(), QSettings::IniFormat);
		bool is_open = settings.value("WJFServer/open").toBool();
		if (is_open)
		{
			std::string run_ip = settings.value("WJFServer/ip").toString().toStdString();
			int run_port = settings.value("WJFServer/port").toInt();
			if (run_ip == "" || run_port <= 0)
			{
				err = "invalid ip or port of WJFServer on config setting: ";
				err += conf_path;
				return false;
			}
			wjf_api_ = new RuntimeDataApi(run_ip, run_port, this);
			if (!wjf_api_->Init(err)) { return false; }
		}
	}

	if (NULL == ctp_api_)
	{
		QSettings settings(conf_path.c_str(), QSettings::IniFormat);
		bool is_open = settings.value("CTPServer/open").toBool();
		if (is_open)
		{
			std::string run_ip = settings.value("CTPServer/ip").toString().toStdString();
			int run_port = settings.value("CTPServer/port").toInt();
			if (run_ip == "" || run_port <= 0)
			{
				err = "invalid ip or port of CTPServer on config setting: ";
				err += conf_path;
				return false;
			}
			ctp_api_ = new RuntimeDataApi(run_ip, run_port, this);
			if (!ctp_api_->Init(err)) { return false; }
		}
	}

	if (NULL == his_api_)
	{
		QSettings settings(conf_path.c_str(), QSettings::IniFormat);
		std::string his_ip = settings.value("HisDataApi/ip").toString().toStdString();
		int his_port = settings.value("HisDataApi/port").toInt();
		if (his_ip == "" || his_port <= 0)
		{
			err = "invalid ip or port of HisDataApi on config settint: ";
			err += conf_path;
			return false;
		}
		his_api_ = new HisDataApi(his_ip, his_port, this);
	}
	if (!his_api_->Init(err)) { return false; }

	is_init_ = true;
	return true;
}

void SubBarsApi::OnTick(BaseTick* tick)
{
	BarsReqObj* obj = GetTickRegist(tick->symbol);
	if (obj == NULL) {
		APP_LOG(LOG_LEVEL_ERROR) << "receive tick not in symbol table: "<<tick->symbol.exchange << " " << tick->symbol.instrument;
		return;
	}

	if (obj->Size() == 0)
	{
		if (tick->symbol.product = PRODUCT_FUTURE)
		{
			if (ctp_api_!= NULL && !ctp_api_->UnSubTick(tick->symbol, error_))
			{
				APP_LOG(LOG_LEVEL_ERROR) << "UnSubTick error: " << error_;
				return;
			}
		}
		else
		{
			if (wjf_api_!= NULL && !wjf_api_->UnSubTick(tick->symbol, error_))
			{
				APP_LOG(LOG_LEVEL_ERROR) << "UnSubTick error: " << error_;
				return;
			}
		}
	}

	obj->UpdateTick(tick);
}

void SubBarsApi::OnKline(RspHisKline* kline)
{
	BarsReqObj* obj = GetTickRegist(kline->info.sym);
	if (obj == NULL) {
		APP_LOG(LOG_LEVEL_ERROR) << "receive kline not in symbol table: "<<kline->info.sym.exchange << " " << kline->info.sym.instrument;
		return;
	}

	if (obj->Size() > 0)
	{
		obj->InitHisKline(kline);
	}
}

Bars* SubBarsApi::SubBars(const HisKlineRequtInfo& info, SubBarsSpi* spi, int unit_len, bool fixed)
{
	APP_LOG(LOG_LEVEL_INFO) << "SubBars: " << info.sym.instrument << "_" << info.sym.exchange << "  "
		<< "(" << info.dim << ", " << info.dim_count << ")";
	if (!is_init_) {
		error_ = "api has not init";
		return NULL;
	}

	BarsReqObj* obj = GetTickRegist(info.sym);
	if (obj == NULL) 
	{ 
		error_ = "the symbol is not in the symbol table";
		APP_LOG(LOG_LEVEL_ERROR) << "SubBars error: the symbol is not in the symbol table";
		return NULL; 
	}

	Bars* bars = obj->Append(info, spi, unit_len, fixed);
	if (!his_api_->RequestKline(info, error_))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "SubBars error: " << error_;
		obj->Remove(info, spi);
		return NULL;
	}

	if (obj->Size() == 1)
	{
		RuntimeDataApi* run_api = GetRunApi(info.sym);
		if (NULL != run_api && !run_api->SubTick(info.sym, error_))
		{
			obj->Remove(info, spi);
			APP_LOG(LOG_LEVEL_ERROR) << "SubTick error: " << error_;
			return NULL;
		}
	}
	return bars;
}

bool SubBarsApi::SubBars(const std::vector<HisKlineRequtInfo>& infos, SubBarsSpi* spi, int unit_len, bool fixed)
{
	if (!is_init_) {
		error_ = "api has not init";
		return false;
	}

	std::vector<Symbol> syms;
	std::vector<HisKlineRequtInfo> kline_infos;
	for (int i=0; i<infos.size(); ++i)
	{
		BarsReqObj* obj = GetTickRegist(infos[i].sym);
		if (obj == NULL) 
		{ 
			APP_LOG(LOG_LEVEL_ERROR) << "SubBars error: the symbol is not in the symbol table";
			continue;
		}

		obj->Append(infos[i], spi, unit_len, fixed);
		kline_infos.push_back(infos[i]);
		if (obj->Size() == 0) { syms.push_back(infos[i].sym); }
	}

	if (!his_api_->RequestKline(kline_infos, error_))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "SubBars error: " << error_;
		for (int i=0; i<kline_infos.size(); ++i)
		{
			BarsReqObj* obj = GetTickRegist(kline_infos[i].sym);
			obj->Remove(kline_infos[i], spi);
		}
		return false;
	}

	if (syms.size() == 0) { return true; }
	RuntimeDataApi* run_api = GetRunApi(syms[0]);
	if (NULL != run_api && !run_api->SubTick(syms, error_))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "SubTick error: " << error_;
		for (int i=0; i<kline_infos.size(); ++i)
		{
			BarsReqObj* obj = GetTickRegist(kline_infos[i].sym);
			obj->Remove(kline_infos[i], spi);
		}
		return false;
	}

	return true;
}

bool SubBarsApi::UnSubBars(const HisKlineRequtInfo& info, SubBarsSpi* spi)
{
	APP_LOG(LOG_LEVEL_INFO) << "UnSubBars: " << info.sym.instrument << "_" << info.sym.exchange << "  "
		<< "(" << info.dim << ", " << info.dim_count << ")";

	if (!is_init_) {
		error_ = "api has not init";
		return false;
	}

	BarsReqObj* obj = GetTickRegist(info.sym);
	if (obj == NULL) 
	{ 
		error_ = "the symbol is not in the symbol table";
		APP_LOG(LOG_LEVEL_ERROR) << "UnSubBars error: the symbol is not in the symbol table";
		return false; 
	}

	obj->Remove(info, spi);
	if (obj->Size() == 0)
	{
		RuntimeDataApi* run_api = GetRunApi(info.sym);
		if (NULL != run_api && !run_api->UnSubTick(info.sym, error_))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "UnSubTick error: " << error_;
			return false;
		}
	}
	return true;
}

bool SubBarsApi::UnSubBars(Bars* bars)
{
	APP_LOG(LOG_LEVEL_INFO) << "UnSubBars: " << bars->symbol.instrument << "_" << bars->symbol.exchange << "  "
		<< "(" << bars->Dimension << ", " << bars->DimCount << ")";

	if (!is_init_) {
		error_ = "api has not init";
		return false;
	}

	BarsReqObj* obj = GetTickRegist(bars->symbol);
	if (obj == NULL) 
	{ 
		error_ = "the symbol is not in the symbol table";
		APP_LOG(LOG_LEVEL_ERROR) << "UnSubBars error: the symbol is not in the symbol table";
		return false; 
	}

	Symbol sym = bars->symbol;
	obj->Remove(bars);
	if (obj->Size() == 0)
	{
		RuntimeDataApi* run_api = GetRunApi(sym);
		if (NULL != run_api && !run_api->UnSubTick(sym, error_))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "UnSubTick error: " << error_;
			bars = NULL;
			return false;
		}
	}
	bars = NULL;
	return true;
}

bool SubBarsApi::UnSubBars(const std::vector<HisKlineRequtInfo>& infos, SubBarsSpi* spi)
{
	if (!is_init_) {
		error_ = "api has not init";
		return false;
	}

	std::vector<Symbol> syms;
	for (int i=0; i<infos.size(); ++i)
	{
		BarsReqObj* obj = GetTickRegist(infos[i].sym);
		if (obj == NULL) 
		{ 
			APP_LOG(LOG_LEVEL_ERROR) << "SubBars error: the symbol is not in the symbol table";
			continue;
		}

		obj->Remove(infos[i], spi);
		if (obj->Size() == 1) { syms.push_back(infos[i].sym); }
	}

	if (syms.size() == 0) { return true; }
	RuntimeDataApi* run_api = GetRunApi(syms[0]);
	if (NULL != run_api && !run_api->UnSubTick(syms, error_))
	{
		APP_LOG(LOG_LEVEL_ERROR) << "UnSubTick error: " << error_;
		return false;
	}

	return true;
}

RuntimeDataApi* SubBarsApi::GetRunApi(const Symbol& sym)
{
	if (sym.product == PRODUCT_FUTURE)
	{
		return ctp_api_;
	}
	else if (sym.product == PRODUCT_STOCK || sym.product == PRODUCT_INDEX)
	{
		return wjf_api_;
	}
	else
	{
		return NULL;
	}
}

//TODO:析构释放内存，包括TickTcpServer
bool SubBarsApi::InitContainer(std::string& err)
{
	SymbolInfoSet* sym_info = SymbolInfoSet::GetInstance();
	if (!sym_info->Init(err))
	{
		return false;
	}

	const std::vector<Symbol>& stks = sym_info->StockASymbols();
	for (int i=0; i<stks.size(); ++i)
	{
		BarsReqObj** obj = stock_container_->Data(stks[i]);
		if (obj == NULL) { continue; }
		*obj = new BarsReqObj();
	}

	const std::vector<Symbol>& idxs = sym_info->StockIndexSymbols();
	for (int i=0; i<idxs.size(); ++i)
	{
		BarsReqObj** obj = index_container_->Data(idxs[i]);
		if (obj == NULL) { continue; }
		*obj = new BarsReqObj();
	}

	//TODO:
	const std::vector<Symbol>& futures = sym_info->FutureSymbols();
	for (int i=0; i<futures.size(); ++i)
	{
		BarsReqObj** obj = fufure_container_->Data(futures[i]);
		if (NULL != obj)
		{
			*obj = new BarsReqObj();
		}
	}

	return true;
}

void SubBarsApi::FreeContainer()
{
	SymbolInfoSet* sym_info = SymbolInfoSet::GetInstance();
	const std::vector<Symbol>& stks = sym_info->StockASymbols();
	for (int i=0; i<stks.size(); ++i)
	{
		BarsReqObj** obj = stock_container_->Data(stks[i]);
		if (obj == NULL) { continue; }
		if (*obj != NULL)
		{
			delete (*obj);
			*obj = NULL;
		}
	}

	const std::vector<Symbol>& idxs = sym_info->StockIndexSymbols();
	for (int i=0; i<idxs.size(); ++i)
	{
		BarsReqObj** obj = index_container_->Data(idxs[i]);
		if (obj == NULL) { continue; }
		if (*obj != NULL)
		{
			delete (*obj);
			*obj = NULL;
		}
	}

	//TODO:
	const std::vector<Symbol>& futures = sym_info->FutureSymbols();
	for (int i=0; i<futures.size(); ++i)
	{
		BarsReqObj** obj = fufure_container_->Data(futures[i]);
		if (NULL != obj)
		{
			if (*obj != NULL)
			{
				delete (*obj);
				*obj = NULL;
			}
		}
	}
}

BarsReqObj* SubBarsApi::GetTickRegist(const Symbol& sym)
{
	BarsReqObjContainer* container;
	switch(sym.product)
	{
	case PRODUCT_STOCK:
		container = stock_container_;
		break;
	case PRODUCT_INDEX:
		container = index_container_;
		break;
	case PRODUCT_FUTURE:
		container = fufure_container_;
		break;
	default:
		return NULL;
	}

	BarsReqObj** obj = container->Data(sym);
	if (obj == NULL || *obj == NULL) { return NULL; }
	return *obj;
}

}
