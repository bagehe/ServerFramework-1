
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "Resource/ResourceUtility.h"
#include "Reward/RewardUtility.h"
#include "RechargeManager.h"

#include "RechargeHandler.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRechargeHandler::~CRechargeHandler()
{

}

int CRechargeHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_USERRECHARGE_REQUEST:
	{
		//ϵͳ��ֵ����
		OnRequestRecharge();
	}
	break;

	case MSGID_ZONE_GETPAYRECORD_REQUEST:
	{
		//�����ȡ��ֵ��¼
		OnRequestGetRecord();
	}
	break;

	case MSGID_ZONE_GETPAYGIFT_REQUEST:
	{
		//�����ȡ��ֵ���
		OnRequestGetPayGift();
	}
	break;

	case MSGID_ZONE_GETPAYORDER_REQUEST:
	{
		//��һ�ȡ����������
		OnRequestGetPayOrder();
	}
	break;

	case MSGID_ZONE_GETPAYORDER_RESPONSE:
	{
		//��һ�ȡ�����ŷ���
		OnResponseGetPayOrder();
	}
	break;

	case MSGID_ZONE_USERRECHARGE_REQUEST:
	{
		//��ҳ�ֵ������
		OnRequestRechargeCoin();
	}
	break;

	default:
		break;
	}

	return 0;
}

//ϵͳ��ֵ����
int CRechargeHandler::OnRequestRecharge()
{
	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_USERRECHARGE_RESPONSE);
	stMsg.mutable_sthead()->set_uisessionfd(m_pRequestMsg->sthead().uisessionfd());

	World_UserRecharge_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stworld_userrecharge_response();

	//��ȡ����
	const World_UserRecharge_Request& stReq = m_pRequestMsg->stbody().m_stworld_userrecharge_request();
	pstResp->set_uin(stReq.uin());
	pstResp->set_irechargeid(stReq.irechargeid());
	pstResp->set_strorderid(stReq.strorderid());
	pstResp->set_iplatform(stReq.iplatform());

	//��ȡ��ɫ
	m_pRoleObj = CUnitUtility::GetRoleByUin(stReq.uin());
	if (!m_pRoleObj)
	{
		LOGERROR("Failed to recharge user, uin %u, recharge id %d, order id %s\n", stReq.uin(), stReq.irechargeid(), stReq.strorderid().c_str());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return -1;
	}

	//��ȡ��ֵ������
	CRechargeManager& stRechargeMgr = m_pRoleObj->GetRechargeManager();
	int iRet = stRechargeMgr.UserRecharge(stReq.irechargeid(), stReq.itime(), stReq.strorderid());
	if (iRet)
	{
		LOGERROR("Failed to recharge user, uin %u, recharge id %d, order %s, ret %d\n", stReq.uin(), stReq.irechargeid(), stReq.strorderid().c_str(), iRet);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return -2;
	}

	//���ͳ�ֵ�ɹ��ķ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	return T_SERVER_SUCCESS;
}

//�����ȡ��ֵ��¼����
int CRechargeHandler::OnRequestGetRecord()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GETPAYRECORD_RESPONSE);
	Zone_GetPayRecord_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getpayrecord_response();

	//��ȡ��ֵ������
	CRechargeManager& stRechargeMgr = m_pRoleObj->GetRechargeManager();
	stRechargeMgr.GetPayRecord(*pstResp);

	//���ͷ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//�����ȡ��ֵ�������
int CRechargeHandler::OnRequestGetPayGift()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GETPAYGIFT_RESPONSE);
	Zone_GetPayGift_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getpaygift_response();

	//��ȡ����
	const Zone_GetPayGift_Request& stReq = m_pRequestMsg->stbody().m_stzone_getpaygift_request();
	pstResp->set_igifttype(stReq.igifttype());

	iRet = m_pRoleObj->GetRechargeManager().GetPayGift(stReq.igifttype());
	if (iRet)
	{
		LOGERROR("Failed to get pay gift, ret %d, type %d, uin %u\n", iRet, stReq.igifttype(), m_pRoleObj->GetUin());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//���ͷ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//��һ�ȡ������
int CRechargeHandler::OnRequestGetPayOrder()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��ȡ����
	Zone_GetPayOrder_Request* pstReq = m_pRequestMsg->mutable_stbody()->mutable_m_stzone_getpayorder_request();
	pstReq->set_uin(m_pRoleObj->GetUin());
	pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());

	//ֱ��ת����World
	CZoneMsgHelper::SendZoneMsgToWorld(*m_pRequestMsg);

	return T_SERVER_SUCCESS;
}

//��һ�ȡ�����ŷ���
int CRechargeHandler::OnResponseGetPayOrder()
{
	unsigned uiUin = m_pRequestMsg->sthead().uin();

	//ֱ��ת�����ͻ���
	CZoneMsgHelper::SendZoneMsgToRole(*m_pRequestMsg, CUnitUtility::GetRoleByUin(uiUin));

	return T_SERVER_SUCCESS;
}

//��ҽ�ҳ�ֵ����
int CRechargeHandler::OnRequestRechargeCoin()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��ȡ����
	const Zone_UserRecharge_Request& stReq = m_pRequestMsg->stbody().m_stzone_userrecharge_request();

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_USERRECHARGE_RESPONSE);
	Zone_UserRecharge_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_userrecharge_response();
	pstResp->set_irechargeid(stReq.irechargeid());

	//��ȡ����
	const RechargeConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetRechargeConfig(stReq.irechargeid());
	if (!pstConfig || pstConfig->stReward.iType != RECHARGE_TYPE_COIN)
	{
		LOGERROR("Failed to recharge coin, invalid config, recharge id %d\n", stReq.irechargeid());

		pstResp->set_iresult(T_ZONE_INVALID_CFG);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//�۳�����
	if (!CResourceUtility::AddUserRes(*m_pRoleObj, RESOURCE_TYPE_DIAMOND, -pstConfig->iCostRMB))
	{
		LOGERROR("Failed to recharge coin, diamonds need:real %d:%lld, uin %u\n", pstConfig->iCostRMB, m_pRoleObj->GetResource(RESOURCE_TYPE_DIAMOND), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -3;
	}

	//��ֵ���
	iRet = m_pRoleObj->GetRechargeManager().UserRecharge(stReq.irechargeid(), CTimeUtility::GetNowTime(), "");
	if (iRet)
	{
		LOGERROR("Failed to recharge coin, ret %d, recharge id %d, uin %u\n", iRet, stReq.irechargeid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -4;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}
