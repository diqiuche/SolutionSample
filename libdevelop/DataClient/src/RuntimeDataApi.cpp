#include "dataserver/RuntimeDataApi.h"
#include <iostream>

namespace itstation {

RuntimeDataApi::RuntimeDataApi(const std::string& ip, int port, RuntimeDataSpi* spi) 
	: is_init_(false), spi_(spi)
{
	tcp_client_ = new LibTcpClient(ip.c_str(), port, this);
}

RuntimeDataApi::~RuntimeDataApi(void)
{
	delete tcp_client_;
}

void RuntimeDataApi::OnReceive(TcpSocket *tcp_sock, char* buf, int len)
{
	BaseTick* base_tick = (BaseTick*)buf;
	if (NULL != spi_)
	{
		spi_->OnTick(base_tick);
	}

	//switch (base_tick->symbol.product)
	//{
	//case PRODUCT_STOCK:
	//	{
	//		StockTick* stock_tick = (StockTick*)base_tick;
	//		std::cout << stock_tick->date_time.Str()<<" "<<stock_tick->symbol.instrument<<" p:"<<stock_tick->last_price
	//			<<" v:"<<stock_tick->volume<<" a:"<<stock_tick->amount<<std::endl<<std::endl;
	//	}
	//	break;

	//case PRODUCT_INDEX:
	//	{
	//		IndexTick* index_tick = (IndexTick*)base_tick;
	//		std::cout << index_tick->date_time.Str()<<" "<<index_tick->symbol.instrument<<" p:"<<index_tick->last_price
	//			<<" v:"<<index_tick->volume<<" a:"<<index_tick->amount<<std::endl<<std::endl;
	//	}
	//	break;

	//default:
	//	break;
	//}
}

bool RuntimeDataApi::Init(std::string& err)
{
	if (is_init_) { return true; }
	is_init_ = tcp_client_->StartUp(err);
	return is_init_;
}

bool RuntimeDataApi::SubTick(const Symbol& sym, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	RunTickRequt* req = (RunTickRequt*)malloc(sizeof(RunTickRequt) + sizeof(Symbol));
	req->type = REQ_RUN_TICK;
	req->num = 1;
	req->is_sub = true;
	memcpy((char*)req + sizeof(RunTickRequt), &sym, sizeof(Symbol));
	common::Locker locker(&lock_);
	if (!tcp_client_->Send((char*)req, sizeof(RunTickRequt) + sizeof(Symbol), err))
	{
		free(req);
		return false;
	}

	free(req);
	return true;
}

bool RuntimeDataApi::SubTick(const std::vector<Symbol>& syms, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	int len = sizeof(RunTickRequt) + sizeof(Symbol) * syms.size();
	RunTickRequt* req = (RunTickRequt*)malloc(len);
	req->type = REQ_RUN_TICK;
	req->num = syms.size();
	req->is_sub = true;
	char* cp_pos = (char*)req + sizeof(RunTickRequt);
	for (int i=0; i<syms.size(); ++i)
	{
		memcpy(cp_pos, &syms[i], sizeof(Symbol));
		cp_pos += sizeof(Symbol);
	}
	common::Locker locker(&lock_);
	if (!tcp_client_->Send((char*)req, len, err))
	{
		free(req);
		return false;
	}

	free(req);
	return true;
}

bool RuntimeDataApi::UnSubTick(const Symbol& sym, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	RunTickRequt* req = (RunTickRequt*)malloc(sizeof(RunTickRequt) + sizeof(Symbol));
	req->type = REQ_RUN_TICK;
	req->num = 1;
	req->is_sub = false;
	memcpy((char*)req + sizeof(RunTickRequt), &sym, sizeof(Symbol));
	common::Locker locker(&lock_);
	if (!tcp_client_->Send((char*)req, sizeof(RunTickRequt) + sizeof(Symbol), err))
	{
		free(req);
		return false;
	}

	free(req);
	return true;
}

bool RuntimeDataApi::UnSubTick(const std::vector<Symbol>& syms, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	int len = sizeof(RunTickRequt) + sizeof(Symbol) * syms.size();
	RunTickRequt* req = (RunTickRequt*)malloc(len);
	req->type = REQ_RUN_TICK;
	req->num = syms.size();
	req->is_sub = false;
	char* cp_pos = (char*)req + sizeof(RunTickRequt);
	for (int i=0; i<syms.size(); ++i)
	{
		memcpy(cp_pos, &syms[i], sizeof(Symbol));
		cp_pos += sizeof(Symbol);
	}
	common::Locker locker(&lock_);
	if (!tcp_client_->Send((char*)req, len, err))
	{
		free(req);
		return false;
	}

	free(req);
	return true;
}

}
