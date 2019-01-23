#include <assert.h>
#include <string.h>

#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "WorldMailManager.hpp"

#include "GameMasterHandler.hpp"

static GameProtocolMsg stMsg;

CGameMasterHandler::~CGameMasterHandler()
{

}

int CGameMasterHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pRequestMsg = (GameProtocolMsg*)pMsg;

	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_GAMEMASTER_REQUEST:
	{
		//Worldת��GM����
		OnRequestGameMaster();
	}
	break;

	case MSGID_WORLD_GAMEMASTER_RESPONSE:
	{
		//World�յ�GM����
		OnResponseGameMaster();
	}
	break;

	default:
	{
		;
	}
	break;
	}

	return 0;
}

int CGameMasterHandler::OnRequestGameMaster()
{
	LOGDEBUG("OnRequestGameMaster\n");

	//��ȡ����
	GameMaster_Request* pstReq = m_pRequestMsg->mutable_stbody()->mutable_m_stworld_gamemaster_request();

	//������Ϣ
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_WORLD_GAMEMASTER_RESPONSE, 0);
	GameMaster_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stworld_gamemaster_response();
	pstResp->set_ioperatype(pstReq->ioperatype());
	pstResp->set_ufromuin(pstReq->ufromuin());
	pstResp->set_utouin(pstReq->utouin());

	int iRet = 0;
	if (pstReq->ioperatype() == GM_OPERA_SENDMAIL)
	{
		//GM�����ʼ�
		iRet = CWorldMailManager::Instance()->SendGMMail(*pstReq);
		
		pstResp->set_iresult(iRet);
		CWorldMsgHelper::SendWorldMsgToWGS(stMsg, pstReq->ifromzoneid());
		return T_SERVER_SUCCESS;
	}

	//��ȡ���������
	CWorldRoleStatusWObj* pstRoleStatObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByUin(pstReq->utouin());
	if (pstRoleStatObj)
	{
		//������ߣ�ת��Zone
		iRet = CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, pstRoleStatObj->GetZoneID());
	}
	else
	{
		//�����Ҫ�滻stHead�е�uinΪ����Ŀ���uin
		//ͬʱ��GM�����˵�uin���浽uFromUin��
		pstReq->set_ufromuin(m_pRequestMsg->sthead().uin());
		m_pRequestMsg->mutable_sthead()->set_uin(pstReq->utouin());

		//��Ҳ����ߣ�ת��RoleDB
		iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pRequestMsg);
	}

	if (iRet)
	{
		LOGERROR("Failed to send gm request, ret %d, target uin %u\n", iRet, pstReq->utouin());
		return iRet;
	}

	return 0;
}

int CGameMasterHandler::OnResponseGameMaster()
{
	//ֱ��ת����GMUser
	CWorldRoleStatusWObj* pstRoleStatObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByUin(m_pRequestMsg->sthead().uin());
	if (pstRoleStatObj)
	{
		CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, pstRoleStatObj->GetZoneID());
	}

	return 0;
}
