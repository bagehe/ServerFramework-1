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

	//��ȡRegAuthDB����
	const OneDBInfo* GetRegAuthDBConfigByIndex(int iDBIndex);

	//��ȡUniqUinDB����
	const OneDBInfo* GetUniqUinDBConfigByIndex(int iDBIndex);

private:
	ConfigManager();

private:
	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;

	//RegAuthDB����
	DBConfigManager m_stRegAuthDBConfigManager;

	//UniqUinDB���ã����ڷ���uin
	DBConfigManager m_stUniqUinDBConfigManager;
};

