#ifndef ITSTATION_DATASERVER_SEARCH_INDEX_H_
#define ITSTATION_DATASERVER_SEARCH_INDEX_H_

#include "dataserver/DataServerStruct.h"

namespace itstation {

//¹ÉÆ±Ë÷Òý
class SearchIndex
{
public:
	typedef enum {
		FIELD_SH_A,
		FIELD_SZ_A,
		FIELD_SH_B,
		FIELD_SZ_B,
		FIELD_SH_IDX,
		FIELD_SZ_IDX,
		FIELD_SH_FUND,
		FIELD_SZ_FUND,
		FIELD_SH_BOND,
		FIELD_SZ_BOND,
		FIELD_A,
		FIELD_B,
		FIELD_IDX,
		FIELD_FUND,
		FIELD_BOND,
		FIELD_SH,
		FIELD_SZ,
		FIELD_ALL, 
		FIELD_OTHER,
		FIELD_NOME
	}SearchField;	//ËÑË÷Óò

	static int IndexSize(SearchField field);
	static int Index(SearchField field, const Symbol& symbol);

private:
	SearchIndex(void) {}
	~SearchIndex(void) {}
};

}

#endif

