#include "ProjectServer.h"
#include "Applog.h"
#include "EngineEnvironment.h"
#include "publicfun.h"
#include "../task/TaskInterfaceDef.h"
namespace itstation 
{
	CProjectServer* CProjectServer::m_pInstance = NULL;
	common::Mutex CProjectServer::m_mutex;
	CProjectServer::CGarbo CProjectServer::Garbo;

	CProjectServer::CProjectServer() 
	{ 
	} 
	CProjectServer::~CProjectServer()
	{
	}

	CProjectServer* CProjectServer::GetInstance() 
	{ 
		if(m_pInstance == NULL) 
		{ 
			common::MutexLocker lock(&m_mutex); //用MutexLocker实现线程安全

			if(m_pInstance == NULL) 
			{ 
				m_pInstance = new CProjectServer(); 
			} 
		} 
		return m_pInstance; 
	}

	void CProjectServer::initialize()
	{
		m_status = false;

		APP_LOG(common::Applog::LOG_INFO)<<"server initialize";
		config_file_path = EngineEnvironment::getInstance()->get_config_file_path();

		m_ptrNotify = new ClientSession(0);
		m_TcpServer = CreateTcpServer(99905, m_ptrNotify);
		m_TcpServer->StartUp();

		std::string ip, username, password, dbname;
		int dbport=0;
		PUBLIC_FUN::GetMysqlConnectSetting( ip, username, password, dbname, dbport );
		//cout << ip << " " << username << " " << password << " " << dbname << " " << dbport << endl;
	}

	void CProjectServer::startServer()
	{
		Start();
	}

	void CProjectServer::stopServer()
	{
		if ( m_ptrNotify )
		{
			delete m_ptrNotify;
			m_ptrNotify = NULL;
		}

		if ( m_TcpServer )
		{
			delete m_TcpServer;
			m_TcpServer = NULL;
		}

		m_status = true;
	}

	void CProjectServer::SendMessages( int type, session_ptr ptrSocket, const char* stream, TCP_MSG_HEAD &header )
	{
		//bool nRet=false;
		switch(type)
		{
		case 1:
			{
				m_TcpServer->SendMessages( ptrSocket, stream, header );
				break;				   
			}			

		case 2:
			{
				m_TcpClient->SendMessages( ptrSocket, stream, header );
				break;	
			}
		default:
			{
				break;
			}
		}

		//return nRet;
	}

	void CProjectServer::OnRun()
	{
		int cout = 0;
		while( !m_status )
		{
			//std::cout << cout++ << std::endl;
			q_sleep(1000);
		}
	}
}
