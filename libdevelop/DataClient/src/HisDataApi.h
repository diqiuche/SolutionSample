#ifndef ITSTATION_DATASERVER_HIS_DATA_API_H_
#define ITSTATION_DATASERVER_HIS_DATA_API_H_

#include <string>
#include <vector>
#include "dataserver/Protocol.h"
#include "network/LibTcpClient.h"
#include "common/SpinLock.h"


namespace itstation {

class HisKlineSpi {
public:
	virtual void OnKline(RspHisKline* kline) = 0;
};

class HisDataApi : public SocketReaderSpi
{
public:
	HisDataApi(const std::string& ip, int port, HisKlineSpi* spi);
	~HisDataApi(void);

	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len);

	bool Init(std::string& err);
	bool RequestKline(const HisKlineRequtInfo& info, std::string& err);
	bool RequestKline(const std::vector<HisKlineRequtInfo>& infos, std::string& err);

	void SetSpi(HisKlineSpi* spi) { spi_ = spi; }

private:
	common::SpinLock lock_;
	LibTcpClient* tcp_client_;
	bool is_init_;
	HisKlineSpi* spi_;
};

}

#endif