
#include <errno.h>

#include "SectionConfig.hpp"
#include "Kernel/AppDef.hpp"
#include "Kernel/ModuleHelper.hpp"

#include "ConfigManager.hpp"

static const char* sModuleName[MODULE_MAX_ID] =
{
	"IsFishEnabled",
};

CConfigManager* CConfigManager::Instance()
{
	static CConfigManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CConfigManager;
	}

	return pInstance;
}

CConfigManager::CConfigManager()
{

}

CConfigManager::~CConfigManager()
{

}

int CConfigManager::Initialize(bool bResumeMode)
{
    int iRet = 0;

    //GMȨ���������
    iRet = m_stGMPrivConfigManager.LoadGMPrivConfig();
    if(iRet)
    {
        return -1;
    }

	//ͨ��BUS���ù�����
	iRet = m_stBusConfigManager.LoadServerBusConfig();
	if (iRet)
	{
		return -2;
	}

	//Excel���ù�����
	iRet = m_stBaseConfigManager.LoadAllConfig("../../../../Common/config/");
	if (iRet)
	{
		return -3;
	}

	//���ط���������
	iRet = LoadZoneConfig();
	if (iRet)
	{
		return -4;
	}

	//�����ԭ����Ϣ��������Ϣ
	CHandlerFactory::ClearAllDisableMsg();

	//load
	LoadMsgInterval();
	LoadMsgDiableList();

    // ��ӡ�������ڴ�ʹ�����
    PrintConfigMemoryStatics();

    return 0;
};

//GMȨ������
CGMPrivConfigManager& CConfigManager::GetGMPrivConfigManager()
{
	return m_stGMPrivConfigManager;
}

//��ȡͨ��BUS������
ServerBusConfigManager& CConfigManager::GetBusConfigMgr()
{
	return m_stBusConfigManager;
}

//��ȡ�������ù�����
BaseConfigManager& CConfigManager::GetBaseCfgMgr()
{
	return m_stBaseConfigManager;
}

//ģ���Ƿ�ر�
bool CConfigManager::IsModuleSwitchOn(MODULE_ID eModuleID)
{
	return m_aiModuleSwitch[(int)eModuleID];
}

//�Ƿ���SessionKey
bool CConfigManager::IsSSKeyCheckEnabled()
{
	return m_iCheckSSKeyEnabled;
}

// �Ƿ���Player��־
bool CConfigManager::IsPlayerLogEnabled()
{
	return m_iIsEnablePlayerLog;
}

//���ط���������
int CConfigManager::LoadZoneConfig()
{
	CSectionConfig stConfigFile;
	int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
	if (iRet)
	{
		LOGERROR("Failed to load config file %s\n", APP_CONFIG_FILE);
		return -1;
	}

	//����ģ�鿪��
	memset(m_aiModuleSwitch, 0, sizeof(m_aiModuleSwitch));
	for (int i = 0; i < (int)MODULE_MAX_ID; ++i)
	{
		stConfigFile.GetItemValue("ModuleSwitch", sModuleName[i], m_aiModuleSwitch[i], 1);
	}

	//����SessionKey��鿪��
	stConfigFile.GetItemValue("module", "SSKeyChecked", m_iCheckSSKeyEnabled, 0);

	//���ش�ӡPlayer��־����
	stConfigFile.GetItemValue("Param", "IsEnablePlayerLog", m_iIsEnablePlayerLog, 0);

	stConfigFile.CloseFile();

	return 0;
}

int CConfigManager::PrintConfigMemoryStatics()
{
    LOGDEBUG("********************** CONFIG MEMORY STATICS *********************\n");

    int iConfigTotalSize = 0;

    // �ܼ�
    LOGDEBUG("Total Memory: %dB, %dMB\n", iConfigTotalSize, iConfigTotalSize/1024/1024);
    LOGDEBUG("*****************************************************************\n");

    return 0;
}

int CConfigManager::LoadMsgInterval()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet != 0)
    {
        return 0;
    }

    char szMsgLimitFile[MAX_FILENAME_LENGTH];
    stConfigFile.GetItemValue("MsgLimit", "MsgLimitFile", szMsgLimitFile, sizeof(szMsgLimitFile), "../conf/MsgLimitFile.conf");

    FILE* fp = fopen(szMsgLimitFile, "r");
    if (fp == NULL)
    {
        TRACESVR("can not open MsgLimitFile:%s for :%s\n", szMsgLimitFile, strerror(errno));
        printf("can not open MsgLimitFile:%s for: %s\n", szMsgLimitFile, strerror(errno));
        return -1;
    }

    char szLine[100];
    unsigned int uiMsgID;
    unsigned int uiMsgLimitInterval;
    while(fgets(szLine, sizeof(szLine), fp))
    {
        //���л��߸�ʽ����
        if (sscanf(szLine, "%u %u", &uiMsgID, &uiMsgLimitInterval) != 2)
        {
            continue;
        }
        if (uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
        {
            TRACESVR("file :%s msg id is too bigger than MAX_HANDLER_NUMBER", szMsgLimitFile);
            fclose(fp);
            return -20;
        }

        if (uiMsgLimitInterval >= (unsigned int)MAX_MSG_INTERVAL_TIME)
        {
            uiMsgLimitInterval = MAX_MSG_INTERVAL_TIME;
        }

        TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
        if (!pstMsgConfig)
        {
            continue;
        }

        TRACESVR("msgid:%u  MsgLimitInterval:%u\n", uiMsgID, uiMsgLimitInterval);
        pstMsgConfig->m_iMsgInterval = uiMsgLimitInterval;
    }

    fclose(fp);

    return 0;
}

int CConfigManager::LoadMsgDiableList()
{
    CSectionConfig stConfigFile;
    int iRet = stConfigFile.OpenFile(APP_CONFIG_FILE);
    if (iRet != 0)
    {
        return 0;
    }

    char szMsgDisableFile[MAX_FILENAME_LENGTH];
    stConfigFile.GetItemValue("MsgDisable", "MsgDisableFile", szMsgDisableFile, sizeof(szMsgDisableFile), "../conf/MsgDisableFile.conf");

    //û�о�����
    FILE* fp = fopen(szMsgDisableFile, "r");
    if (fp == NULL)
    {
        TRACESVR("can not open MsgLimitFile:%s for :%s\n", szMsgDisableFile, strerror(errno));
        return 0;
    }

    char szLine[100];
    unsigned int uiMsgID;
    while(fgets(szLine, sizeof(szLine), fp))
    {
        //���л��߸�ʽ����
        if (sscanf(szLine, "%u", &uiMsgID) != 1)
        {
            continue;
        }

        CHandlerFactory::DisabledMsg(uiMsgID);
        TRACESVR("msgid:%u is disabled", uiMsgID);
    }

    fclose(fp);

    return 0;
}

int CConfigManager::GetMsgInterval(unsigned int uiMsgID)
{
    if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
    {
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
        return 0;
    }

    TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
    if (!pstMsgConfig)
    {
        return 0;
    }

    return pstMsgConfig->m_iMsgInterval;
}
