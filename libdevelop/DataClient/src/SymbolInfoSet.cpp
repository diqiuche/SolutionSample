#include "dataserver/SymbolInfoSet.h"
#include <iostream>
#include <fstream>
#include "common/Global.h"

namespace itstation{

SymbolInfoSet* SymbolInfoSet::inst_ = NULL;

SymbolInfoSet::SymbolInfoSet(void) : is_init_(false)
{
}

SymbolInfoSet::~SymbolInfoSet(void)
{
}

bool SymbolInfoSet::Init(std::string& err)
{
	if (is_init_) { return true; }
	DBTableFactory* factory = DBTableFactory::GetInstance();
	if (!factory->Init(Global::GetInstance()->fnd_home))
	{
		err = "DBTableFactory初始化失败";
		return false;
	}

	//InstrumentInfoTable future_table(factory->GetEnv(), "FutureInfo.db");
	//if (!future_table.OpenDb())
	//{
	//	err = "打开数据库失败";
	//	return false;
	//}

	//if (!future_table.Foreach(this))
	//{
	//	err = "查询数据库失败";
	//	return false;
	//}

	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1411"));
	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1412"));
	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1503"));
	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1412"));
	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1503"));
	future_symbols_.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1506"));
	for (int i=0; i<future_symbols_.size(); ++i)
	{
		char name_key[16] = {0};
		sprintf(name_key, "%c%c_%s", future_symbols_[i].product, future_symbols_[i].exchange, future_symbols_[i].instrument);
		symbol_names_[name_key] = future_symbols_[i].instrument;
	}

	SymbolExTable code_table(factory->GetEnv(), "StockCodeTable.db");
	if (!code_table.OpenDb())
	{
		err = "打开数据库失败";
		return false;
	}

	if (!code_table.Foreach(this))
	{
		err = "查询数据库失败";
		return false;
	}

	is_init_ = true;
	return true;
}

void SymbolInfoSet::Foreach(const std::string* key, const InstrumentInfoData* data)
{
	//std::cout << data->symbol.exchange << "  " << data->symbol.instrument << std::endl;
	//static std::ofstream ofs("tmp.log");
	//ofs << data->symbol.exchange << "  " << data->symbol.instrument << std::endl;

	Symbol sym;
	memcpy(sym.instrument, data->symbol.instrument, sizeof(InstrumentIdType));
	if (strcmp(data->symbol.exchange, "CFFEX") == 0) { sym.exchange = EXCHANGE_CFFEX; }
	else if (strcmp(data->symbol.exchange, "DCE") == 0) { sym.exchange = EXCHANGE_DCE; }
	else if (strcmp(data->symbol.exchange, "CZCE") == 0) { sym.exchange = EXCHANGE_CZCE; }
	else if (strcmp(data->symbol.exchange, "SHFE") == 0) { sym.exchange = EXCHANGE_SHFE; }
	else { return; }
	sym.product = PRODUCT_FUTURE;

	symbol_names_[sym.Str()] = data->instrument_name.data;
	future_symbols_.push_back(sym);
}

void SymbolInfoSet::Foreach(const std::string* key, const SymbolEx* data)
{
	//static std::ofstream ofs("tmp.log");
	//ofs << (*key) << "  " << data->instrument << "  " << data->name << endl;

	symbol_names_[*key] = data->name;

	Symbol *sym = (Symbol*)data;
	if (sym->product == PRODUCT_STOCK)
	{
		stock_a_symbols_.push_back(*sym);
	} 
	else if (sym->product == PRODUCT_INDEX)
	{
		stock_index_symbols_.push_back(*sym);
	} 
}

SymbolInfoSet* SymbolInfoSet::GetInstance()
{
	if (NULL == inst_) {
		inst_ = new SymbolInfoSet();
	}

	return inst_;
}

}
