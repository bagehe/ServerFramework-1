
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Mail/MailUtility.h"

#include "ExchangeHandler.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CExchangeHandler::~CExchangeHandler()
{

}

int CExchangeHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_SETEXCHANGE_REQUEST:
	{
		//�޸��û���Ϣ
		OnRequestChangeUserInfo();
	}
	break;

	case MSGID_ZONE_EXCHANGEITEM_REQUEST:
	{
		//��Ҷһ�
		OnRequestExchange();
	}
	break;

	case MSGID_ZONE_GETLIMITNUM_REQUEST:
	{
		//�����ȡ����
		OnRequestGetLimitInfo();
	}
	break;

	case MSGID_ZONE_GETLIMITNUM_RESPONSE:
	{
		//World��ȡ��������
		OnResponseGetLimitInfo();
	}
	break;

	case MSGID_WORLD_ADDLIMITNUM_RESPONSE:
	{
		//World�޸������ķ���
		OnResponseUpdateLimit();
	}
	break;

	case MSGID_WORLD_GETCARDNO_RESPONSE:
	{
		//World��ȡ������Ϣ�ķ���
		OnResponseGetCardNo();
	}
	break;

	case MSGID_ZONE_GETEXCHANGEREC_REQUEST:
	{
		//�����ȡ�һ���¼������
		OnRequestGetExchangeRec();
	}
	break;

	case MSGID_ZONE_GETEXCHANGEREC_RESPONSE:
	{
		//World��ȡ�һ���¼�ķ���
		OnResponseGetExchangeRec();
	}
	break;

	default:
		break;
	}

	return 0;
}

