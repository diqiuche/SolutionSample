#ifndef ITSTATION_DATASERVER_BARS_H_
#define ITSTATION_DATASERVER_BARS_H_

#include "dataserver/Array.h"
#include <common/DateTime.h>
#include <common/AppLog.h>

namespace itstation {
namespace common {

struct Bar
{
	DateTime begin;
	DateTime end;
	float open;
	float high;
	float low;
	float close;
	float volume;
};

struct BaseBars 
{
	enum {
		DEFAULT_UNIT_LEN = 720
	};

	Array<DateTime>* begin;
	Array<DateTime>* end;
	Array<float>* open;
	Array<float>* high;
	Array<float>* low;
	Array<float>* close;
	Array<float>* volume;

	SpinLock mutex_;

	BaseBars(int unit_len = DEFAULT_UNIT_LEN, bool fixed = false) 
	{
		if (fixed)
		{
			begin = new FixedArray<DateTime>(unit_len);
			end = new FixedArray<DateTime>(unit_len);
			open = new FixedArray<float>(unit_len);
			high = new FixedArray<float>(unit_len);
			low = new FixedArray<float>(unit_len);
			close = new FixedArray<float>(unit_len);
			volume = new FixedArray<float>(unit_len);
		}
		else
		{
			begin = new ExtendArray<DateTime>(unit_len);
			end = new ExtendArray<DateTime>(unit_len);
			open = new ExtendArray<float>(unit_len);
			high = new ExtendArray<float>(unit_len);
			low = new ExtendArray<float>(unit_len);
			close = new ExtendArray<float>(unit_len);
			volume = new ExtendArray<float>(unit_len);
		}
	}

	void Update(const Bar& bar)
	{
		Locker locker(&mutex_);
		begin->Update(bar.begin);
		end->Update(bar.end);
		open->Update(bar.open);
		high->Update(bar.high);
		low->Update(bar.low);
		close->Update(bar.close);
		volume->Update(bar.volume);
	}

	void Append(const Bar& bar)
	{
		Locker locker(&mutex_);
		begin->Append(bar.begin);
		end->Append(bar.end);
		open->Append(bar.open);
		high->Append(bar.high);
		low->Append(bar.low);
		close->Append(bar.close);
		volume->Append(bar.volume);
	}
};

//冗余设计
class Bars {
public:
	enum {
		MAX_PRICE_LEVEL = 10
	};

	Symbol symbol;	
	KlineDimensionType Dimension;
	unsigned short DimCount;
	DateTime UpdateTime;		
	float LastPrice;
	float NewVolume;
	float PreClosePrice;
	float TodayOpen;
	float TodayHigh;
	float TodayLow;
	float TodayVolume;	
	float TodayAmount;	

	float BidPrice[MAX_PRICE_LEVEL];	//买1~5价
	float AskPrice[MAX_PRICE_LEVEL];	//卖1~5价
	float BidVolume[MAX_PRICE_LEVEL];	//1~5档的买盘委托量
	float AskVolume[MAX_PRICE_LEVEL];	//1~5档的卖盘委托量

	//以下字段只对期货有效
	float Position;			//增仓
	float TotalPosition;	//总持仓
	float PreSettlePrice;	//昨结算价
	bool IsAdded;

	Array<DateTime>& Begin;
	Array<DateTime>& End;
	Array<float>& Open;
	Array<float>& High;
	Array<float>& Low;
	Array<float>& Close;
	Array<float>& Volume;

	explicit Bars(BaseBars* base_bars)
		: Begin(*base_bars->begin)
		, End(*base_bars->end)
		, Open(*base_bars->open)
		, High(*base_bars->high)
		, Low(*base_bars->low)
		, Close(*base_bars->close)
		, Volume(*base_bars->volume)
		, mutex_(base_bars->mutex_)
		, IsAdded(false)
	{
		memset(BidPrice, 0, sizeof(float) * MAX_PRICE_LEVEL);
		memset(AskPrice, 0, sizeof(float) * MAX_PRICE_LEVEL);
		memset(BidVolume, 0, sizeof(float) * MAX_PRICE_LEVEL);
		memset(AskVolume, 0, sizeof(float) * MAX_PRICE_LEVEL);
		NewVolume = 0;
		TodayVolume = 0;
		Position = 0;
		TotalPosition = 0;
	}

	static void ChangeDateTime(DateTime& src, const SimpleDateTime& des)
	{
		src.m_date.year = des.date.year;
		src.m_date.month = des.date.month;
		src.m_date.day = des.date.day;
		src.m_time.hour = des.time.hour;
		src.m_time.minute = des.time.minute;
		src.m_time.sec = des.time.sec;
		src.m_time.milsec = des.time.mil_sec;
	}

	static float UnitToPrice(PriceMoveUnit price, float base_p)
	{
		if (price == NAN_PRICE_UNIT) { return FLOAT_NAN; }
		return base_p + 0.01 * price;
	}

	SpinLock& GetMutex() { return mutex_; }

	void UpdateTick(BaseTick* tick)
	{
		if (symbol != tick->symbol) { 
			APP_LOG(LOG_LEVEL_ERROR) << "different symbol, except " << symbol.instrument
				<< " while recive " << tick->symbol.instrument;
			return; 
		}

		Locker locker(&mutex_);
		ChangeDateTime(UpdateTime, tick->date_time);

		if (TodayVolume < 0.000001) {
			NewVolume = 0;
		} 
		else {
			NewVolume = tick->volume - TodayVolume;
		}

		LastPrice = tick->last_price;
		PreClosePrice = tick->pre_close;
		TodayOpen = tick->today_open;
		TodayHigh = tick->today_high;
		TodayLow = tick->today_low;
		TodayVolume = tick->volume;
		TodayAmount = tick->amount;

		if (symbol.product == PRODUCT_STOCK)
		{
			StockTick* stock_tick = (StockTick*)tick;
			for (int i=0; i<PRICE_LEVEL_NUM; ++i)
			{
				BidPrice[i] = UnitToPrice(stock_tick->buy_price[i], tick->last_price);
				AskPrice[i] = UnitToPrice(stock_tick->sell_price[i], tick->last_price);
			}
			memcpy(BidVolume, stock_tick->buy_volume, PRICE_LEVEL_NUM * sizeof(float));
			memcpy(AskVolume, stock_tick->sell_volume, PRICE_LEVEL_NUM * sizeof(float));
		}
		else if (symbol.product == PRODUCT_FUTURE)
		{
			FutureTick* future_tick = (FutureTick*)tick;

			if (TotalPosition < 0.000001) {
				Position = 0;
			} 
			else {
				Position = future_tick->position - TotalPosition;
			}

			TotalPosition = future_tick->position;
			PreSettlePrice = future_tick->pre_settlement;
			BidPrice[0] = future_tick->buy_price;
			AskPrice[0] = future_tick->sell_price;
			BidVolume[0] = future_tick->buy_volume;
			AskVolume[0] = future_tick->sell_volume;
		}
	}

private:
	SpinLock& mutex_;
};

}
}
#endif
