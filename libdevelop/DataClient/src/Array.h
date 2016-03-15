#ifndef ITSTATION_DATASERVER_ARRAY_H_
#define ITSTATION_DATASERVER_ARRAY_H_

#include "dataserver/DataServerStruct.h"
#include "common/SpinLock.h"
#include <assert.h>

namespace itstation {
namespace common {

template <typename T>
class Array
{
	enum {
		DEFAULT_UNIT_LEN = 720
	};
public:
	Array(int unit_len = DEFAULT_UNIT_LEN) : cur_pos_(-1), unit_len_(unit_len) {
		assert(unit_len > 0);
		max_len_ = unit_len * 2;
		data_ = new T[max_len_];
	}
	virtual ~Array() { 
		Locker locker(&mutex_);
		delete data_; 
	}

	int UnitLen() { return unit_len_; }

	virtual int Len() { 
		return cur_pos_ + 1; 
	}

	void Clear() {
		cur_pos_ = -1;
	}

	//以Data的方式遍历数列，需要获取锁
	//void ObtainLock() { mutex_.Lock(); }
	//void ReleaseLock() { mutex_.Unlock(); }
	common::SpinLock& GetMutex() { return mutex_; }
	T* Data() { return data_; }

	T GetPoint(int pos) {
		Locker locker(&mutex_);
		if (pos < 0 || pos > cur_pos_) { return T(); }
		return data_[pos];
	}

	T operator[](int idx) {
		Locker locker(&mutex_);
		int pos = cur_pos_-idx;
		if (pos < 0) { return T(); }
		return data_[pos];
	}

	void Update(const T& value) {
		Locker locker(&mutex_);
		if (cur_pos_ < 0) { return; }
		data_[cur_pos_] = value;
	}

	void Append(const T& value) {
		Locker locker(&mutex_);
		cur_pos_++;
		if (cur_pos_ >= max_len_) { StretchPos(); }

		data_[cur_pos_] = value;
	}

	virtual bool IsFixed() = 0;

	virtual void StretchPos() = 0;

protected:
	common::SpinLock mutex_;
	T* data_;
	int cur_pos_;
	int max_len_;
	int unit_len_;
};

template <typename T>
class FixedArray : public Array<T>
{
public:
	FixedArray(int unit_len = DEFAULT_UNIT_LEN) : Array<T>(unit_len) {}
	virtual ~FixedArray() {}

	virtual bool IsFixed() { return true; }

private:
	virtual void StretchPos()
	{
		int half_len = max_len_ >> 1;
		memcpy(data_, data_ + half_len, sizeof(T) * half_len);
		cur_pos_ -= half_len;
	}
};

template <typename T>
class ExtendArray : public Array<T>
{
public:
	ExtendArray(int unit_len = DEFAULT_UNIT_LEN) : Array<T>(unit_len) {}
	virtual ~ExtendArray() {}

	virtual bool IsFixed() { return false; }

private:
	virtual void StretchPos()
	{
		T* new_data = new T[max_len_ + unit_len_];
		memcpy(new_data, data_, sizeof(T) * max_len_);
		delete data_;
		data_ = new_data;
		max_len_ += unit_len_;
	}
};

}
}
#endif