//����޸���Ϣ������
int CExchangeHandler::OnRequestChangeUserInfo()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SETEXCHANGE_RESPONSE);
	Zone_SetExchange_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_setexchange_response();

	//��������
	const Zone_SetExchange_Request& stRequest = m_pRequestMsg->stbody().m_stzone_setexchange_request();
	iRet = m_pRoleObj->GetExchangeManager().SetUserExchangeInfo(stRequest.stuserinfo(), *pstResp);
	if (iRet)
	{
		LOGERROR("Failed to set user exchange info, uin %u, ret %d\n", m_pRoleObj->GetUin(), iRet);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//���ͷ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//��Ҷһ�������
int CExchangeHandler::OnRequestExchange()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_EXCHANGEITEM_RESPONSE);
	Zone_ExchangeItem_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_exchangeitem_response();

	//��������
	const Zone_ExchangeItem_Request& stRequest = m_pRequestMsg->stbody().m_stzone_exchangeitem_request();
	pstResp->mutable_storder()->set_iexchangeid(stRequest.iexchangeid());

	//��ȡ����
	const ExchangeConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetExchangeConfig(stRequest.iexchangeid());
	if (!pstConfig)
	{
		LOGERROR("Failed to exchange item, invalid exchange id %d, uin %u\n", stRequest.iexchangeid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//VIP�ȼ��Ƿ�����
	if (m_pRoleObj->GetVIPLevel() < (pstConfig->iVIPLv + 1))
	{
		//VIP�ȼ�������
		LOGERROR("Failed to exchange item, vip real:need %d:%d, uin %u\n", m_pRoleObj->GetVIPLevel(), (pstConfig->iVIPLv + 1), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -10;
	}

	//��ȡ�һ�������
	CExchangeManager& stExchangeMgr = m_pRoleObj->GetExchangeManager();

	if (pstConfig->stReward.iType == REWARD_TYPE_ENTITY && !stExchangeMgr.IsUserInfoSet())
	{
		//ʵ��һ������û���Ϣδ����
		LOGERROR("Failed to exchange item, user info not set, exchange id %d, uin %u\n", stRequest.iexchangeid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -10;
	}

	//�����Ҹ��˶һ�����
	if ((pstConfig->stReward.iRewardID == EXCHANGE_FIVEBILL_ID && !stExchangeMgr.CheckIsLimit(PERSONLIMIT_TYPE_FIVEBILL)) ||
		(pstConfig->stReward.iRewardID == EXCHANGE_TENBILL_ID && !stExchangeMgr.CheckIsLimit(PERSONLIMIT_TYPE_TENBILL)))
	{
		//��Ҹ�����������
		LOGERROR("Failed to exchange item, person limit check failed, exchange id %d, uin %u\n", stRequest.iexchangeid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PERSON_LIMIT);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -10;
	}

	//�ȿ�����
	//����������ֲ��ܹ�����Ҫ�˻�����
	iRet = stExchangeMgr.DoExchangeCost(*pstConfig);
	if (iRet)
	{
		//���ܿ۳�����
		LOGERROR("Failed to do exchange cost, ret %d, uin %u, exchange id %d\n", iRet, m_pRoleObj->GetUin(), stRequest.iexchangeid());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -3;
	}

	//��������
	if (pstConfig->iIsLimit)
	{
		//�����һ�
		CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_ADDLIMITNUM_REQUEST);
		World_AddLimitNum_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_addlimitnum_request();
		pstReq->set_ireason(LIMIT_REASON_EXCHANGE);
		pstReq->set_iexchangeid(stRequest.iexchangeid());
		pstReq->set_iaddnum(-1);	//�۳�1��
		pstReq->set_uin(m_pRoleObj->GetUin());
		pstReq->set_izoneid(CModuleHelper::GetZoneID());

		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

		return 0;
	}

	//�Ƿ�һ�����
	if (pstConfig->stReward.iType == REWARD_TYPE_CARDNO)
	{
		//���ͻ�ȡ��������
		iRet = CExchangeManager::SendGetCardNoRequest(m_pRoleObj->GetUin(), *pstConfig);
		if (iRet)
		{
			//��������
			stExchangeMgr.DoExchangeCost(*pstConfig, true);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -20;
		}

		return 0;
	}

	iRet = stExchangeMgr.ExchangeItem(*pstConfig, *pstResp->mutable_storder());
	if (iRet)
	{
		LOGERROR("Failed to exchange item, uin %u, ret %d\n", m_pRoleObj->GetUin(), iRet);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -4;
	}

	//���ͷ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//�����ȡ������������
int CExchangeHandler::OnRequestGetLimitInfo()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��������
	Zone_GetLimitNum_Request* pstReq = m_pRequestMsg->mutable_stbody()->mutable_m_stzone_getlimitnum_request();

	//��װ����
	pstReq->set_uin(m_pRoleObj->GetUin());
	pstReq->set_izoneid(CModuleHelper::GetZoneID());

	//ת������World
	CZoneMsgHelper::SendZoneMsgToWorld(*m_pRequestMsg);

	//��World���غ������������

	return T_SERVER_SUCCESS;
}

//World��ȡ�����ķ���
int CExchangeHandler::OnResponseGetLimitInfo()
{
	unsigned uin = m_pRequestMsg->sthead().uin();

	//ֱ��ת�����ͻ���
	CZoneMsgHelper::SendZoneMsgToRole(*m_pRequestMsg, CUnitUtility::GetRoleByUin(uin));

	return T_SERVER_SUCCESS;
}

//World���������ķ���
int CExchangeHandler::OnResponseUpdateLimit()
{
	//��ȡ����
	const World_AddLimitNum_Response& stResp = m_pRequestMsg->stbody().m_stworld_addlimitnum_response();

	int iRet = T_SERVER_SUCCESS;
	switch (stResp.ireason())
	{
	case LIMIT_REASON_EXCHANGE:
	{
		//��Ҷһ�
		CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_EXCHANGEITEM_RESPONSE);
		Zone_ExchangeItem_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_exchangeitem_response();

		CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stResp.uin());
		if (!pstRoleObj)
		{
			return -1;
		}

		//��ȡ�һ�����
		const ExchangeConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetExchangeConfig(stResp.iexchangeid());
		if (!pstConfig)
		{
			LOGERROR("Failed to get exchange config, invalid id %d, uin %u\n", stResp.iexchangeid(), stResp.uin());

			pstResp->set_iresult(T_ZONE_INVALID_CFG);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, CUnitUtility::GetRoleByUin(stResp.uin()));
			return -2;
		}

		CExchangeManager& stExchangeMgr = pstRoleObj->GetExchangeManager();
		if (stResp.iresult() != 0)
		{
			//�һ�ʧ��,��������
			iRet = stExchangeMgr.DoExchangeCost(*pstConfig, true);
			if (iRet)
			{
				LOGERROR("Failed to return exchange cost, ret %d, uin %u, exchange id %d\n", iRet, stResp.uin(), stResp.iexchangeid());
			}

			pstResp->set_iresult(stResp.iresult());
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
			return -3;
		}
		else
		{
			//����һ�

			//�Ƿ�һ�����
			if (pstConfig->stReward.iType == REWARD_TYPE_CARDNO)
			{
				//���ͻ�ȡ��������
				iRet = CExchangeManager::SendGetCardNoRequest(pstRoleObj->GetUin(), *pstConfig);
				if (iRet)
				{
					//��������
					stExchangeMgr.DoExchangeCost(*pstConfig, true);

					pstResp->set_iresult(iRet);
					CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
					return -20;
				}

				return 0;
			}

			iRet = stExchangeMgr.ExchangeItem(*pstConfig, *pstResp->mutable_storder());
			if (iRet)
			{
				LOGERROR("Failed to exchange item, uin %u, ret %d\n", pstRoleObj->GetUin(), iRet);

				pstResp->set_iresult(iRet);
				CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
				return -4;
			}

			//���ͷ���
			pstResp->set_iresult(T_SERVER_SUCCESS);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
		}
	}
	break;

	case LIMIT_REASON_GM:
	{
		//GM����Ա�޸�

	}
	break;

	default:
		break;
	}

	return T_SERVER_SUCCESS;
}

