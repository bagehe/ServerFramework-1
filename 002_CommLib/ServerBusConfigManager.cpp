
#include "SectionConfig.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"

#include "ServerBusConfigManager.h"

using namespace ServerLib;

#define SERVER_BUS_CONFIG_FILE		"../conf/GameServer.tcm"	//Ĭ�϶��� conf/GameServer.tcm �ж�ȡ

ServerBusConfigManager::ServerBusConfigManager()
{

}

ServerBusConfigManager::~ServerBusConfigManager()
{
	m_vServerBusConfig.clear();
}

void ServerBusConfigManager::Reset()
{
	m_vServerBusConfig.clear();
}

//����ͨ��BUS������
int ServerBusConfigManager::LoadServerBusConfig()
{
	CSectionConfig stConfigFile;
	int iRet = stConfigFile.OpenFile(SERVER_BUS_CONFIG_FILE);
	if (iRet)
	{
		LOGERROR("Failed to load server bus config file: %s!\n", SERVER_BUS_CONFIG_FILE);
		return iRet;
	}

	//����ServerBus��������
	int iTotalBusNum = 0;
	stConfigFile.GetItemValue("ServerBus", "ServerBusNum", iTotalBusNum);

	//���ؾ���ServerBus������
	int iIsServerEnd = 0;
	char szServerBusID[SERVER_BUSID_LEN] = { 0 };
	char szBusIPAddr[SERVER_BUSID_LEN] = { 0 };
	int iBusPort = 0;

	//Bus����������Key
	char szIsServerEndKey[SERVER_BUSID_LEN] = { 0 };
	char szServerBusKey[SERVER_BUSID_LEN] = { 0 };
	char szBusIPAddrKey[SERVER_BUSID_LEN] = { 0 };
	char szBusPortKey[SERVER_BUSID_LEN] = { 0 };

	m_vServerBusConfig.clear();

	ServerBusConfig stOneConfig;
	for (int i = 0; i<iTotalBusNum; ++i)
	{
		//��װBus��Key
		SAFE_SPRINTF(szIsServerEndKey, SERVER_BUSID_LEN - 1, "IsServerEnd_%d", i);
		SAFE_SPRINTF(szServerBusKey, SERVER_BUSID_LEN - 1, "ServerBusID_%d", i);
		SAFE_SPRINTF(szBusIPAddrKey, SERVER_BUSID_LEN - 1, "ServerBusIP_%d", i);
		SAFE_SPRINTF(szBusPortKey, SERVER_BUSID_LEN - 1, "ServerBusPort_%d", i);

		//��ȡ�����е�ֵ
		stConfigFile.GetItemValue("ServerBus", szIsServerEndKey, iIsServerEnd, 0);
		stConfigFile.GetItemValue("ServerBus", szServerBusKey, szServerBusID, sizeof(szServerBusID) - 1, "0.0.0.0");
		stConfigFile.GetItemValue("ServerBus", szBusIPAddrKey, szBusIPAddr, sizeof(szBusIPAddr) - 1, "0.0.0.0");
		stConfigFile.GetItemValue("ServerBus", szBusPortKey, iBusPort, 0);

		stOneConfig.Reset();

		//�Ƿ�Server��
		stOneConfig.bIsServerEnd = iIsServerEnd;

		//����ͨ��Bus��ID
		stOneConfig.ullServerBusID = GetBusID(szServerBusID);

		//��װʵ��ͨ��Bus�ĵ�ַ,��ʽΪ IP:PORT
		sprintf(stOneConfig.szBusAddr, "%s:%d", szBusIPAddr, iBusPort);

		m_vServerBusConfig.push_back(stOneConfig);
	}

	stConfigFile.CloseFile();

	return 0;
}

//��ȡͨ��Bus����
const ServerBusConfig* ServerBusConfigManager::GetServerBus(const char* szServerBusID)
{
	if (!szServerBusID)
	{
		return NULL;
	}

	uint64_t ullServerBusID = GetBusID(szServerBusID);
	for (unsigned i = 0; i<m_vServerBusConfig.size(); ++i)
	{
		if (m_vServerBusConfig[i].ullServerBusID == ullServerBusID)
		{
			return &m_vServerBusConfig[i];
		}
	}

	return NULL;
}

//��ȡ����ͨ��BUS������
const std::vector<ServerBusConfig>& ServerBusConfigManager::GetAllServerBus()
{
	return m_vServerBusConfig;
}
