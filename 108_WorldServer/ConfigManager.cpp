#include <fstream>
#include <sstream>

#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "SectionConfig.hpp"

#include "ConfigManager.hpp"

using namespace ServerLib;

CConfigManager* CConfigManager::Instance()
{
	static CConfigManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CConfigManager;
	}

	return pInstance;
}

CConfigManager::~CConfigManager()
{
	m_stBusConfigMgr.Reset();
	m_stBaseConfigManager.Reset();
}

CConfigManager::CConfigManager()
{

}

int CConfigManager::Initialize(bool bResume)
{
	//��ʱ����Ҫ������Ϣ���Զ������ؾ���
	/*
    //����������������
    int iRet = LoadZoneConf("../conf/zone_conf.txt");
    if (iRet != 0)
    {
        TRACESVR("failed to load zone config\n");
        return -1;
    }
	*/

	//����ͨ��BUS����
	int iRet = m_stBusConfigMgr.LoadServerBusConfig();
	if (iRet)
	{
		LOGERROR("Failed to load server bus config, ret %d\n", iRet);
		return -2;
	}

	//���ػ�������
	iRet = m_stBaseConfigManager.LoadAllConfig("../../../Common/config/");
	if (iRet)
	{
		LOGERROR("Failed to load server base config, ret %d\n", iRet);
		return -3;
	}

	//���ش�����������
	CSectionConfig stConfigFile;
	iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
	if (iRet)
	{
		LOGERROR("Failed to load server config file %s, ret %d\n", APP_CONFIG_FILE, iRet);
		return -4;
	}

	stConfigFile.GetItemValue("WorldStatus", "FullWorldNum", m_iFullWorldNum, (MAX_ROLE_OBJECT_NUMBER_IN_ZONE * 4 / 5));
	stConfigFile.GetItemValue("WorldStatus", "BusyWorldNum", m_iBusyWorldNum, (MAX_ROLE_OBJECT_NUMBER_IN_ZONE / 2));
	
	stConfigFile.CloseFile();

    return 0;
}

int CConfigManager::LoadZoneConf(const char* pszConfFile)
{
    ASSERT_AND_LOG_RTN_INT(pszConfFile);

    m_stZoneConfList.m_ushLen = 0;

    std::ifstream inFile(pszConfFile);
    if (!inFile.good())
    {
        TRACESVR("failed to open config file: %s\n", pszConfFile);

        return -1;
    }

    int i = 0;
    char szLine[1024];
    TZoneConf stZoneConfTmp;
    memset(&m_stZoneConfList, 0, sizeof(m_stZoneConfList));

    while (inFile.getline(szLine, sizeof(szLine)))
    {
        if (('\r' == szLine[0]) || (0 == strlen(szLine))) // ���У�vi����һ��ʱ��������\r
        {
            break;
        }

        std::stringstream strStream(szLine);

        strStream >> stZoneConfTmp.m_szHostIP
            >> stZoneConfTmp.m_ushPort
            >> stZoneConfTmp.m_ushZoneID
            >> stZoneConfTmp.m_szZoneName;

        int iZoneID = stZoneConfTmp.m_ushZoneID;
        if (iZoneID >= (int)MAX_ZONE_PER_WORLD)
        {
            continue;
        }

        memcpy(&m_stZoneConfList.m_astZoneConf[i], &stZoneConfTmp, sizeof(TZoneConf));

        m_stZoneConfList.m_ushLen++;
        i++;
    }

    inFile.close();

    return 0;
}

const TZoneConfList& CConfigManager::GetZoneConfList()
{
	return m_stZoneConfList;
}

//��ȡͨ��BUS������
ServerBusConfigManager& CConfigManager::GetBusConfigMgr()
{
	return m_stBusConfigMgr;
}

//��ȡ�������ù�����
BaseConfigManager& CConfigManager::GetBaseCfgMgr()
{
	return m_stBaseConfigManager;
}

//��ȡ�������������
int CConfigManager::GetFullWorldNum()
{
	return m_iFullWorldNum;
}

//��ȡ������æ�������
int CConfigManager::GetBusyWorldNum()
{
	return m_iBusyWorldNum;
}
