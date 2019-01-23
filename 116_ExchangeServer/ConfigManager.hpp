#pragma once

#include "ServerBusConfigManager.h"
#include "DBConfigManager.h"

class CConfigManager
{
public:

	static CConfigManager* Instance();
	~CConfigManager();

    int LoadAllConf();
public:

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

	//��ȡCardNoDB����
	const OneDBInfo* GetCardNoDBConfigByIndex(int iDBIndex);

private:
	CConfigManager();

private:

	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;

	//CardNoDB��Ϣ����
	DBConfigManager m_stCardNoDBConfigManager;
};
