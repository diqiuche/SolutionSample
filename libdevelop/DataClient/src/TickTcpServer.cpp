#include "dataserver/TickTcpServer.h"
#include <algorithm>
#include <common/AppLog.h>
#include "dataserver/SymbolInfoSet.h"

namespace itstation {

void TickRegister::Append(TcpSocket* sock)
{
	Locker locker(&mutex_);
	std::list<TcpSocket*>::iterator iter = std::find(sockets_.begin(), sockets_.end(), sock);
	if (iter == sockets_.end())
	{
		sockets_.push_back(sock);
	}
}

void TickRegister::Remove(TcpSocket* sock)
{
	Locker locker(&mutex_);
	std::list<TcpSocket*>::iterator iter = std::find(sockets_.begin(), sockets_.end(), sock);
	if (iter != sockets_.end())
	{
		sockets_.erase(iter);
	}
}

void TickRegister::Send(char* buf, int len)
{
	Locker locker(&mutex_);
	std::list<TcpSocket*>::iterator iter;
	for (iter = sockets_.begin(); iter != sockets_.end(); ++iter)
	{
		std::string err;
		if (!(*iter)->Send(buf, len, err))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "seng tick error: " << err;
		}
	}
}

void TickRegister::SetTick(BaseTick* tick) 
{
	Locker locker(&tick_mutex_);
	if (NULL != tick_) { free(tick_); }
	tick_ = tick;
}

