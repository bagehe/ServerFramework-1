#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "NowTime.hpp"
#include "SectionConfig.hpp"
#include "StringUtility.hpp"
#include "ObjStatistic.hpp"
#include "MsgStatistic.hpp"
#include "PerformanceStatistic.hpp"
#include "AppUtility.hpp"
#include "ConfigManager.hpp"
#include "ServerBusManager.h"
#include "RoleDBProtocolEngine.hpp"
#include "AppThreadManager.hpp"
#include "ThreadLogManager.hpp"

#include "RoleDBApp.hpp"

using namespace ServerLib;

int CRoleDBApp::ms_iAppCmd = APPCMD_NOTHING_TODO;

CRoleDBApp::CRoleDBApp()
{
    m_bResumeMode = false;
}

CRoleDBApp::~CRoleDBApp()
{

}

int CRoleDBApp::Initialize(bool bResume, int iWorldID)
{
    // ��ȡ����
	int iRet = LoadConfig();
    if (iRet)
    {
        return -1;
    }

	m_iWorldID = iWorldID;
    m_bResumeMode = bResume;

	iRet = CServerBusManager::Instance()->Init("RoleDB", ConfigManager::Instance()->GetBusConfigMgr());
    if (iRet)
    {
        return -2;
    }

    //��ʼ�������߳�
	for (int i = 0; i < (int)ROLE_TABLE_SPLIT_FACTOR; ++i)
	{
		iRet = CAppThreadManager::Instance()->InitOneThread(new CRoleDBProtocolEngine, new CRoleDBHandlerSet);
		if (iRet)
		{
			TRACESVR("Error: init thread manager fail. rt:%d\n", iRet);
			return -3;
		}
	}

    return 0;
}

void CRoleDBApp::Run()
{
    TRACESVR("RoleDBServer starts running ...\n");

    //TODO -Werror=unused-but-set-variable
    //int iRes = 0;

    while (true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        if (ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
            return;
        }

        if (ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. Do nothing now\n");
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        const int MAX_MSG_COUNT_PER_COUNT = 100;
        int iRecvMsgCount = 0;

        while (true)
        {
            int iRecvNewMsgCount = 0;
            int iSendNewMsgCount = 0;

            //TODO -Werror=unused-but-set-variable
            HandleMsgIn(iRecvNewMsgCount);

            //process thread msg
            //TODO -Werror=unused-but-set-variable
            HandleThreadMsg(iSendNewMsgCount);

            // û������, �������ݳ�����������, ��ͣ��������
            iRecvMsgCount += iRecvNewMsgCount;

            // ���û����Ϣ�������û����Ϣ����� �����ܽ�����Ϣ��������
            if ((iRecvNewMsgCount <= 0 && iSendNewMsgCount <= 0) || iRecvMsgCount >= MAX_MSG_COUNT_PER_COUNT)
            {
                break;
            }

            LOGDEBUG("recv msg:%d, send msg:%d\n", iRecvNewMsgCount, iSendNewMsgCount);
        }

        usleep(APP_ROLEDB_MAX_SLEEP_USEC);
    }
}

int CRoleDBApp::HandleMsgIn(int& riRecvMsgCount)
{
    int iRes = ReceiveAndDispatchMsg();
    if (iRes != 0)
    {
        return -1;
    }
    else
    {
        riRecvMsgCount++;
    }

    return 0;
}

int CRoleDBApp::ReceiveAndDispatchMsg()
{
    int iBufLen = sizeof(m_szCodeBuf);
    int iCodeLen = 0;

    // ������������
	SERVERBUSID stFromBusID;
    int iRet = CServerBusManager::Instance()->RecvOneMsg(m_szCodeBuf, iBufLen, iCodeLen, stFromBusID);
    if ((iRet < 0) || (0 == iCodeLen))
    {
        return -1;
    }

	//4�ֽڵ�uin
    unsigned uiUin = *((unsigned int*)(m_szCodeBuf+sizeof(unsigned short)));

    TRACESVR("Receive code OK, len: %d, Uin = %u\n", iCodeLen, uiUin);

    iRet = CAppThreadManager::Instance()->PushCode(uiUin, (const unsigned char*)m_szCodeBuf, iCodeLen);
    if (iRet != 0)
    {
        TRACESVR("Error: push into thread fail. rt:%d, uin:%u\n", iRet, uiUin);
        return -5;
    }

    return 0;
}

void CRoleDBApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CRoleDBApp::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
    char szLogPath[64] = {0};
    char szLogName[64] = {0};
    int iLogLevel;

    if (!pszConfigFile)
    {
        // ʹ��Ĭ������
        SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
        SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
        iLogLevel = LOG_LEVEL_ANY;
    }
    else
    {
        // ��ȡ�����ļ�
        CSectionConfig stConfigFile;
        int iRet = stConfigFile.OpenFile(pszConfigFile);
        if (iRet == 0)
        {
            stConfigFile.GetItemValue("Param", "LogPath", szLogPath, sizeof(szLogPath), "../log/");
            stConfigFile.GetItemValue("Param", "LogName", szLogName, sizeof(szLogName), pszLogName);
            stConfigFile.GetItemValue("Param", "LogLevel", iLogLevel, LOG_LEVEL_WARNING);
            stConfigFile.CloseFile();
        }
        else
        {
            // ��ȡʧ��, ʹ��Ĭ������
            SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
            SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
            iLogLevel = LOG_LEVEL_ANY;
        }
    }

    // server log
    TLogConfig stServerLogConfig;
    SAFE_STRCPY(stServerLogConfig.m_szPath, szLogPath, sizeof(stServerLogConfig.m_szPath)-1);
    SAFE_STRCPY(stServerLogConfig.m_szBaseName, szLogName, sizeof(stServerLogConfig.m_szBaseName)-1);
    SAFE_STRCPY(stServerLogConfig.m_szExtName, ".log", sizeof(stServerLogConfig.m_szExtName)-1);
    stServerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
    stServerLogConfig.m_iLogLevel = iLogLevel;

    stServerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
    stServerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
    stServerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
    stServerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;

    //��ʼ��log
    int iRt = CThreadLogManager::Instance()->Initialize(ROLE_TABLE_SPLIT_FACTOR, stServerLogConfig);
    if (iRt != 0)
    {
        return -5;
    }

    ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

    // ����ͳ����־
    TLogConfig stStatLogConfig;
    char szStatPath[MAX_FILENAME_LENGTH];
    memset(szStatPath, 0, sizeof(szStatPath));

    sprintf(szStatPath, "%s%s", szLogPath, "/stat/");
    strncpy(stStatLogConfig.m_szPath, szStatPath, sizeof(stStatLogConfig.m_szPath) - 1);

    strncpy(stStatLogConfig.m_szExtName, ".log", sizeof(stStatLogConfig.m_szExtName)-1);
    stStatLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
    stStatLogConfig.m_iLogLevel = LOG_LEVEL_ANY;
    stStatLogConfig.m_shLogType = ELT_LOG_USE_FPRINTF;
    stStatLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
    stStatLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
    stStatLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT;

    // ��Ϣͳ��
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "m_", pszLogName);
    MsgStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
    MsgStatisticSingleton::Instance()->Reset();

    // ����ͳ��
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "o_", pszLogName);
    ObjectStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
    ObjectStatisticSingleton::Instance()->Reset();

    // ����ͳ��
    sprintf(stStatLogConfig.m_szBaseName, "%s%s", "p_", pszLogName);
    PerformanceStatisticSingleton::Instance()->Initialize(stStatLogConfig.m_szPath, stStatLogConfig.m_szBaseName);
    PerformanceStatisticSingleton::Instance()->Reset();

    return 0;
}

