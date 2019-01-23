
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "Resource/ResourceUtility.h"
#include "Reward/RewardUtility.h"

#include "RewardHandler.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRewardHandler::~CRewardHandler()
{

}

int CRewardHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_GETLOGINREWARD_REQUEST:
	{
		//�����ȡ��¼����
		OnRequestGetLoginReward();
	}
	break;

	default:
		break;
	}

	return 0;
}

//�����ȡ��¼����
int CRewardHandler::OnRequestGetLoginReward()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GETLOGINREWARD_RESPONSE);
	Zone_GetLoginReward_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getloginreward_response();

	//��ȡ����
	const Zone_GetLoginReward_Request& stReq = m_pRequestMsg->stbody().m_stzone_getloginreward_request();
	pstResp->set_irewardid(stReq.irewardid());

	//�Ƿ������¼����
	if (m_pRoleObj->GetLoginDays() < stReq.irewardid())
	{
		//������������ȡ
		LOGERROR("Failed to get login reward, login days need:real %d:%d, uin %u\n", stReq.irewardid(), m_pRoleObj->GetLoginDays(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -6;
	}

	//��ȡ����
	const LoginRewardConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetLoginRewardConfig(stReq.irewardid());
	if (!pstConfig)
	{
		LOGERROR("Failed to get login reward, uin %u, reward id %d\n", m_pRoleObj->GetUin(), stReq.irewardid());
		
		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//�Ƿ�����ȡ
	if (m_pRoleObj->HasGetLoginReward(stReq.irewardid()))
	{
		//�Ѿ���ȡ��
		LOGERROR("Failed to get login reward, already get it, uin %u, id %d\n", m_pRoleObj->GetUin(), stReq.irewardid());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -3;
	}

	//��ȡ����
	iRet = CRewardUtility::GetReward(*m_pRoleObj, 1, &pstConfig->stReward, 1, ITEM_CHANNEL_LOGINADD);
	if (iRet)
	{
		LOGERROR("Failed to get login reward, uin %u, id %d, ret %d\n", m_pRoleObj->GetUin(), stReq.irewardid(), iRet);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -4;
	}

	//����Ϊ����ȡ
	m_pRoleObj->SetLoginRewardStat(stReq.irewardid());

	//��ȡ��һ�ý�����Ӧ����
	long8 lNewRewardNum = 0;
	switch (pstConfig->stReward.iType)
	{
	case REWARD_TYPE_RES:
	{
		lNewRewardNum = m_pRoleObj->GetResource(pstConfig->stReward.iRewardID);
	}
	break;

	case REWARD_TYPE_ITEM:
	{
		lNewRewardNum = m_pRoleObj->GetRepThingsManager().GetRepItemNum(pstConfig->stReward.iRewardID);
	}
	default:
		break;
	}

	//��ӡ��Ӫ��־
	CZoneOssLog::TraceLoginReward(m_pRoleObj->GetUin(), m_pRoleObj->GetChannel(), m_pRoleObj->GetNickName(), pstConfig->stReward.iRewardID, stReq.irewardid(),
		lNewRewardNum - pstConfig->stReward.iRewardNum, lNewRewardNum);

	//���ͳɹ��Ļظ�
	pstResp->set_iresult(T_SERVER_SUCCESS);
	pstResp->set_irewardid(stReq.irewardid());
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return 0;
}
