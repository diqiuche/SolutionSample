#ifndef ITSTATION_DATASERVER_TICK_TCP_SERVER_H_
#define ITSTATION_DATASERVER_TICK_TCP_SERVER_H_

#include "dataserver/DataServerStruct.h"
#include "network/TcpServer.h"
#include "common/SpinLock.h"
#include "dataserver/SymbolContainer.h"
#include "dataserver/Protocol.h"
#include <list>
#include <map>

namespace itstation {

using namespace common;

class TickRegister {
public:
	TickRegister() : tick_(NULL) {}

	void Append(TcpSocket* sock);
	void Remove(TcpSocket* sock);

	void Send(char* buf, int len);

	void SetTick(BaseTick* tick);
	void SendTick(TcpSocket* sock);

private:
	std::list<TcpSocket*> sockets_;
	SpinLock mutex_;
	BaseTick* tick_;
	SpinLock tick_mutex_;
};

typedef SymbolContainer<TickRegister> TickRegisterContainer;
//class TickTcpServer;
//
//class TickRequsetReader : public SocketReaderSpi {
//public:
//	TickRequsetReader(TickTcpServer* tcp_server) : tcp_server_(tcp_server) {}
//
//	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len);
//
//private:
//	TickTcpServer* tcp_server_;
//};

typedef std::map<TcpSocket*, std::list<Symbol> > TickThemeMap;

class TickTcpServer : public TcpServerConnSpi, public SocketReaderSpi
{
public:
	TickTcpServer(int port);
	virtual ~TickTcpServer(void);

	bool StartUp(std::string& err);

	void Subscribe(TcpSocket *tcp_sock, const Symbol& sym);
	void UnSubscribe(TcpSocket *tcp_sock, const Symbol& sym);

	void SendTick(const Symbol& sym, char* buf, int len);

private:
	virtual void OnAccept(TcpSocket* sock);
	virtual void OnDiscon(TcpSocket *tcp_sock);
	virtual void OnReceive(TcpSocket *tcp_sock, char* buf, int len);

	TickRegister* GetTickRegist(const Symbol& sym);
	void ClearRegist(TcpSocket *tcp_sock);

	bool InitContainer(std::string& err);

private:
	TcpServer* tcp_server_;

	TickThemeMap theme_map_;	//每个socket维护一个订阅列表，当socket断开时能快速取消订阅该socket的所以行情
	TickRegisterContainer* stock_container_;
	TickRegisterContainer* index_container_;
	TickRegisterContainer* fufure_container_;

	std::list<TcpSocket*> all_symbol_sockets_;	//订阅所有品种的socket列表

	//TickRequsetReader* tick_req_reader_;
	SpinLock mutex_;
};

}

#endif