int CRoleDBApp::ReloadConfig()
{
    LoadLogConfig(APP_CONFIG_FILE, "RoleDBServer");

	//���������ļ�
	int iRet = ConfigManager::Instance()->LoadAllConfig();
	if (iRet)
	{
		LOGERROR("Failed to load all config, ret %d\n", iRet);
		return iRet;
	}

    TRACESVR("Reload Config Success.\n");

    return 0;
}

int CRoleDBApp::LoadConfig()
{
    LoadLogConfig(APP_CONFIG_FILE, "RoleDBServer");

	//���������ļ�
	int iRet = ConfigManager::Instance()->LoadAllConfig();
	if (iRet)
	{
		LOGERROR("Failed to load all config, ret %d\n", iRet);
		return iRet;
	}

    TRACESVR("Load Config Success.\n");

    return 0;
}

int CRoleDBApp::HandleThreadMsg(int& riSendMsgCount)
{
    int iRet = 0;
    int iMaxLength = sizeof(m_szCodeBuf);
    int riMsgLength = 0;

    for (int i = 0; i < CAppThreadManager::Instance()->GetThreadNum(); i++)
    {
        iRet = CAppThreadManager::Instance()->PopCode(i, (unsigned char*)m_szCodeBuf, iMaxLength, riMsgLength);
        if ((iRet >= 0) && (riMsgLength > 0))
        {
            // ������������
			SERVERBUSID stToBusID;
			stToBusID.usWorldID = m_iWorldID;
			stToBusID.usServerID = GAME_SERVER_WORLD;
            int iRet = CServerBusManager::Instance()->SendOneMsg(m_szCodeBuf, riMsgLength, stToBusID);
            if (iRet < 0)
            {
                TRACESVR("Error: send fail. rt:%d\n", iRet);
                return -1;
            }
            else
            {
                riSendMsgCount++;
            }
        }
    }

    return 0;
}
