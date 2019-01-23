#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>

#include "SignalUtility.hpp"
#include "NowTime.hpp"
#include "SectionConfig.hpp"
#include "LogAdapter.hpp"
#include "StringSplitter.hpp"
#include "StringUtility.hpp"
#include "ObjStatistic.hpp"
#include "MsgStatistic.hpp"
#include "PerformanceStatistic.hpp"
#include "Random.hpp"

#include "AppUtility.hpp"

using namespace ServerLib;

const char* APP_LOCK_FILE   = "App.lock";
const char* APP_PID_FILE    = "App.pid";

int CAppUtility::ReadConfigFile(const char* pszFilename, bool* pbDaemonLaunch, bool* pbResume, int *piWorldID,int *piZoneID, int *piInstance)
{
    CSectionConfig stConfigFile;
    *pbDaemonLaunch = true;
    *pbResume = false;

    int iRet = stConfigFile.OpenFile(pszFilename);
    if (iRet != 0)
    {
        TRACESVR("Open File Error: %s \n", pszFilename);
        return -1;
    }

    char szTemp[128];
    stConfigFile.GetItemValue("RunParam","StartType", szTemp, sizeof(szTemp), "-i");

    if(!strcasecmp(szTemp, "-d"))
    {
        *pbDaemonLaunch = false;
    }
    else if(!strcasecmp(szTemp, "-i"))
    {
        *pbResume = false;
    }
    else if (!strcasecmp(szTemp, "-r"))
    {
        *pbResume = true;
    }
    else if(!strcasecmp(szTemp, "-di") || !strcasecmp(szTemp, "-id"))
    {
        *pbDaemonLaunch = false;
        *pbResume = false;
    }
    else if(!strcasecmp(szTemp, "-dr") || !strcasecmp(szTemp, "-rd"))
    {
        *pbDaemonLaunch = false;
        *pbResume = true;
    }
    else if(!strcasecmp(szTemp, "-"))
    {
        //Ĭ��
    }

    int SvrID;
    stConfigFile.GetItemValue("RunParam","SvrID", SvrID, 0);

    if (piZoneID)
    {
        *piZoneID = ((SvrID)>>16) & 0xFF;
    }

    if (piWorldID)
    {
        *piWorldID = ((SvrID)>>24) & 0xFF;
    }

    if (piInstance)
    {
        *piInstance = SvrID& 0xFF;
    }

    stConfigFile.CloseFile();
    return 0;
}

