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

	//��ȡNameDB����
	const OneDBInfo* GetNameDBConfigByIndex(int iDBIndex);

private:
	ConfigManager();

private:
	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;

	//NameDB��Ϣ����
	DBConfigManager m_stNameDBConfigManager;
};

