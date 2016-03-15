#pragma once

#include <string>
//#include "E:\prj\iTStation\project\DataServer\Stockdrv.h"
#include "dataserver/DataServerStruct.h"
//#include "3rd/bars.h"


using namespace std;

namespace itstation {

class DataClientApi
{
public:
	DataClientApi(void);
	~DataClientApi(void);

	//void SubscribeData(const SubscribeObject& inst, Bars** bars);

private:
	void ReadFile();
	string m_day_kline_path;
	string m_min_kline_path;
};

}



