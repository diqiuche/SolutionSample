#pragma once

#include <string>
#include <vector>
#include <map>
#include "marketapi/MarketDefine.h"
#include "db/DBTableFactory.h"
#include "dataserver/DataServerStruct.h"

namespace itstation{

using namespace marketapi;
using namespace common;

typedef BklyDB<std::string, InstrumentInfoData> InstrumentInfoTable;
typedef common::BklyDB<std::string, SymbolEx> SymbolExTable;

class SymbolInfoSet : public InstrumentInfoTable::RoundOver, public SymbolExTable::RoundOver
{
public:
	SymbolInfoSet(void);
	~SymbolInfoSet(void);

	bool Init(std::string& err);
	const std::vector<Symbol>& FutureSymbols() { return future_symbols_; }
	const std::vector<Symbol>& StockASymbols() { return stock_a_symbols_; }
	const std::vector<Symbol>& StockIndexSymbols() { return stock_index_symbols_; }
	std::map<std::string, std::string>& SymbolNames() { return symbol_names_; }

	static SymbolInfoSet* GetInstance();

private:
	virtual void Foreach(const std::string* key, const InstrumentInfoData* data);
	virtual void Foreach(const std::string* key, const SymbolEx* data);

private:
	static SymbolInfoSet* inst_;

	std::vector<Symbol> future_symbols_;
	std::vector<Symbol> stock_a_symbols_;
	std::vector<Symbol> stock_index_symbols_;
	std::map<std::string, std::string> symbol_names_;
	bool is_init_;
};

}