void TickRegister::SendTick(TcpSocket* sock) 
{ 
	Locker locker(&tick_mutex_);
	if (NULL != tick_)
	{
		int len = 0;
		switch (tick_->symbol.product)
		{
		case PRODUCT_STOCK:
			len = sizeof(StockTick);
			break;
		case PRODUCT_INDEX:
			len = sizeof(IndexTick);
			break;
		case PRODUCT_FUTURE:
			len = sizeof(FutureTick);
			break;
		default:
			return;
		}

		std::string err;
		if (!sock->Send((char*)tick_, len, err))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "seng tick error: " << err;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

TickTcpServer::TickTcpServer(int port)
{
	tcp_server_ = new TcpServer(port, this);
	tcp_server_->SetConnSpi(this);
	//tick_req_reader_ = new TickRequsetReader(this);
	//SetReaderSpi(tick_req_reader_);

	stock_container_ = new StockContainer<TickRegister>(SearchIndex::FIELD_A);
	index_container_ = new StockContainer<TickRegister>(SearchIndex::FIELD_IDX);
	fufure_container_ = new FutureContainer<TickRegister>();
}

TickTcpServer::~TickTcpServer(void)
{
	delete tcp_server_;

	delete stock_container_;
	delete index_container_;
	delete fufure_container_;
}

bool TickTcpServer::StartUp(std::string& err)
{
	if (!InitContainer(err)) { return false; }
	if (!tcp_server_->Init(err)) { return false; }
	return true;
}

void TickTcpServer::OnAccept(TcpSocket* sock)
{
	Locker locker(&mutex_);
	theme_map_[sock] = std::list<Symbol>();
}

TickRegister* TickTcpServer::GetTickRegist(const Symbol& sym)
{
	TickRegisterContainer* container;
	switch(sym.product)
	{
	case PRODUCT_STOCK:
		container = stock_container_;
		break;
	case PRODUCT_INDEX:
		container = index_container_;
		break;
	case PRODUCT_FUTURE:
		container = fufure_container_;
		break;
	default:
		return NULL;
	}

	TickRegister** tick_reg = container->Data(sym);
	if (tick_reg == NULL || *tick_reg == NULL) { return NULL; }
	return *tick_reg;
}

void TickTcpServer::OnReceive(TcpSocket *tcp_sock, char* buf, int len)
{
	if (len <= sizeof(ProtocolHead)) { return; }

	ProtocolHead* head = (ProtocolHead*)buf;
	switch (head->type)
	{
	case REQ_RUN_TICK:
		{
			if (len <= sizeof(RunTickRequt)) { return; }
			RunTickRequt* run_tick_req = (RunTickRequt*)buf;
			if (run_tick_req->num <= 0 || len != run_tick_req->num * sizeof(Symbol)
				+ sizeof(RunTickRequt)) 
			{ 
				return; 
			}

			Symbol *symbols = (Symbol*)(buf  + sizeof(RunTickRequt));
			if (run_tick_req->is_sub)
			{
				for (int i=0; i < run_tick_req->num; ++i)
				{
					Subscribe(tcp_sock, symbols[i]);
				}
			} 
			else
			{
				for (int i=0; i < run_tick_req->num; ++i)
				{
					UnSubscribe(tcp_sock, symbols[i]);
				}
			}

		}

	default:
		break;
	}
}

void TickTcpServer::ClearRegist(TcpSocket *tcp_sock)
{
	TickThemeMap::iterator iter = theme_map_.find(tcp_sock);
	if (iter == theme_map_.end()) { return; }
	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter;
	for (sym_iter = sym_list.begin(); sym_iter != sym_list.end(); ++sym_iter)
	{
		TickRegister* tick_reg = GetTickRegist(*sym_iter);
		if (tick_reg == NULL) { continue; }
		tick_reg->Remove(tcp_sock);
	}

	sym_list.clear();
}

void TickTcpServer::OnDiscon(TcpSocket *tcp_sock)
{
	Locker locker(&mutex_);
	ClearRegist(tcp_sock);

	TickThemeMap::iterator iter = theme_map_.find(tcp_sock);
	if (iter != theme_map_.end())
	{
		theme_map_.erase(iter);
	}

	std::list<TcpSocket*>::iterator sock_iter = std::find(all_symbol_sockets_.begin(), all_symbol_sockets_.end(), tcp_sock);
	if (sock_iter != all_symbol_sockets_.end())
	{
		all_symbol_sockets_.erase(sock_iter);
	}
}

void TickTcpServer::Subscribe(TcpSocket *tcp_sock, const Symbol& sym)
{
	Locker locker(&mutex_);
	TickThemeMap::iterator iter = theme_map_.find(tcp_sock);
	if (iter == theme_map_.end()) { return; }	//未连接的socket

	if (sym == SYMBOL_FOR_ALL)
	{
		ClearRegist(tcp_sock);	//为了防止重复订阅，需要先取消其它的订阅
		std::list<TcpSocket*>::iterator sock_iter = std::find(all_symbol_sockets_.begin(), all_symbol_sockets_.end(), tcp_sock);
		if (sock_iter == all_symbol_sockets_.end())
		{
			all_symbol_sockets_.push_back(tcp_sock);
		}
		return;
	}
	
	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter = std::find(sym_list.begin(), sym_list.end(), sym);
	if (sym_iter != sym_list.end()) { return; }  //不重复订阅

	TickRegister* tick_reg = GetTickRegist(sym);
	if (tick_reg == NULL) { return; }

	tick_reg->Append(tcp_sock);
	sym_list.push_back(sym);
	tick_reg->SendTick(tcp_sock);
}

void TickTcpServer::UnSubscribe(TcpSocket *tcp_sock, const Symbol& sym)
{
	Locker locker(&mutex_);
	TickThemeMap::iterator iter = theme_map_.find(tcp_sock);
	if (iter == theme_map_.end()) { return; }	//未连接的socket

	if (sym == SYMBOL_FOR_ALL)
	{
		ClearRegist(tcp_sock);	//为了防止重复订阅，需要先取消其它的订阅
		std::list<TcpSocket*>::iterator sock_iter = std::find(all_symbol_sockets_.begin(), all_symbol_sockets_.end(), tcp_sock);
		if (sock_iter != all_symbol_sockets_.end())
		{
			all_symbol_sockets_.erase(sock_iter);
		}
		return;
	}

	std::list<Symbol>& sym_list = iter->second;
	std::list<Symbol>::iterator sym_iter = std::find(sym_list.begin(), sym_list.end(), sym);
	if (sym_iter == sym_list.end()) { return; }  //未订阅
	sym_list.erase(sym_iter);

	TickRegister* tick_reg = GetTickRegist(sym);
	if (tick_reg == NULL) { return; }
	tick_reg->Remove(tcp_sock);
}

void TickTcpServer::SendTick(const Symbol& sym, char* buf, int len)
{
	Locker locker(&mutex_);
	TickRegister* tick_reg = GetTickRegist(sym);
	if (tick_reg == NULL) { return; }

	tick_reg->Send(buf, len);
	std::list<TcpSocket*>::iterator iter;
	for (iter = all_symbol_sockets_.begin(); iter != all_symbol_sockets_.end(); ++iter)
	{
		std::string err;
		if (!(*iter)->Send(buf, len, err))
		{
			APP_LOG(LOG_LEVEL_ERROR) << "seng tick error: " << err;
		}
	}

	BaseTick* tick = (BaseTick*)malloc(len);
	memcpy(tick, buf, len);
	tick_reg->SetTick(tick);
}

bool TickTcpServer::InitContainer(std::string& err)
{
	SymbolInfoSet* sym_info = SymbolInfoSet::GetInstance();
	if (!sym_info->Init(err))
	{
		return false;
	}

	const std::vector<Symbol>& stks = sym_info->StockASymbols();
	for (int i=0; i<stks.size(); ++i)
	{
		TickRegister** tick_reg = stock_container_->Data(stks[i]);
		if (tick_reg == NULL) { continue; }
		*tick_reg = new TickRegister();
	}

	const std::vector<Symbol>& idxs = sym_info->StockIndexSymbols();
	for (int i=0; i<idxs.size(); ++i)
	{
		TickRegister** tick_reg = index_container_->Data(idxs[i]);
		if (tick_reg == NULL) { continue; }
		*tick_reg = new TickRegister();
	}

	//TODO:
	std::vector<Symbol> futures;
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1411"));
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1412"));
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "IF1503"));
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1412"));
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1503"));
	futures.push_back(Symbol(PRODUCT_FUTURE, EXCHANGE_CFFEX, "TF1506"));
	for (int i=0; i<futures.size(); ++i)
	{
		TickRegister** tick_reg = fufure_container_->Data(futures[i]);
		if (NULL != tick_reg)
		{
			*tick_reg = new TickRegister();
		}
	}
}

}
