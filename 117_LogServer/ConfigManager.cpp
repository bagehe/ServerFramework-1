
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "AppDef.hpp"

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
	//����ͨ��BUS
	int iRet = m_stBusConfigMgr.LoadServerBusConfig();
	if (iRet)
	{
		LOGERROR("Failed to load server bus config, ret %d\n", iRet);
		return -iRet;
	}

	//����LogDB
	iRet = m_stLogDBConfigManager.LoadDBConfig(MOFANGDBINFO_CONFIG_FILE);
	if (iRet)
	{
		TRACESVR("Failed to load log db config, ret %d\n", iRet);
		return iRet;
	}

	return 0;
}

//��ȡͨ��BUS������
ServerBusConfigManager& ConfigManager::GetBusConfigMgr()
{
	return m_stBusConfigMgr;
}

//��ȡLogDB����
const OneDBInfo* ConfigManager::GetLogDBConfigByIndex(int iDBIndex)
{
	return m_stLogDBConfigManager.GetDBConfigByIndex(iDBIndex);
}
	