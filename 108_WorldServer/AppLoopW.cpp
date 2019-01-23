#include <unistd.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "NowTime.hpp"
#include "TimeUtility.hpp"
#include "AppDefW.hpp"
#include "MsgStatistic.hpp"
#include "AppUtility.hpp"
#include "ShmObjectCreator.hpp"
#include "WorldMsgHelper.hpp"
#include "Random.hpp"
#include "ServerBusManager.h"
#include "WorldRankInfoManager.h"
#include "WorldExchangeLimitManager.h"
#include "WorldLimitLotteryManager.h"
#include "WorldLasvegasManager.h"
#include "WorldMailManager.hpp"

#include "AppLoopW.hpp"

using namespace ServerLib;

//��ʼ��AppCmd
int CAppLoopW::ms_iAppCmd = APPCMD_NOTHING_TODO;

CAppLoopW::CAppLoopW()
{

}

CAppLoopW::~CAppLoopW()
{
}

int CAppLoopW::ReloadConfig()
{
    //GameServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "WorldServer");

    //������Ϸ����
	int iRet = CConfigManager::Instance()->Initialize(true);
	if (iRet)
	{
		TRACESVR("Failed to reload server config, ret %d\n", iRet);
		return iRet;
	}

    return 0;
}

int CAppLoopW::LoadConfig()
{
    //GameServer.tcm
    CAppUtility::LoadLogConfig(APP_CONFIG_FILE, "WorldServer");

	//������Ϸ����
	int iRet = CConfigManager::Instance()->Initialize(false);
	if (iRet)
	{
		TRACESVR("Failed to load server config, ret %d\n", iRet);
		return iRet;
	}

    return 0;
}

int CAppLoopW::Initialize(bool bResumeMode, int iWorlID)
{
    // ��ȡ����
    int iRet = LoadConfig();
	if (iRet)
	{
		TRACESVR("Failed to load server config, ret %d\n", iRet);
		exit(1);
	}

    CModuleHelper::RegisterServerID(iWorlID);
    CModuleHelper::RegisterWorldProtocolEngine(&m_stWorldProtocolEngine);
	
	iRet = m_stAllocator.Initialize(bResumeMode);
    if(iRet < 0)
    {
        TRACESVR("Allocator initialize failed: iRet = %d\n", iRet);
        exit(2);
    }

    iRet = m_stWorldProtocolEngine.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Protocol Engine initialize failed: iRet = %d\n", iRet);
        exit(3);
    }
    m_stWorldProtocolEngine.RegisterAllHandler();

	//ͨ��BUS������
    iRet = CServerBusManager::Instance()->Init("WorldServer", CConfigManager::Instance()->GetBusConfigMgr());
    if (iRet < 0)
    {
        TRACESVR("MsgTransceiver initialize failed: iRet = %d\n", iRet);
        exit(4);
    }

	//���а������
	CWorldRankInfoManager::Instance()->Init();

	//�����һ�������
	CWorldExchangeLimitManager::Instance()->Init();

	//�����齱������
	CWorldLimitLotteryManager::Instance()->Init();

	//��ת�̹�����
	CWorldLasvegasManager::Instance()->Init();

	//�ʼ�������
	CWorldMailManager::Instance()->Init();

    iRet = m_stAppTick.Initialize(bResumeMode);
    if (iRet < 0)
    {
        TRACESVR("Tick Initialize failed: %d.n\n", iRet);
        exit(6);
    }

    TRACESVR("Server Initialize Success.\n");

    return 0;
}

int CAppLoopW::Run()
{
    unsigned  int uiLoopCount = 0;

    TRACESVR("Run, run, run, never stop!\n");
    TRACESVR("WorldID: %d\n", CModuleHelper::GetWorldID());
    TRACESVR("==============================================================================\n");

    MsgStatisticSingleton::Instance()->Initialize();
    MsgStatisticSingleton::Instance()->Reset();
    static char szBuffer[MAX_MSGBUFFER_SIZE];
    int iBuffLength = sizeof(szBuffer);
    unsigned int uiNowTime = time(NULL);
    unsigned int uiStatTime = time(NULL);

    while(true)
    {
        NowTimeSingleton::Instance()->RefreshNowTime();
        NowTimeSingleton::Instance()->RefreshNowTimeVal();

        if(ms_iAppCmd == APPCMD_STOP_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_STOP_SERVICE\n");
            return 0;
        }

        if(ms_iAppCmd == APPCMD_RELOAD_CONFIG)
        {
            TRACESVR("Receive Command: APPCMD_RELOAD_CONFIG. \n");
            ReloadConfig();
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        if (ms_iAppCmd == APPCMD_QUIT_SERVICE)
        {
            TRACESVR("Receive Command: APPCMD_QUIT_SERVICE\n");
            break;
            ms_iAppCmd = APPCMD_NOTHING_TODO;
        }

        // ������Ϣ����
        int iRecvMsgCount = 0;
        while(1)
        {
            iBuffLength = sizeof(szBuffer);
            int iMsgLength=0;
			SERVERBUSID stFromBusID;
            int iRet = CServerBusManager::Instance()->RecvOneMsg(szBuffer, iBuffLength, iMsgLength, stFromBusID);
            if (iRet < 0 || iMsgLength == 0)
            {
                break;
            }

            m_stWorldProtocolEngine.OnRecvCode(szBuffer, iMsgLength, stFromBusID);
            iRecvMsgCount++;
            if (iRecvMsgCount >= 1000)
            {
                break;
            }
        }

        m_stAppTick.OnTick();

        //ͳ��
        uiNowTime = time(NULL);
        if(uiNowTime - uiStatTime >= 5 * 60) // 5����
        {
            MsgStatisticSingleton::Instance()->Print();
            MsgStatisticSingleton::Instance()->Reset();
            uiStatTime = uiNowTime;
        }

        uiLoopCount++;
        usleep(10);
    }

    return 0;
}

void CAppLoopW::SetAppCmd(int iAppCmd)
{
    ms_iAppCmd = iAppCmd;

}

