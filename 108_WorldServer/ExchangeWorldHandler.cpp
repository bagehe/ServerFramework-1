#include <assert.h>
#include <string.h>

#include "GameProtocol.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "WorldExchangeLimitManager.h"

#include "ExchangeWorldHandler.h"

static GameProtocolMsg stMsg;

CExchangeWorldHandler::~CExchangeWorldHandler()
{

}

int CExchangeWorldHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pRequestMsg = (GameProtocolMsg*)pMsg;

	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_GETLIMITNUM_REQUEST:
	{
		//��ȡ�����һ���Ϣ
		OnRequestGetExchange();
	}
	break;

	case MSGID_WORLD_ADDLIMITNUM_REQUEST:
	{
		//�޸������һ���Ϣ
		OnRequestUpdateExchange();
	}
	break;

	case MSGID_WORLD_ONLINEEXCHANGE_REQUEST:
	{
		//���߶һ�����
		OnRequestOnlineExchange();
	}
	break;

	case MSGID_WORLD_ONLINEEXCHANGE_RESPONSE:
	{
		//���߶һ�����
		OnResponseOnlineExchange();
	}
	break;

	case MSGID_WORLD_ADDEXCREC_REQUEST:
	{
		//���Ӷһ���¼����
		OnRequestAddExchangeRec();
	}
	break;

	case MSGID_ZONE_GETEXCHANGEREC_REQUEST:
	{
		//��ȡ�һ���¼����
		OnRequestGetExchangeRec();
	}
	break;

	default:
		break;
	}

	return 0;
}

//��ȡ�����һ���Ϣ
int CExchangeWorldHandler::OnRequestGetExchange()
{
	LOGDEBUG("OnRequestGetExchange\n");

	const Zone_GetLimitNum_Request& stReq = m_pRequestMsg->stbody().m_stzone_getlimitnum_request();
	
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_ZONE_GETLIMITNUM_RESPONSE, stReq.uin());
	Zone_GetLimitNum_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getlimitnum_response();

	CWorldExchangeLimitManager::Instance()->GetExchangeLimit(*pstResp);
	pstResp->set_iresult(T_SERVER_SUCCESS);

	//���ͷ���
	CWorldMsgHelper::SendWorldMsgToWGS(stMsg, stReq.izoneid());

	return T_SERVER_SUCCESS;
}

//�޸������һ���Ϣ
int CExchangeWorldHandler::OnRequestUpdateExchange()
{
	LOGDEBUG("OnRequestUpdateExchange\n");

	//��ȡ����
	const World_AddLimitNum_Request& stReq = m_pRequestMsg->stbody().m_stworld_addlimitnum_request();

	//������Ϣ
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_WORLD_ADDLIMITNUM_RESPONSE, stReq.uin());
	World_AddLimitNum_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stworld_addlimitnum_response();
	pstResp->set_iexchangeid(stReq.iexchangeid());
	pstResp->set_uin(stReq.uin());
	pstResp->set_ireason(stReq.ireason());

	//�޸������һ�
	int iRet = CWorldExchangeLimitManager::Instance()->UpdateExchangeLimit(stReq.iexchangeid(), stReq.iaddnum());
	if (iRet)
	{
		LOGERROR("Failed to update exchange, ret %d, exchange id %d, add num %d\n", iRet, stReq.iexchangeid(), stReq.iaddnum());

		pstResp->set_iresult(iRet);
		CWorldMsgHelper::SendWorldMsgToWGS(stMsg, stReq.izoneid());
		return iRet;
	}

	//���ͳɹ��Ļظ�
	CWorldMsgHelper::SendWorldMsgToWGS(stMsg, stReq.izoneid());

	return T_SERVER_SUCCESS;;
}

//���߶һ�����
int CExchangeWorldHandler::OnRequestOnlineExchange()
{
	LOGDEBUG("OnRequestOnlineExchange\n");

	//ֱ��ת����ExchangeServer
	CWorldMsgHelper::SendWorldMsgToExchange(*m_pRequestMsg);

	return T_SERVER_SUCCESS;;
}

//���߶һ�����
int CExchangeWorldHandler::OnResponseOnlineExchange()
{
	const World_OnlineExchange_Response& stResp = m_pRequestMsg->stbody().m_stworld_onlineexchange_response();

	//ֱ��ת�������
	CWorldMsgHelper::SendWorldMsgToWGS(*m_pRequestMsg, stResp.ifromzoneid());

	return T_SERVER_SUCCESS;
}

//���Ӷһ���¼����
int CExchangeWorldHandler::OnRequestAddExchangeRec()
{
	LOGDEBUG("OnRequestAddExchangeRec\n");

	//��ȡ����
	const World_AddExcRec_Request& stReq = m_pRequestMsg->stbody().m_stworld_addexcrec_request();

	//����Ҫ���ͷ���

	//���Ӷһ���¼
	CWorldExchangeLimitManager::Instance()->AddExchangeRec(stReq.strecord());

	return T_SERVER_SUCCESS;
}

//��ȡ�һ���¼����
int CExchangeWorldHandler::OnRequestGetExchangeRec()
{
	LOGDEBUG("OnRequestGetExchangeRec\n");

	//��ȡ����
	const Zone_GetExchangeRec_Request& stReq = m_pRequestMsg->stbody().m_stzone_getexchangerec_request();

	//������Ϣ
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_ZONE_GETEXCHANGEREC_RESPONSE, 0);
	Zone_GetExchangeRec_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getexchangerec_response();
	pstResp->set_ifromindex(stReq.ifromindex());
	pstResp->set_inum(stReq.inum());
	pstResp->set_uin(stReq.uin());

	//��ȡ�һ���¼
	int iRet = CWorldExchangeLimitManager::Instance()->GetExchangeRec(stReq.ifromindex(), stReq.inum(), *pstResp);
	if (iRet)
	{
		LOGERROR("Failed to get exchange record, ret %d\n", iRet);
		CWorldMsgHelper::SendWorldMsgToWGS(stMsg, stReq.izoneid());
		return -1;
	}

	//��ȡ�ɹ�
	CWorldMsgHelper::SendWorldMsgToWGS(stMsg, stReq.izoneid());

	return T_SERVER_SUCCESS;
}
