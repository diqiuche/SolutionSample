#ifndef ITSTATION_DATASERVER_SYMBOL_CONTAINER_H_
#define ITSTATION_DATASERVER_SYMBOL_CONTAINER_H_

#include <map>
#include "dataserver/DataServerStruct.h"
#include "dataserver/SearchIndex.h"

namespace itstation {

template<class T>
class SymbolContainer
{
public:
	typedef void (*EachHandle)(T* data, void* user_data);

	SymbolContainer(void) {}
	virtual ~SymbolContainer(void) {}

	virtual int Size() = 0;
	virtual T** Data(const Symbol& sym) = 0;
	virtual void ForEach(EachHandle hdl, void* user_data = NULL) = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class StockContainer : public SymbolContainer<T>
{
public:
	StockContainer(SearchIndex::SearchField search_idx) : SymbolContainer<T>(), search_idx_(search_idx)
	{
		size_ = SearchIndex::IndexSize(search_idx);
		data_ = new T*[size_];
		memset(data_, 0, sizeof(T*) * size_);
	}

	virtual ~StockContainer(void)
	{
		delete [] data_;
	}

	virtual int Size()
	{
		return size_;
	}

	virtual T** Data(const Symbol& sym)
	{
		int idx = SearchIndex::Index(search_idx_, sym);
		if (idx < 0 || idx >= size_)
		{
			return NULL;
		}
		return &data_[idx];
	}

	virtual void ForEach(EachHandle hdl, void* user_data = NULL)
	{
		for (int i=0; i<size_; ++i)
		{
			if (NULL == data_[i]) { continue; }
			hdl(data_[i], user_data);
		}
	}

private:
	T** data_;
	SearchIndex::SearchField search_idx_;
	int size_;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class FutureContainer : public SymbolContainer<T>
{
public:
	FutureContainer() : SymbolContainer<T>()
	{
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1411")] = NULL;
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1412")] = NULL;
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1503")] = NULL;
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1412")] = NULL;
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1503")] = NULL;
		data_[Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1506")] = NULL;
	}

	virtual ~FutureContainer(void)
	{
	}

	virtual int Size()
	{
		return data_.size();
	}

	virtual T** Data(const Symbol& sym)
	{
		std::map<Symbol, T*>::iterator iter = data_.find(sym);
		if (iter == data_.end()) { return NULL; }

		return &data_[sym];
	}

	virtual void ForEach(EachHandle hdl, void* user_data = NULL)
	{
		std::map<Symbol, T*>::iterator iter;
		for (iter = data_.begin(); iter != data_.end(); ++iter)
		{
			if (iter->second == NULL) { continue;}
			hdl(iter->second, user_data);
		}
	}

private:
	std::map<Symbol, T*> data_;
};

}

#endif

