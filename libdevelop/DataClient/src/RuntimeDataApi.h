#ifndef ITS_DATASERVER_RUNTIME_DATA_API_H_
#define ITS_DATASERVER_RUNTIME_DATA_API_H_

#include <vector>
#include "network/LibTcpClient.h"
#include "dataserver/Protocol.h"
#include "common/SpinLock.h"

namespace itstation {

class RuntimeDataSpi {
public:
	virtual void OnTick(BaseTick* tick) = 0;
};

class RuntimeDataApi : public SocketReaderSpi
{
public:
	RuntimeDataApi(const std::string& ip, int port, RuntimeDataSpi* spi);
	~RuntimeDataApi(void);

	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len);
	bool Init(std::string& err);

	bool SubTick(const Symbol& sym, std::string& err);
	bool SubTick(const std::vector<Symbol>& syms, std::string& err);

	bool UnSubTick(const Symbol& sym, std::string& err);
	bool UnSubTick(const std::vector<Symbol>& syms, std::string& err);

	void SetSpi(RuntimeDataSpi* spi) { spi_ = spi; }

private:
	common::SpinLock lock_;
	LibTcpClient* tcp_client_;
	bool is_init_;

	RuntimeDataSpi* spi_;
};

}
#endif

