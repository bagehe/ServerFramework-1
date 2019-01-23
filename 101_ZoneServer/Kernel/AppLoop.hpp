#ifndef __APP_LOOP_HPP__
#define __APP_LOOP_HPP__

#include "GameProtocolEngine.hpp"
#include "HandlerList.hpp"
#include "ModuleHelper.hpp"
#include "ZoneObjectAllocator.hpp"
#include "TimeUtility.hpp"
#include "AppTick.hpp"

using namespace ServerLib;

class CAppLoop
{
public:
    // �źŴ���
    static int ms_iAppCmd;
    static void SetAppCmd(int iAppCmd);

public:
    CAppLoop();

    int Initialize(bool bResume, int iWorldID, int iZoneID, int iInstanceID);
    int Run();

public:
    void OnTick();

    static CTimeValue m_stLotusMsgMaxProsessTime;
    static CTimeValue m_stWorldMsgMaxProcessTime;
    static CTimeValue m_stTickMaxProcessTime;

private:
    // ���ü���
    int LoadConfig();
    int ReloadConfig();

    // ͣ��
    bool Stop();

private:
    // �������������CAppLoop�У��ڹ����ڴ�ָ�
    CGameProtocolEngine m_stZoneProtocolEngine;
    CHandlerList m_stHandlerList;
    CSessionManager m_stSessionManager;
    CZoneObjectAllocator m_stAllocator;
    CAppTick m_stAppTick;

private:
    bool  m_bResumeMode;
    CTimeValue m_stStopService;
    timeval m_tvLastCheckTimeout;
    timeval m_tvLastCheckStat;
};

#endif


