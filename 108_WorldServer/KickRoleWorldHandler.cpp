#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "WorldRoleStatus.hpp"
#include "ErrorNumDef.hpp"

#include "KickRoleWorldHandler.hpp"

CKickRoleWorldHandler::~CKickRoleWorldHandler(void)
{
}

GameProtocolMsg CKickRoleWorldHandler::ms_stGameMsg;
int CKickRoleWorldHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pRequestMsg = (GameProtocolMsg*)pMsg;

	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_KICKROLE_REQUEST:
	{
		OnZoneRequestKickRole();
		break;
	}

	case MSGID_WORLD_KICKROLE_RESPONSE:
	{
		OnZoneResponseKickRole();
		break;
	}

	default:
	{
		break;
	}
	}

	return 0;
}

// ��������Zone����������
int CKickRoleWorldHandler::OnZoneRequestKickRole()
{
	const World_KickRole_Request& rstReq = m_pRequestMsg->stbody().m_stworld_kickrole_request();

	int iRet = 0;

	// �жϱ�world�ĵ�¼���
	LOGDEBUG("Recv Zone KickRole Req: From Uin = %u, Kicked Uin = %u, From ZoneID = %d\n", rstReq.stkicker().stroleid().uin(), rstReq.ukickeduin(), rstReq.stkicker().ifromzoneid());

	CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByUin(rstReq.ukickeduin());

	//������ڸ�world��¼, ��Zone������������
	if (pWorldRoleStatusWObj != NULL)
	{
		int iZoneID = pWorldRoleStatusWObj->GetZoneID();
		if (CModuleHelper::GetZoneTick()->IsZoneActive(iZoneID))
		{
			//ת����Zone����
			CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, iZoneID);
			LOGDEBUG("Send zone KickRole Req: iRet:%d, Kicked Uin = %u, zoneid:%d\n", iRet, rstReq.ukickeduin(), iZoneID);
			return 0;
		}
		else
		{
			// ֱ��ɾ����uin����Ϣ�����µ�¼
			WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(rstReq.stkicker().stroleid().uin());
		}
	}

	// ��Zone���ͻظ�
	CWorldMsgHelper::GenerateMsgHead(ms_stGameMsg, 0, MSGID_WORLD_KICKROLE_RESPONSE, 0);
	World_KickRole_Response* pstKickResp = ms_stGameMsg.mutable_stbody()->mutable_m_stworld_kickrole_response();
	pstKickResp->set_bislogin(rstReq.bislogin());
	pstKickResp->set_ukickeduin(rstReq.ukickeduin());
	pstKickResp->mutable_stkicker()->CopyFrom(rstReq.stkicker());

	pstKickResp->set_iresult(T_SERVER_SUCCESS);

	iRet = CWorldMsgHelper::SendWorldMsgToWGS(ms_stGameMsg, rstReq.stkicker().ifromzoneid());

	LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", rstReq.stkicker().stroleid().uin(), iRet);

	return 0;
}

// ��������Zone�ı��߻ظ�
int CKickRoleWorldHandler::OnZoneResponseKickRole()
{
	const World_KickRole_Response& rstResp = m_pRequestMsg->stbody().m_stworld_kickrole_response();
	unsigned int uin = rstResp.ukickeduin();
	int iRet = -1;

	LOGDEBUG("Recv Zone KickRole Resp: Uin = %u, result = %d\n", uin, rstResp.iresult());

	// ������˳ɹ�, ���ٴ�ȷ��Worldû������. �����, ��ǿ��ɾ����������.
	if (rstResp.iresult() == T_SERVER_SUCCESS)
	{
		CWorldRoleStatusWObj* pWorldRoleStatusWObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByUin(rstResp.ukickeduin());
		if (pWorldRoleStatusWObj)
		{
			WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uin);
		}
	}

	//��¼��zone�����˵�zone����һ��world
	if (rstResp.stkicker().ifromworldid() == CModuleHelper::GetWorldID())
	{
		// ��Zone���ͻظ�
		iRet = CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, rstResp.stkicker().ifromzoneid());

		LOGDEBUG("Send Zone KickRole Resp: Uin = %u, iRet = %d\n", uin, iRet);

	}
	//��world��¼�����ذ���
	else
	{

	}

	return iRet;
}
