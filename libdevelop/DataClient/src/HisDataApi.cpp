#include "dataserver/HisDataApi.h"
#include "common/AppLog.h"

namespace itstation {
using namespace common;

HisDataApi::HisDataApi(const std::string& ip, int port, HisKlineSpi* spi) : is_init_(false), spi_(spi)
{
	tcp_client_ = new LibTcpClient(ip.c_str(), port, this);
}

HisDataApi::~HisDataApi(void)
{
}

void HisDataApi::OnReceive(TcpSocket *tcp_sock, char* buf, int len)
{
	if (len <= sizeof(RspHisKline)) { return; }
	RspHisKline* rsp_kline = (RspHisKline*)buf;
	if (rsp_kline->type != RSP_HIS_KLINE) { return; }

	int expect_len = sizeof(RspHisKline) + sizeof(Kline) * rsp_kline->num;
	if (expect_len != len) {
		APP_LOG(LOG_LEVEL_ERROR) << "unequal len: return " << len << " while except " << expect_len;
		return;
	}

	spi_->OnKline(rsp_kline);
}

bool HisDataApi::Init(std::string& err)
{
	if (is_init_) { return true; }
	is_init_ = tcp_client_->StartUp(err);
	return is_init_;
}

bool HisDataApi::RequestKline(const HisKlineRequtInfo& info, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	HisKlineRequt* req = (HisKlineRequt*)malloc(sizeof(HisKlineRequt) + sizeof(HisKlineRequtInfo));
	req->type = REQ_HIS_KLINE;
	req->num = 1;
	memcpy((char*)req + sizeof(HisKlineRequt), &info, sizeof(HisKlineRequtInfo));
	common::Locker locker(&lock_);
	if (!tcp_client_->Send((char*)req, sizeof(HisKlineRequt) + sizeof(HisKlineRequtInfo), err))
	{
		free(req);
		return false;
	}

	free(req);
	return true;
}

bool HisDataApi::RequestKline(const std::vector<HisKlineRequtInfo>& infos, std::string& err)
{
	if (!is_init_) { 
		err = "HisDataApi has not been init";
		return false; 
	}

	int len = sizeof(HisKlineRequt) + sizeof(HisKlineRequtInfo) * infos.size();
	HisKlineRequt* req = (HisKlineRequt*)malloc(len);
	req->type = REQ_HIS_KLINE;
	req->num = infos.size();
	char* cp_pos = (char*)req + sizeof(HisKlineRequt);
	for (int i=0; i<infos.size(); ++i)
	{
		memcpy(cp_pos, &infos[i], sizeof(HisKlineRequtInfo));
		cp_pos += sizeof(HisKlineRequtInfo);
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