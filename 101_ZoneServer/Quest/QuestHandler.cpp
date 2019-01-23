
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"

#include "QuestHandler.h"

using namespace ServerLib;

GameProtocolMsg CQuestHandler::ms_stZoneMsg;

CQuestHandler::~CQuestHandler()
{

}

int CQuestHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_FINQUEST_REQUEST:
	{
		OnRequestFinQuest();
	}
	break;

	case MSGID_ZONE_GETLIVNESS_REQUEST:
	{
		//�����ȡ��Ծ�Ƚ���
		OnRequestGetLivnessReward();
	}
	break;

	default:
		break;
	}

	return 0;
}

//����������
int CQuestHandler::OnRequestFinQuest()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);

		SendResponse(MSGID_ZONE_FINQUEST_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);
		return -1;
	}

	//������������
	const Zone_FinQuest_Request& stRequest = m_pRequestMsg->stbody().m_stzone_finquest_request();

	//�������
	iRet = m_pRoleObj->GetQuestManager().FinQuest(stRequest.iquestid());
	if (iRet)
	{
		LOGERROR("Failed to fin quest, ret %d, uin %u, quest id %d\n", iRet, m_pRoleObj->GetUin(), stRequest.iquestid());

		SendResponse(MSGID_ZONE_FINQUEST_RESPONSE, iRet, *m_pNetHead);
		return -2;
	}
	
	//���ͳɹ��Ļظ�
	SendResponse(MSGID_ZONE_FINQUEST_RESPONSE, T_SERVER_SUCCESS, *m_pNetHead);

	return T_SERVER_SUCCESS;
}

//�����ȡ��Ծ�Ƚ���
int CQuestHandler::OnRequestGetLivnessReward()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);

		SendResponse(MSGID_ZONE_GETLIVNESS_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);
		return -1;
	}

	//��ȡ��Ծ�Ƚ�������
	const Zone_GetLivness_Request& stRequest = m_pRequestMsg->stbody().m_stzone_getlivness_request();

	//��ȡ��Ծ�Ƚ���
	iRet = m_pRoleObj->GetQuestManager().GetLivnessReward(stRequest.ilivnessrewardid());
	if (iRet)
	{
		LOGERROR("Failed to get livness reward, ret %d, uin %u, reward id %d\n", iRet, m_pRoleObj->GetUin(), stRequest.ilivnessrewardid());

		SendResponse(MSGID_ZONE_GETLIVNESS_RESPONSE, iRet, *m_pNetHead);
		return -2;
	}

	//���ͳɹ��Ļظ�
	SendResponse(MSGID_ZONE_GETLIVNESS_RESPONSE, T_SERVER_SUCCESS, *m_pNetHead);

	return T_SERVER_SUCCESS;
}

// ���ͻظ�
int CQuestHandler::SendResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, uiMsgID);

	switch (uiMsgID)
	{
	case MSGID_ZONE_FINQUEST_RESPONSE:
	{
		ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_finquest_response ()->set_iresult(uiResultID);
	}
	break;

	case MSGID_ZONE_GETLIVNESS_RESPONSE:
	{
		Zone_GetLivness_Response* pstResp = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_getlivness_response();
		pstResp->set_iresult(uiResultID);
		if (uiResultID == T_SERVER_SUCCESS)
		{
			pstResp->set_ilivnessrewardid(m_pRequestMsg->stbody().m_stzone_getlivness_request().ilivnessrewardid());
		}
	}
	break;

	default:
	{
		return -1;
	}
	break;
	}

	CZoneMsgHelper::SendZoneMsgToClient(ms_stZoneMsg, rstNetHead);

	return 0;
}

