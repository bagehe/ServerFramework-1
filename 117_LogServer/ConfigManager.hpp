#pragma once

#include "ServerBusConfigManager.h"
#include "DBConfigManager.h"

//���ù�����
class ConfigManager
{
public:
	static ConfigManager* Instance();
	~ConfigManager();

	//��������
	int LoadAllConfig();

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

	//��ȡMofangDB����
	const OneDBInfo* GetLogDBConfigByIndex(int iDBIndex);

private:
	ConfigManager();

private:
	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;

	//LogDB��Ϣ����
	DBConfigManager m_stLogDBConfigManager;
};