// ��������������
void CAppUtility::AppLaunch(int argc, char** argv,
	Function_SignalHandler pSigHandler,
	bool& rbResume,
	int& riWorldID,
	int& riInstanceID,
	int* piZoneID,
	char* pszHostIP,
	int* piHostPort,
	bool bEnableQuitSig)
{
	bool bDaemonLaunch = true;
	rbResume = false;
	riWorldID = 1;
	riInstanceID = 1;
	if (piZoneID)
	{
		*piZoneID = 0;
	}

	if (argc > 1)
	{
		if (!strcasecmp(argv[1], "-file"))
		{
			if (ReadConfigFile(argv[2], &bDaemonLaunch, &rbResume, &riWorldID, piZoneID, &riInstanceID))
			{
				exit(0);
			}
		}

		// ����-d, -i, -v, -r�ĸ�ԭ�еĻ�������
		if (!strcasecmp(argv[1], "-v"))
		{
			ShowVersion();
			exit(0);
		}
		else if (!strcasecmp(argv[1], "-d"))
		{
			bDaemonLaunch = false;
		}
		else if (!strcasecmp(argv[1], "-i"))
		{
			rbResume = false;
		}
		else if (!strcasecmp(argv[1], "-r"))
		{
			rbResume = true;
		}
		else if (!strcasecmp(argv[1], "-di") || !strcasecmp(argv[1], "-id"))
		{
			bDaemonLaunch = false;
			rbResume = false;
		}
		else if (!strcasecmp(argv[1], "-dr") || !strcasecmp(argv[1], "-rd"))
		{
			bDaemonLaunch = false;
			rbResume = true;
		}

		// ���ʣ��Ĳ���
		for (int i = 1; i < argc; i++)
		{
			if (!strcasecmp(argv[i], "-w"))
			{
				riWorldID = atoi(argv[i + 1]);
				i++;
			}
			else if (!strcasecmp(argv[i], "-z"))
			{
				if (piZoneID)
				{
					*piZoneID = atoi(argv[i + 1]);
				}
				i++;
			}
			else if (!strcasecmp(argv[i], "-ins"))
			{
				riInstanceID = atoi(argv[i + 1]);
				i++;
			}
			else if (!strcasecmp(argv[i], "-h"))
			{
				if (pszHostIP)
				{
					SAFE_STRCPY(pszHostIP, argv[i + 1], 16);
				}
				i++;
			}
			else if (!strcasecmp(argv[i], "-p"))
			{
				if (piHostPort)
				{
					*piHostPort = atoi(argv[i + 1]);
				}
				i++;
			}
			else if (!strcasecmp(argv[i], "--resume"))
			{
				rbResume = true;
			}
			else if (!strncasecmp(argv[i], "--id=", strlen("--id=")))
			{
				// TBUS��ַΪ: world:12.zone:4.function:8.instance:8
				char szID[16];
				CStringSplitter stSpliter;

				const char* pszTBusAddr = argv[i];
				pszTBusAddr += strlen("--id=");
				stSpliter.SetString(pszTBusAddr);

				int iRet;

				// WorldID
				iRet = stSpliter.GetNextToken(".", szID, sizeof(szID));
				if (iRet < 0)
				{
					exit(1);
				}
				riWorldID = atoi(szID);

				// ZoneID
				iRet = stSpliter.GetNextToken(".", szID, sizeof(szID));
				if (iRet < 0)
				{
					exit(1);
				}
				if (piZoneID)
				{
					*piZoneID = atoi(szID);
				}

				// FunctionID
				iRet = stSpliter.GetNextToken(".", szID, sizeof(szID));
				if (iRet < 0)
				{
					exit(1);
				}

				// InstanceID
				iRet = stSpliter.GetNextToken(".", szID, sizeof(szID));
				if (iRet < 0)
				{
					exit(1);
				}
				riInstanceID = atoi(szID);
			}
			else if (!strcasecmp(argv[i], "stop"))
			{
				int iPid = ReadPidFile();
				if (iPid > 0)
				{
					kill(iPid, SIGUSR2);
				}

				exit(0);
			}
			else if (!strcasecmp(argv[i], "reload"))
			{
				int iPid = ReadPidFile();
				if (iPid > 0)
				{
					kill(iPid, SIGUSR1);
				}

				exit(0);
			}
		}
	}

	// ���Lock�ļ�, ��ֹ�ظ�����
	CheckLock(APP_LOCK_FILE);

	// ��̨Daemonģʽ����
	if (bDaemonLaunch)
	{
		DaemonLaunch();
	}

	// ʱ���Ż�
	NowTimeSingleton::Instance()->RefreshNowTime();
	NowTimeSingleton::Instance()->RefreshNowTimeVal();

	// ���ý������������
	CRandomCalculator::Initialize();

	// ����USR�źŴ���, ����ֹͣ����������
	RegisterSignalHandler(pSigHandler, bEnableQuitSig);

	// д��PID�ļ�
	WritePidFile();
}

void CAppUtility::ShowVersion()
{
    const char* pszDebug = "Yes";
    const char* pszAssert = "Enable";

#ifndef _DEBUG_
    pszDebug = "No";
#endif

#ifdef _DISABLE_ASSERT_
    pszAssert = "Disable";
#endif

    printf("Time: %s %s\n", __DATE__, __TIME__);
    printf("Debug: %s\n", pszDebug);
    printf("Assert: %s\n", pszAssert);
}

// ��������
void CAppUtility::ShowUsage(const char* pszName)
{
    printf("\nUsage: %s  -[d|i|v]\n\n", pszName);
    printf("  -d     : debug mode\n");
    printf("  -i     : initialize mode to attach share memory\n");
    printf("  -r     : resume mode to attach share memory\n");
    printf("  -v     : print the version information and exit\n");
}

