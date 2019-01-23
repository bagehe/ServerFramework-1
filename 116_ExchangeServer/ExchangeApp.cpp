#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "NowTime.hpp"
#include "LogAdapter.hpp"
#include "AppUtility.hpp"
#include "SectionConfig.hpp"
#include "ObjStatistic.hpp"
#include "MsgStatistic.hpp"
#include "PerformanceStatistic.hpp"
#include "ServerBusManager.h"
#include "AppThreadManager.hpp"
#include "ThreadLogManager.hpp"
#include "ConfigManager.hpp"
#include "ExchangeMsgProcessor.hpp"

#include "ExchangeApp.hpp"

int CExchangeApp::ms_iAppCmd = APPCMD_NOTHING_TODO;

CExchangeApp::CExchangeApp()
{
    m_bResumeMode = false;
    m_uiLoopTimes = 0;
}

CExchangeApp::~CExchangeApp()
{
}

int CExchangeApp::Initialize(bool bResume)
{
    // ��ȡ����
    LoadConfig();

    m_bResumeMode = bResume;

	//ͨ��BUS
	int iRet = CServerBusManager::Instance()->Init("Exchange", CConfigManager::Instance()->GetBusConfigMgr());
    if (iRet)
    {
        return -1;
    }

	//��Ϣ������
	iRet = CExchangeMsgProcessor::Instance()->Initialize();
    if (iRet)
    {
        return -2;
    }

	//ƽ̨��֤�̳߳�
	for (int i = 0; i < WORK_THREAD_NUM; ++i)
	{
		iRet = CAppThreadManager::Instance()->InitOneThread(new CExchangeProtocolEngine, new CExchangeHandlerSet);
		if (iRet)
		{
			return -4;
		}
	}

    return 0;
}

void CExchangeApp::Run()
{
    TRACESVR("ExchangeServer starts running ...\n");

    m_uiLoopTimes = 0;
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
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. \n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
        }

        const int MAX_MSG_COUNT_PER_COUNT = 1000;
        int iRecvMsgCount = 0;

        while (true)
        {
            int iNewMsgCount = 0;

			CExchangeMsgProcessor::Instance()->ProcessRecvMsg(iNewMsgCount);

            // û������, �������ݳ�����������, ��ͣ��������
            iRecvMsgCount += iNewMsgCount;
            if (iNewMsgCount == 0 || iRecvMsgCount >= MAX_MSG_COUNT_PER_COUNT)
            {
                break;
            }
        }

        usleep(MAIN_LOOP_SLEEP);
        m_uiLoopTimes++;

        // ����ʱ��cache��㣬��ѭ��1�������һ��
        if (10000 == m_uiLoopTimes)
        {
            m_uiLoopTimes = 0; // ���¼���
        }
    }
}

void CExchangeApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CExchangeApp::ReloadConfig()
{
	LoadLogConfig(APP_CONFIG_FILE, "ExchangeServer");

	// ��ȡExchange Server������
	int iRet = CConfigManager::Instance()->LoadAllConf();
	if (iRet)
	{
		TRACESVR("failed to load config\n");
		return -1;
	}

    return 0;
}

int CExchangeApp::LoadConfig()
{
	LoadLogConfig(APP_CONFIG_FILE, "ExchangeServer");

	// ��ȡExchange Server������
	int iRet = CConfigManager::Instance()->LoadAllConf();
	if (iRet)
	{
		TRACESVR("failed to load config\n");
		return -1;
	}

    return 0;
}

int CExchangeApp::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
	char szLogPath[64] = { 0 };
	char szLogName[64] = { 0 };
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
	SAFE_STRCPY(stServerLogConfig.m_szPath, szLogPath, sizeof(stServerLogConfig.m_szPath) - 1);
	SAFE_STRCPY(stServerLogConfig.m_szBaseName, szLogName, sizeof(stServerLogConfig.m_szBaseName) - 1);
	SAFE_STRCPY(stServerLogConfig.m_szExtName, ".log", sizeof(stServerLogConfig.m_szExtName) - 1);
	stServerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
	stServerLogConfig.m_iLogLevel = iLogLevel;

	stServerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
	stServerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
	stServerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
	stServerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;

	//��ʼ�����߳���־
	int iRet = CThreadLogManager::Instance()->Initialize(WORK_THREAD_NUM, stServerLogConfig);
	if (iRet)
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

	strncpy(stStatLogConfig.m_szExtName, ".log", sizeof(stStatLogConfig.m_szExtName) - 1);
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
