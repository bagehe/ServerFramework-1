#include "AppTick.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"
#include "ObjStatistic.hpp"
#include "ModuleHelper.hpp"
#include "WorldLasvegasManager.h"
#include "WorldRankInfoManager.h"

CAppTick::CAppTick()
{
}

int CAppTick::Initialize(bool bResumeMode)
{
    m_tvLastBaseTick.RefreshTime();

    m_stZoneTick.Initialize(bResumeMode);
    m_stOnlineStatTick.Initialize();

    m_iLastMsgStatTick = time(NULL);

    return 0;
}

//static const char *aaanme[] = {"CGuildApplyRequest", "CRoleGuildMapping", "CGuildObj"};

//const int aaanme_num = sizeof(aaanme)/sizeof(aaanme[0]);
int CAppTick::CountObjStat()
{
    //int i = 0;
    //ObjectStatisticSingleton::Instance()->SetObjectStatName(aaanme, aaanme_num);

    return 0;
}

int CAppTick::OnTick()
{
    //ÿ��tick������ʱ�䣬��tickʹ��
    CTimeUtility::m_uiTimeTick = time(NULL);
    CTimeUtility::m_stTimeValueTick.RefreshTime();

    CTimeValue stNow = CTimeUtility::m_stTimeValueTick;

    CTimeValue tvSlape = stNow - m_tvLastBaseTick;
    int iMilliSeconds = tvSlape.GetTimeValue().tv_sec * 1000 + tvSlape.GetTimeValue().tv_usec / 1000;

    if (iMilliSeconds >= TICK_TIMEVAL_SECOND)
    {
		//1s��ʱ��

        m_stUnitTick.OnTick();

        //��ontick��ϳ��� ������Ҫͳ��
        m_stZoneTick.OnTick();

        //�ϱ�world״̬��cluster
        m_stOnlineStatTick.OnTick();

		//��ת��Tick
		CWorldLasvegasManager::Instance()->OnTick(CTimeUtility::m_uiTimeTick);

		//���а�Tick
		CWorldRankInfoManager::Instance()->OnTick(CTimeUtility::m_uiTimeTick);

		m_tvLastBaseTick.RefreshTime();
	}

    int iSlapTime =  CTimeUtility::m_uiTimeTick - m_iLastMsgStatTick;
    if (iSlapTime >= STATISTIC_TIME)
    {
        CountObjStat();
        ObjectStatisticSingleton::Instance()->Print();
        ObjectStatisticSingleton::Instance()->Reset();

        m_iLastMsgStatTick = CTimeUtility::m_uiTimeTick;
    }

    return 0;
}