//World��ȡ������Ϣ�ķ���
int CExchangeHandler::OnResponseGetCardNo()
{
	//��ȡ����
	const World_GetCardNo_Response& stResp = m_pRequestMsg->stbody().m_stworld_getcardno_response();

	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stResp.uin());
	if (!pstRoleObj)
	{
		return -1;
	}

	//���ظ���ҵ���Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_EXCHANGEITEM_RESPONSE);
	Zone_ExchangeItem_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_exchangeitem_response();

	//��ȡ�һ�����
	const ExchangeConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetExchangeConfig(stResp.iexchangeid());
	if (!pstConfig)
	{
		LOGERROR("Failed to get exchange config, invalid id %d, uin %u\n", stResp.iexchangeid(), stResp.uin());

		pstResp->set_iresult(T_ZONE_INVALID_CFG);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
		return -3;
	}

	int iRet = T_SERVER_SUCCESS;
	CExchangeManager& stExchangeMgr = pstRoleObj->GetExchangeManager();
	if (stResp.iresult() != 0)
	{
		//��ȡ����ʧ�ܣ����ضһ�����
		iRet = stExchangeMgr.DoExchangeCost(*pstConfig, true);
		if (iRet)
		{
			LOGERROR("Failed to return exchange cost, ret %d, uin %u, exchange id %d\n", iRet, stResp.uin(), stResp.iexchangeid());
		}

		pstResp->set_iresult(stResp.iresult());
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
		return -5;
	}

	//��ȡ���ܳɹ�

	//���Ϳ����ʼ�
	iRet = CMailUtility::SendCardNoMail(*pstRoleObj, pstConfig->iID, stResp.strcardno(), stResp.strcardpwd());
	if (iRet)
	{
		LOGERROR("Failed to send card no mail, exchange id %d, ret %d, uin %u\n", pstConfig->iID, iRet, stResp.uin());

		pstResp->set_iresult(stResp.iresult());
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
		return -6;
	}

	//��ɶһ�����
	iRet = stExchangeMgr.ExchangeItem(*pstConfig, *pstResp->mutable_storder());
	if (iRet)
	{
		LOGERROR("Failed to exchange item, uin %u, ret %d\n", stResp.uin(), iRet);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
		return -7;
	}

	//���ͷ�����Ϣ
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	return 0;
}

//�����ȡ�һ���¼������
int CExchangeHandler::OnRequestGetExchangeRec()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��������
	Zone_GetExchangeRec_Request* pstReq = m_pRequestMsg->mutable_stbody()->mutable_m_stzone_getexchangerec_request();
	
	//������
	if (pstReq->ifromindex() <= 0 || pstReq->inum() <= 0)
	{
		LOGERROR("Failed to get exchange record, invalid param, uin %u\n", m_pRoleObj->GetUin());
		return -2;
	}

	//��װ����
	pstReq->set_uin(m_pRoleObj->GetUin());
	pstReq->set_izoneid(CModuleHelper::GetZoneID());

	//ת������World
	CZoneMsgHelper::SendZoneMsgToWorld(*m_pRequestMsg);

	//��World���غ������������

	return T_SERVER_SUCCESS;
}

//World��ȡ�һ���¼�ķ���
int CExchangeHandler::OnResponseGetExchangeRec()
{
	//��������
	const Zone_GetExchangeRec_Response& stResp = m_pRequestMsg->stbody().m_stzone_getexchangerec_response();

	//ֱ�ӷ��͸��ͻ���
	CZoneMsgHelper::SendZoneMsgToRole(*m_pRequestMsg, CUnitUtility::GetRoleByUin(stResp.uin()));

	return T_SERVER_SUCCESS;
}

