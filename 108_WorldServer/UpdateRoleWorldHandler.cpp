#include <string.h>

#include "ProtoDataUtility.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"

#include "UpdateRoleWorldHandler.hpp"

using namespace ServerLib;

CUpdateRoleWorldHandler::~CUpdateRoleWorldHandler()
{
}

int CUpdateRoleWorldHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_UPDATEROLE_REQUEST:
		{
			OnRequestUpdateRoleWorld();
			break;
		}

	case MSGID_WORLD_UPDATEROLE_RESPONSE:
		{
			OnResponseUpdateRoleWorld();
			break;
		}
	default:
		{
			break;
		}
	}

	return 0;
}

int CUpdateRoleWorldHandler::OnRequestUpdateRoleWorld()
{
	World_UpdateRole_Request* pstReq = m_pMsg->mutable_stbody()->mutable_m_stworld_updaterole_request();
	unsigned uiUin = pstReq->stroleid().uin();

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(pstReq->stroleid());
	if (!pWorldRoleStatusObj)
	{
		// Worldû��cache���ؽ���ɫ��Ϣ
		pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::CreateByUin(uiUin);
		if (!pWorldRoleStatusObj)
		{
			LOGERROR("CreateRoleStatusObj Failed: Uin = %u.\n", uiUin);
			SendUpdateRoleResponseWGS(pstReq->stroleid(), pstReq->ireqid(), T_WORLD_SERVER_BUSY);
			return -1;
		}

		pWorldRoleStatusObj->SetUin(pstReq->stroleid().uin());
		pWorldRoleStatusObj->SetZoneID(pstReq->ireqid());

		BASEDBINFO stBaseInfo;
		if(!DecodeProtoData(pstReq->stuserinfo().strbaseinfo(), stBaseInfo))
		{
			LOGERROR("Failed to decode base proto data, uin %u\n", uiUin);
			return -3;
		}

		unsigned int uStatus = stBaseInfo.ustatus();
		if(pstReq->bneedresponse())
		{
			uStatus &= ~(EGUS_ONLINE);
		}
		else
		{
			uStatus |= EGUS_ONLINE;
		}

		stBaseInfo.set_ustatus(uStatus);
		if(!EncodeProtoData(stBaseInfo, *pstReq->mutable_stuserinfo()->mutable_strbaseinfo()))
		{
			LOGERROR("Failed to encode proto data, uin %u\n", uiUin);
			return -5;
		}

		//���»�����Ϣ
		pWorldRoleStatusObj->SetRoleInfo(pstReq->stuserinfo());
	}

    LOGDEBUG("UpdateRole: Uin = %u\n", uiUin);

    // ���͸�������RoleDB
    int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("UpdateRoleWorld Failed: iRet = %d\n", iRet);

        if (pstReq->bneedresponse())
        {
            int iZoneID = pWorldRoleStatusObj->GetZoneID();
            SendUpdateRoleResponseWGS(pstReq->stroleid(), iZoneID, T_WORLD_SERVER_BUSY);
        }

        return -2;
    }

    return 0;
}

//�������ݷ��صĴ������
int CUpdateRoleWorldHandler::OnResponseUpdateRoleWorld()
{
	//ֱ��ת����Game Server
	const World_UpdateRole_Response& rstResp = m_pMsg->stbody().m_stworld_updaterole_response();
	CWorldRoleStatusWObj* pstUserStatus = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstResp.stroleid());
	if(!pstUserStatus)
	{
		LOGERROR("Fail to get world user status, uin %u\n", rstResp.stroleid().uin());
		return -1;
	}

	int iRet = SendUpdateRoleResponseWGS(rstResp.stroleid(), pstUserStatus->GetZoneID(), rstResp.iresult());

	return iRet;
}

int CUpdateRoleWorldHandler::SendUpdateRoleResponseWGS(const RoleID& stRoleID, int iZoneID, unsigned int uiResultID)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_UPDATEROLE_RESPONSE, stRoleID.uin());

	World_UpdateRole_Response* pstResp = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_updaterole_response();
	pstResp->mutable_stroleid()->CopyFrom(stRoleID);
	pstResp->set_iresult(uiResultID);
    
    CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iZoneID);
	return 0;
}