void CAppUtility::CheckLock(const char* pszLockFile)
{
    int iLockFD = open(pszLockFile, O_RDWR | O_CREAT, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
    if(iLockFD < 0)
    {
        printf("Failed to open lock file: %s, quit!\n", pszLockFile);
        exit(1);
    }

    if(flock(iLockFD, LOCK_EX | LOCK_NB) < 0)
    {
        printf("Same Server is Running ......  quit!\n");
        exit(1);
    }
}


//��ʼ��Ϊ�ػ����̵ĺ���
void CAppUtility::DaemonLaunch(void)
{
    if(fork())
    {
        exit(0);
    }

    setsid();
    umask(0);

    CSignalUtility::IgnoreSignalSet();

    if(fork())
    {
        exit(0);
    }
}

//����������˳���pid�ļ�����ɾ��������Ϊ�쳣��ֹ
//����pid�ɱ��ű������رս��̡����¶�ȡ�����ļ�
void CAppUtility::WritePidFile()
{
    FILE* fp;

    fp = fopen(APP_PID_FILE, "w");
    if(NULL == fp)
    {
        printf("Failed to open pid file: %s\n", APP_PID_FILE);
    }

    int iPid = getpid();

    fprintf(fp, "%d", iPid);

    fclose(fp);

    printf("WritePid: %d\n", iPid);
}

void CAppUtility::CleanPidFile()
{
    unlink(APP_PID_FILE);
}

// ��ȡPID�ļ�
int CAppUtility::ReadPidFile()
{
    FILE* fp;

    fp = fopen(APP_PID_FILE, "r");
    if(NULL == fp)
    {
        printf("Failed to open pid file: %s\n", APP_PID_FILE);
        return -1;
    }

    char szPid[100];
    fread(szPid, sizeof(szPid), 1, fp);
    int iPid = atoi(szPid);

    fclose(fp);

    printf("ReadPid: %d\n", iPid);

    return iPid;
}

// ��ʼ����־����
void CAppUtility::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
	//�����ж�
	if (!pszConfigFile || !pszLogName)
	{
		printf("Failed to load log config, invalid param!\n");
		return;
	}

    char szLogPath[64] = {0};
    char szLogName[64] = {0};
    int iLogLevel = 0;
	int iPrintLog = 0;

	//��ȡ��־����
	CSectionConfig stConfigFile;
	int iRet = stConfigFile.OpenFile(pszConfigFile);
	if (iRet)
	{
		printf("Failed to open log config file %s, ret %d\n", pszConfigFile, iRet);
		return;
	}

	//��ȡ��־����Ϣ
	stConfigFile.GetItemValue("Param", "LogPath", szLogPath, sizeof(szLogPath), "../log/");
	stConfigFile.GetItemValue("Param", "LogName", szLogName, sizeof(szLogName), pszLogName);
	stConfigFile.GetItemValue("Param", "LogLevel", iLogLevel, LOG_LEVEL_WARNING);

	//------------------------------------- ����������־ -------------------------------------
    TLogConfig stServerLogConfig;
    memset(&stServerLogConfig, 0, sizeof(stServerLogConfig));
    SAFE_STRCPY(stServerLogConfig.m_szPath, szLogPath, sizeof(stServerLogConfig.m_szPath)-1);
    SAFE_STRCPY(stServerLogConfig.m_szBaseName, szLogName, sizeof(stServerLogConfig.m_szBaseName)-1);
    SAFE_STRCPY(stServerLogConfig.m_szExtName, ".log", sizeof(stServerLogConfig.m_szExtName)-1);
    stServerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
    stServerLogConfig.m_iLogLevel = iLogLevel;
    stServerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
    stServerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
    stServerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
    stServerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;
    ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

	//------------------------------------- ��Ӫ��־ -------------------------------------
	stConfigFile.GetItemValue("Param", "OssLog", iPrintLog, 0);
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

	//------------------------------------- �����־ -------------------------------------
	stConfigFile.GetItemValue("Param", "PlayerLog", iPrintLog, 0);
	if (iPrintLog)
	{
		TLogConfig stPlayerLogConfig;
		memset(&stPlayerLogConfig, 0, sizeof(stPlayerLogConfig));
		char szPlayLogPath[MAX_FILENAME_LENGTH];
		memset(szPlayLogPath, 0, sizeof(szPlayLogPath));
		sprintf(szPlayLogPath, "%s%s", szLogPath, "/player/");
		SAFE_STRCPY(stPlayerLogConfig.m_szPath, szPlayLogPath, sizeof(stPlayerLogConfig.m_szPath) - 1);
		SAFE_STRCPY(stPlayerLogConfig.m_szBaseName, "player_", sizeof(stPlayerLogConfig.m_szBaseName) - 1);
		SAFE_STRCPY(stPlayerLogConfig.m_szExtName, ".log", sizeof(stPlayerLogConfig.m_szExtName) - 1);
		stPlayerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
		stPlayerLogConfig.m_iLogLevel = iLogLevel;
		stPlayerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
		stPlayerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
		stPlayerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
		stPlayerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT;
		PlayerLogSingleton::Instance()->ReloadLogConfig(stPlayerLogConfig);
	}

	//------------------------------------- ͳ����־ -------------------------------------
	stConfigFile.GetItemValue("Param", "StatLog", iPrintLog, 0);
	if (iPrintLog)
	{
		TLogConfig stStatLogConfig;
		memset(&stStatLogConfig, 0, sizeof(stStatLogConfig));
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
	}

	stConfigFile.CloseFile();

	return;
}

// ע��ֹͣ�����������źŴ���
void CAppUtility::RegisterSignalHandler(Function_SignalHandler pSigHandler, bool bEnableQuitSig)
{
    // USR1 Ϊ���¼�������
    CSignalUtility::SetHandler_USR1(pSigHandler, APPCMD_RELOAD_CONFIG);

    // USR2 Ϊֹͣ������
    CSignalUtility::SetHandler_USR2(pSigHandler, APPCMD_STOP_SERVICE);

    // Quit Ϊ�����˳�ѭ��
    if (bEnableQuitSig)
    {
        CSignalUtility::SetHandler_QUIT(pSigHandler, APPCMD_QUIT_SERVICE);
    }
}

