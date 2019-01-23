#include "WorldMsgHelper.hpp"
#include "ModuleHelper.hpp"
#include "UnixTime.hpp"
#include "WorldOnlineStatTick.hpp"

using namespace ServerLib;
const int MAX_ONLINE_ZONE_NUM = 3000;
const int MAX_REGISTER_WORLD_NUM = 1000000;
const int ONLINE_STAT_INTERVAL = 60; // 60s

int CWorldOnlineStatTick::Initialize()
{
    // ������������cluster����һ��������Ϣ, World���ø���Ϣ��Ϊcluster��������
    SendWorldOnlineStat();
    m_stLastTickTime = time(NULL);

    return 0;
}

int CWorldOnlineStatTick::OnTick()
{
    time_t stNow = time(NULL);
    int iSec = stNow - m_stLastTickTime;

    if (iSec >= ONLINE_STAT_INTERVAL)
    {
        SendWorldOnlineStat();

        m_stLastTickTime = stNow;
    }

    return 0;
}

int CWorldOnlineStatTick::SendWorldOnlineStat()
{
    CWorldMsgHelper::GenerateMsgHead(m_stMsg, 0, MSGID_WORLD_ONLINESTAT_REQUEST, 0);
    World_OnlineStat_Request* pstReqMsg = m_stMsg.mutable_stbody()->mutable_m_stworld_onlinestat_request();
    pstReqMsg->set_iworldid(CModuleHelper::GetWorldID());

    WorldOnlineInfo* pstWorldInfo = pstReqMsg->mutable_stworldinfo();
    pstWorldInfo->set_ionlinerolenum(CModuleHelper::GetZoneTick()->GetWorldOnlineNum());

    if (CModuleHelper::GetZoneTick()->GetActiveZoneNum() > 0)
    {
        pstWorldInfo->set_imaxonlinenum(MAX_ONLINE_ZONE_NUM*CModuleHelper::GetZoneTick()->GetActiveZoneNum());
    }
    else
    {    
        pstWorldInfo->set_imaxonlinenum(MAX_ONLINE_ZONE_NUM);
    }

    pstWorldInfo->set_iregisternum(0);
    pstWorldInfo->set_imaxregisternum(MAX_REGISTER_WORLD_NUM);

    pstReqMsg->set_urecordtime(time(NULL));

    // ������Ϣ
    for (int i = 1; i < MAX_ZONE_PER_WORLD; i++)
    {
        if (CModuleHelper::GetZoneTick()->IsZoneActive(i))
        {
            ZoneOnlineInfo* pstZoneInfo = pstWorldInfo->add_stzoneinfos();
            pstZoneInfo->set_izoneid(i);
            pstZoneInfo->set_irolenum(CModuleHelper::GetZoneTick()->GetZoneStatus(i).m_iOnlineNumber);
        }
    }

    //CWorldMsgHelper::SendWorldMsgToCluster(m_stMsg);

    return 0;
}
