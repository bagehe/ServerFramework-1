#pragma once

#include "ServerBusConfigManager.h"
#include "DBConfigManager.h"

//RoleDB ���ù�����
class ConfigManager
{
public:
	static ConfigManager* Instance();
	~ConfigManager();

	//��������
	int LoadAllConfig();

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

	//��ȡRoleDB����
	const OneDBInfo* GetRoleDBConfigByIndex(int iDBIndex);

private:
	ConfigManager();

private:
	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;

	//ROLEDB���ù�����
	DBConfigManager m_stRoleDBConfigManager;
};

