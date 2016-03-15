#include "dataserver/SearchIndex.h"
#include <QString>
#include <common/AppLog.h>

namespace itstation {
using namespace common;

int SearchIndex::IndexSize(SearchField field)
{
	switch (field){
	case FIELD_SH_A:
		return 3000;
	case FIELD_SZ_A:
		return 3100;
	case FIELD_SH_IDX:
		return 1000;
	case FIELD_SZ_IDX:
		return 1000;
	case FIELD_A:
		return 6100;
	case FIELD_IDX:
		return 2000;
	case FIELD_SH_B:
		return 0;
	case FIELD_SZ_B:
		return 0;
	case FIELD_B:
		return 0;
	case FIELD_SH:
		return 0;
	case FIELD_SZ:
		return 0;
	default:
		return 20000;
	}
}

int SearchIndex::Index(SearchField field, const Symbol& symbol)
{
	QString sym = symbol.instrument;
	QString pre_fix = sym.left(3);
	QString back_fix = sym.right(3);

	switch (field){
	case FIELD_SH_A:
		{
			if (symbol.exchange != EXCHANGE_SSE)
			{
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}

			if (pre_fix == "600") { return back_fix.toInt(); }
			else if (pre_fix == "601") { return back_fix.toInt() + 1000; }
			else if (pre_fix == "603") { return back_fix.toInt() + 2000; }
			else {
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}
		}
	case FIELD_SZ_A:
		{
			if (symbol.exchange != EXCHANGE_SZE)
			{
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}

			if (pre_fix == "000") { return back_fix.toInt(); }
			else if (pre_fix == "002") { return back_fix.toInt() + 1000; }
			else if (pre_fix == "300") { return back_fix.toInt() + 2000; }
			else if (pre_fix == "001") { 
				if (back_fix == "696") { return 3001; }
				else if (back_fix == "896") { return 3002; }
				else { return 3010; }
			}
			else {
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}
		}
	case FIELD_SH_IDX:
		{
			if (symbol.exchange != EXCHANGE_SSE || pre_fix != "000")
			{
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}
			return back_fix.toInt();
		}
	case FIELD_SZ_IDX:
		{
			if (symbol.exchange != EXCHANGE_SZE || pre_fix != "399")
			{
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}
			return back_fix.toInt();
		}
	case FIELD_A:
		{
			if (symbol.exchange == EXCHANGE_SSE) {
				return Index(FIELD_SH_A, symbol);
			}
			else if (symbol.exchange == EXCHANGE_SZE) {
				return 3000 + Index(FIELD_SZ_A, symbol);
			}
		}
	case FIELD_IDX:
		{
			if (symbol.exchange == EXCHANGE_SSE && pre_fix == "000")
			{
				return back_fix.toInt();
			} 
			else if (symbol.exchange == EXCHANGE_SZE && pre_fix == "399")
			{
				return back_fix.toInt() + 1000;
			}
			else {
				APP_LOG(LOG_LEVEL_ERROR) << "invalid argument";
				return -1;
			}
		}

	case FIELD_SH_B:
	case FIELD_SZ_B:
	case FIELD_B:
	case FIELD_SH:
	case FIELD_SZ:
	default:
		return 0;
	}
}

}
