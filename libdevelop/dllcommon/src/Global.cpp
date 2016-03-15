#include "Global.h"
#include <iostream>

std::string g_eyegle_home = "";

Global* Global::m_instance = NULL;

Global::Global() 
{
}

const Global* Global::GetInstance() {
	if (NULL == m_instance) {
		m_instance = new Global();
		m_instance->Init();
	}

	return m_instance; 
}

void Global::Init() {
	char* fnd_home_c = getenv("FND_HOME");
	if(NULL == fnd_home_c) { 
		throw "ITS_HOME环境变量未设置";
	}
	fnd_home = fnd_home_c;
}
