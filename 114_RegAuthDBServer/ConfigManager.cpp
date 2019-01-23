
#include "AppDef.hpp"
#include "LogAdapter.hpp"

#include "ConfigManager.hpp"

using namespace ServerLib;

ConfigManager* ConfigManager::Instance()
{
	static ConfigManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new ConfigManager;
	}

	return pInstance;
}

ConfigManager::ConfigManager()
{

}

ConfigManager::~ConfigManager()
{
	m_stBusConfigMgr.Reset();
}

//��������
int ConfigManager::LoadAllConfig()
{
	//����ͨ��BUS����
	int iRet = m_stBusConfigMgr.LoadServerBusConfig();
	if (iRet)
	{
		LOGERROR("Failed to load server bus config, ret %d\n", iRet);
		return iRet;
	}

	//����RegAuthDB����
	iRet = m_stRegAuthDBConfigManager.LoadDBConfig(REGAUTHDBINFO_CONFIG_FILE);
	if (iRet)
	{
		TRACESVR("Failed to load RegAuth db info config, ret %d\n", iRet);
		return iRet;
	}

	//����uniqUinDB����
	iRet = m_stUniqUinDBConfigManager.LoadDBConfig(UNIQUINDBINFO_CONFIG_FILE);
	if (iRet)
	{
		TRACESVR("Failed to load uniquin db info config, ret %d\n", iRet);
		return iRet;
	}

	return 0;
}

//��ȡͨ��BUS������
ServerBusConfigManager& ConfigManager::GetBusConfigMgr()
{
	return m_stBusConfigMgr;
}

//��ȡRegAuthDB����
const OneDBInfo* ConfigManager::GetRegAuthDBConfigByIndex(int iDBIndex)
{
	return m_stRegAuthDBConfigManager.GetDBConfigByIndex(iDBIndex);
}

//��ȡUniqUinDB����
const OneDBInfo* ConfigManager::GetUniqUinDBConfigByIndex(int iDBIndex)
{
	return m_stUniqUinDBConfigManager.GetDBConfigByIndex(iDBIndex);
}
