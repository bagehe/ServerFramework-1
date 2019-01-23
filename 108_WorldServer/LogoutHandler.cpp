#include <string.h>

#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"

#include "LogoutHandler.hpp"

CLogoutHandler::CLogoutHandler(void)
{
}

CLogoutHandler::~CLogoutHandler(void)
{
}

GameProtocolMsg CLogoutHandler::m_stWorldMsg;
int CLogoutHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pRequestMsgWGS = (GameProtocolMsg*)pMsg;

    switch (m_pRequestMsgWGS->sthead().uimsgid())
    {
    case MSGID_ZONE_LOGOUT_NOTIFY:
        {
            OnLogoutNotify();
            break;
        }
    default:
        {
            break;
        }
    }

    return 0;
}

int CLogoutHandler::OnLogoutNotify()
{
    const Zone_Logout_Notify& rstNotify = m_pRequestMsgWGS->stbody().m_stzone_logout_notify();
    CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstNotify.stroleid());
    if (!pWorldRoleStatusWObj)
    {
        return -1;
    }

    LogoutNotify(pWorldRoleStatusWObj);

    return 0;
}

// ֪ͨZone��������ɫ����
int CLogoutHandler::LogoutRole(int iZoneID, const unsigned int uin)
{
    //todo jasonxiong ��ʱ��ע�͵����������ʵ�֣����������Ҫʵ��������
    /*
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, time(NULL), M, rstRoleID.m_uiUin);
    KICKROLE_WORLD_REQUEST& rstWorldRequest = m_stWorldMsg.m_stMsgBody.m_stKickRoleWorld_Request;

    // ��Zone������������
    CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iZoneID);

    return 0;
    */

    return 0;
}

int CLogoutHandler::LogoutNotify(CWorldRoleStatusWObj* pRoleObj)
{
    unsigned int uiUin = pRoleObj->GetUin();

    LOGDEBUG("LogoutNotify: Uin = %u\n", uiUin);

    /*
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_ZONE_LOGOUT_NOTIFY, uiUin);

    Zone_Logout_Notify* pstNotify = m_stWorldMsg.mutable_stbody()->mutable_m_stzone_logout_notify();

    //������cluster����һ��
	//CWorldMsgHelper::SendWorldMsgToCluster(m_stWorldMsg);
    */

    WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);

    return 0;
}





