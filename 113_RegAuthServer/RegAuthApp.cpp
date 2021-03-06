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
#include "SessionManager.hpp"
#include "RegAuthMsgProcessor.hpp"

#include "RegAuthApp.hpp"

int CRegAuthApp::ms_iAppCmd = APPCMD_NOTHING_TODO;

CRegAuthApp::CRegAuthApp()
{
    m_bResumeMode = false;
    m_uiLoopTimes = 0;
}

CRegAuthApp::~CRegAuthApp()
{
}

int CRegAuthApp::Initialize(bool bResume)
{
    // 读取配置
    LoadConfig();

    m_bResumeMode = bResume;

	//通信BUS
	int iRet = CServerBusManager::Instance()->Init("RegAuth", CConfigManager::Instance()->GetBusConfigMgr());
    if (iRet)
    {
        return -1;
    }

	//消息处理器
	iRet = CRegAuthMsgProcessor::Instance()->Initialize();
    if (iRet)
    {
        return -2;
    }

	//Session管理
	iRet = CSessionManager::Instance()->Initialize(bResume);
    if (iRet)
    {
        return -3;
    }

	//平台认证线程池
	for (int i = 0; i < PLATFORM_AUTH_THREAD_NUM; ++i)
	{
		iRet = CAppThreadManager::Instance()->InitOneThread(new CRegAuthProtocolEngine, new CRegAuthHandlerSet);
		if (iRet)
		{
			return -4;
		}
	}

    return 0;
}

void CRegAuthApp::Run()
{
    TRACESVR("RegAuthServer starts running ...\n");

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

			CRegAuthMsgProcessor::Instance()->ProcessRecvMsg(iNewMsgCount);

            // 没有数据, 或者数据超出处理限制, 暂停处理数据
            iRecvMsgCount += iNewMsgCount;
            if (iNewMsgCount == 0 || iRecvMsgCount >= MAX_MSG_COUNT_PER_COUNT)
            {
                break;
            }
        }

        usleep(MAIN_LOOP_SLEEP);
        m_uiLoopTimes++;

        // 清理超时的cache结点，主循环1万次清理一次
        if (10000 == m_uiLoopTimes)
        {
            m_uiLoopTimes = 0; // 重新计数
        }
    }
}

void CRegAuthApp::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;
}

int CRegAuthApp::ReloadConfig()
{
	LoadLogConfig(APP_CONFIG_FILE, "RegAuthServer");

	// 读取RegAuth Server的配置
	int iRet = CConfigManager::Instance()->LoadAllConf();
	if (iRet)
	{
		TRACESVR("failed to load config\n");
		return -1;
	}

    return 0;
}

int CRegAuthApp::LoadConfig()
{
	LoadLogConfig(APP_CONFIG_FILE, "RegAuthServer");

	// 读取RegAuth Server的配置
	int iRet = CConfigManager::Instance()->LoadAllConf();
	if (iRet)
	{
		TRACESVR("failed to load config\n");
		return -1;
	}

    return 0;
}

int CRegAuthApp::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
	char szLogPath[64] = { 0 };
	char szLogName[64] = { 0 };
	int iLogLevel = 0;
	int iPrintLog = 0;

	if (!pszConfigFile)
	{
		// 使用默认配置
		SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
		SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
		iLogLevel = LOG_LEVEL_ANY;
		iPrintLog = 0;
	}
	else
	{
		// 读取配置文件
		CSectionConfig stConfigFile;
		int iRet = stConfigFile.OpenFile(pszConfigFile);
		if (iRet == 0)
		{
			stConfigFile.GetItemValue("Param", "LogPath", szLogPath, sizeof(szLogPath), "../log/");
			stConfigFile.GetItemValue("Param", "LogName", szLogName, sizeof(szLogName), pszLogName);
			stConfigFile.GetItemValue("Param", "LogLevel", iLogLevel, LOG_LEVEL_WARNING);
			stConfigFile.GetItemValue("Param", "OssLog", iPrintLog, 0);
			stConfigFile.CloseFile();
		}
		else
		{
			// 读取失败, 使用默认配置
			SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
			SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
			iLogLevel = LOG_LEVEL_ANY;
			iPrintLog = 0;
		}
	}

	//服务器逻辑日志
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

	//初始化多线程逻辑日志
	int iRet = CThreadLogManager::Instance()->Initialize(PLATFORM_AUTH_THREAD_NUM, stServerLogConfig);
	if (iRet)
	{
		return -5;
	}

	ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

	//初始化运营日志
	if (iPrintLog)
	{
		TLogConfig stBillLogConfig;
		memset(&stBillLogConfig, 0, sizeof(stBillLogConfig));
		time_t tNow = ServerLib::NowTimeSingleton::Instance()->GetNowTime();
		ServerLib::CUnixTime stUnixTime(tNow);
		SAFE_SPRINTF(stBillLogConfig.m_szPath, sizeof(stBillLogConfig.m_szPath) - 1,
			"../bills/%04d-%02d/", stUnixTime.GetYear(), stUnixTime.GetMonth());
		SAFE_SPRINTF(stBillLogConfig.m_szBaseName, sizeof(stBillLogConfig.m_szBaseName) - 1, "%sOssLog", pszLogName);
		SAFE_STRCPY(stBillLogConfig.m_szExtName, ".log", sizeof(stBillLogConfig.m_szExtName) - 1);
		stBillLogConfig.m_iAddDateSuffixType = EADST_DATE_YMDH;	//EADST_DATE_YMD;
		stBillLogConfig.m_iLogLevel = LOG_LEVEL_ANY;
		stBillLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
		stBillLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
		stBillLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
		stBillLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;

		BillLogSingleton::Instance()->ReloadLogConfig(stBillLogConfig);
	}

	// 配置统计日志
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

	// 消息统计
	sprintf(stStatLogConfig.m_szBaseName, "%s%s", "m_", pszLogName);
	MsgStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
	MsgStatisticSingleton::Instance()->Reset();

	// 对象统计
	sprintf(stStatLogConfig.m_szBaseName, "%s%s", "o_", pszLogName);
	ObjectStatisticSingleton::Instance()->ReloadLogConfig(stStatLogConfig);
	ObjectStatisticSingleton::Instance()->Reset();

	// 性能统计
	sprintf(stStatLogConfig.m_szBaseName, "%s%s", "p_", pszLogName);
	PerformanceStatisticSingleton::Instance()->Initialize(stStatLogConfig.m_szPath, stStatLogConfig.m_szBaseName);
	PerformanceStatisticSingleton::Instance()->Reset();

	return 0;
}
