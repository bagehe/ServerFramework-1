
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Random.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Resource/ResourceUtility.h"
#include "RepThings/RepThingsUtility.hpp"
#include "Reward/RewardUtility.h"
#include "Kernel/ZoneOssLog.hpp"
#include "Mail/MailUtility.h"

#include "RechargeUtility.h"
#include "RechargeManager.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRechargeManager::CRechargeManager()
{
	Reset();
}

CRechargeManager::~CRechargeManager()
{
	Reset();
}

//��ʼ��
int CRechargeManager::Initialize()
{
	Reset();

	return T_SERVER_SUCCESS;
}

void CRechargeManager::SetOwner(unsigned int uin)
{
	m_uiUin = uin;
}

CGameRoleObj* CRechargeManager::GetOwner()
{
	return CUnitUtility::GetRoleByUin(m_uiUin);
}

//��ҳ�ֵ
int CRechargeManager::UserRecharge(int iRechargeID, int iTime, const std::string& strOrderID)
{
	CGameRoleObj* pstRoleObj = GetOwner();
	if (!pstRoleObj)
	{
		LOGERROR("Failed to get role obj, uin %u\n", m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	//��ȡ����
	const RechargeConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetRechargeConfig(iRechargeID);
	if (!pstConfig)
	{
		LOGERROR("Failed to get recharge config, id %d\n", iRechargeID);
		return T_ZONE_INVALID_CFG;
	}

	switch (pstConfig->stReward.iType)
	{
	case RECHARGE_TYPE_COIN:
	{
		//��ҳ�ֵ
		int iAddCoinNum = pstConfig->stReward.iRewardNum;

		//��ȡ��ֵ֮ǰ�Ľ��������Ӫ��־ʹ��
		long8 lOldCoinNum = pstRoleObj->GetResource(RESOURCE_TYPE_COIN);

		if (!CResourceUtility::AddUserRes(*pstRoleObj, RESOURCE_TYPE_COIN, iAddCoinNum))
		{
			LOGERROR("Failed to recharge coins, uin %u, recharge id %d, coins %d\n", m_uiUin, iRechargeID, iAddCoinNum);
			return T_ZONE_PARA_ERROR;
		}

		//��ӡ��Ӫ��־
		CZoneOssLog::TraceRecharge(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), iRechargeID, iAddCoinNum,
			lOldCoinNum, pstRoleObj->GetResource(RESOURCE_TYPE_COIN), pstRoleObj->GetOnlineTotalTime(), pstRoleObj->GetVIPLevel());
	}
	break;

	case RECHARGE_TYPE_MONTH:
	{
		//�¿���ֵ
		CRechargeUtility::AddMonthEndTime(*pstRoleObj, pstConfig->stReward.iRewardNum);

		//��ӡ��Ӫ��־
		CZoneOssLog::TraceRecharge(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), iRechargeID, 0,
			0, pstRoleObj->GetResource(RESOURCE_TYPE_COIN), pstRoleObj->GetOnlineTotalTime(), pstRoleObj->GetVIPLevel());
	}
	break;

	case RECHARGE_TYPE_ITEM:
	{
		//���߳�ֵ,Ŀǰû��
	}
	break;

	case RECHARGE_TYPE_DIAMOND:
	{
		//��ʯ��ֵ
		int iAddDiamondNum = pstConfig->stReward.iRewardNum;
		if (std::find(m_vRechargeIDs.begin(), m_vRechargeIDs.end(), iRechargeID) == m_vRechargeIDs.end())
		{
			//�׳����ʯ��Ŀ
			m_vRechargeIDs.push_back(iRechargeID);
			iAddDiamondNum += pstConfig->iExtraNum;
		}

		//��ȡ��ֵ֮ǰ����ʯ������Ӫ��־ʹ��
		long8 lOldDiamondNum = pstRoleObj->GetResource(RESOURCE_TYPE_DIAMOND);

		if (!CResourceUtility::AddUserRes(*pstRoleObj, RESOURCE_TYPE_DIAMOND, iAddDiamondNum))
		{
			LOGERROR("Failed to recharge diamonds, uin %u, recharge id %d, diamonds %d\n", m_uiUin, iRechargeID, iAddDiamondNum);
			return T_ZONE_PARA_ERROR;
		}

		//������ҳ�ֵ��ʯ��Ŀ
		pstRoleObj->GetTallyInfo().iRechargeDiamonds += iAddDiamondNum;

		//��ӡ��Ӫ��־
		CZoneOssLog::TraceRecharge(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), iRechargeID, iAddDiamondNum,
			lOldDiamondNum, pstRoleObj->GetResource(RESOURCE_TYPE_DIAMOND), pstRoleObj->GetOnlineTotalTime(), pstRoleObj->GetVIPLevel());
	}
	break;

	default:
	{
		LOGERROR("Failed to recharge, invalid type %d, uin %u, recharge id %d\n", pstConfig->stReward.iType, m_uiUin, iRechargeID);
		return T_ZONE_PARA_ERROR;
	}
	break;
	}

	if (pstConfig->stReward.iType != RECHARGE_TYPE_COIN)
	{
		//�����׳����״̬
		if (m_iFirstRewardStat == FIRST_REWARD_NOTOK)
		{
			m_iFirstRewardStat = FIRST_REWARD_OK;
		}

		//���ӳ�ֵ�齱����
		CResourceUtility::AddUserRes(*pstRoleObj, RESOURCE_TYPE_PAYSCORE, pstConfig->iCostRMB);

		//������ҳ�ֵ��¼
		AddRechargeRecord(iRechargeID, iTime);

		//����VIP���
		AddVipExp(pstConfig->iCostRMB * 10);
	}

	//���ͳ�ֵ��֪ͨ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_USERRECHARGE_NOTIFY);
	Zone_UserRecharge_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_userrecharge_notify();
	pstNotify->set_irechargeid(iRechargeID);

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	//���ϱ�GM��ֵ�ͽ�ҳ�ֵ
	if (pstConfig->stReward.iType != RECHARGE_TYPE_COIN && strOrderID.compare("GM") != 0)
	{
		//�ϱ�ħ����ֵ����
		CZoneOssLog::ReportUserPay(m_uiUin, pstRoleObj->GetAccount(), pstRoleObj->GetDeviceID(), pstRoleObj->GetChannel(), strOrderID.c_str(), pstConfig->iCostRMB, 1, pstRoleObj->GetSession()->GetClientIP());
	}

	return T_SERVER_SUCCESS;
}

//�����ȡ��¼
void CRechargeManager::GetPayRecord(Zone_GetPayRecord_Response& stResp)
{
	stResp.Clear();
	for (unsigned i = 0; i < m_vRecords.size(); ++i)
	{
		RechargeRecord* pstOneInfo = stResp.add_strecords();
		pstOneInfo->set_itime(m_vRecords[i].iTime);
		pstOneInfo->set_irechargeid(m_vRecords[i].iRechargeID);
	}

	return;
}

//�����ȡ��ֵ���
int CRechargeManager::GetPayGift(int iGiftType)
{
	//��ȡ���
	CGameRoleObj* pstRoleObj = GetOwner();
	if (!pstRoleObj)
	{
		LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	int iRet = T_SERVER_SUCCESS;
	switch (iGiftType)
	{
	case PAY_GIFT_FIRST:
	{
		//��ȡ�׳����
		if (m_iFirstRewardStat != FIRST_REWARD_OK)
		{
			//��������ȡ
			LOGERROR("Failed to get pay gift, type %d, first stat %d, uin %u\n", iGiftType, m_iFirstRewardStat, m_uiUin);
			return T_ZONE_PARA_ERROR;
		}

		iRet = CRepThingsUtility::OpenBoxGift(*pstRoleObj, CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_RECHARGE_BOXID));
		if (iRet)
		{
			LOGERROR("Failed to get pay gift, type %d, ret %d, uin %u\n", iGiftType, iRet, m_uiUin);
			return iRet;
		}

		//����Ϊ�Ѿ���ȡ
		m_iFirstRewardStat = FIRST_REWARD_GET;
	}
	break;

	case PAY_GIFT_MONTHCARD:
	{
		//��ȡ�¿����
		int iTimeNow = CTimeUtility::GetNowTime();
		
		if (iTimeNow > pstRoleObj->GetMonthEndTime())
		{
			//�¿��ѽ���
			LOGERROR("Failed to get month gift, end:now %d:%d, uin %u\n", pstRoleObj->GetMonthEndTime(), iTimeNow, m_uiUin);
			return T_ZONE_PARA_ERROR;
		}

		if (CTimeUtility::IsInSameDay(iTimeNow, pstRoleObj->GetLastMonthTime()))
		{
			//��������ȡ
			LOGERROR("Failed to get month gift, now:lastget %d:%d, uin %u\n", iTimeNow, pstRoleObj->GetLastMonthTime(), m_uiUin);
			return T_ZONE_PARA_ERROR;
		}

		pstRoleObj->SetLastMonthTime(iTimeNow);

		iRet = CRepThingsUtility::OpenBoxGift(*pstRoleObj, CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_MONTHCARD_BOXID));
	}
	break;

	default:
	{
		iRet = T_ZONE_PARA_ERROR;
	}
	break;
	}

	if (iRet)
	{
		LOGERROR("Failed to get pay gift, type %d, uin %u\n", iGiftType, m_uiUin);
		return iRet;
	}

	return T_SERVER_SUCCESS;
}

//���³�ֵ��DB
void CRechargeManager::UpdateRechargeToDB(RECHARGEDBINFO& stInfo)
{
	stInfo.set_ifirstrewardstat(m_iFirstRewardStat);

	for (unsigned i = 0; i < m_vRechargeIDs.size(); ++i)
	{
		stInfo.add_irechargeids(m_vRechargeIDs[i]);
	}

	for (unsigned i = 0; i < m_vRecords.size(); ++i)
	{
		RechargeRecord* pstOneRecord = stInfo.add_strecords();
		pstOneRecord->set_itime(m_vRecords[i].iTime);
		pstOneRecord->set_irechargeid(m_vRecords[i].iRechargeID);
	}

	return;
}

//��DB���س�ֵ
void CRechargeManager::InitRechargeFromDB(const RECHARGEDBINFO& stInfo)
{
	m_iFirstRewardStat = stInfo.ifirstrewardstat();

	m_vRechargeIDs.clear();
	for (int i = 0; i < stInfo.irechargeids_size(); ++i)
	{
		m_vRechargeIDs.push_back(stInfo.irechargeids(i));
	}

	m_vRecords.clear();
	PayRecordData stOneData;
	for (int i = 0; i < stInfo.strecords_size(); ++i)
	{
		stOneData.iTime = stInfo.strecords(i).itime();
		stOneData.iRechargeID = stInfo.strecords(i).irechargeid();

		m_vRecords.push_back(stOneData);
	}

	return;
}

//������ҳ�ֵ��¼
void CRechargeManager::AddRechargeRecord(int iRechargeID, int iTime)
{
	PayRecordData stOneData;
	stOneData.iTime = iTime;
	stOneData.iRechargeID = iRechargeID;

	m_vRecords.insert(m_vRecords.begin(), stOneData);

	if (m_vRecords.size() > (unsigned)MAX_RECHARGE_RECORD_NUM)
	{
		m_vRecords.erase(m_vRecords.begin()+MAX_RECHARGE_RECORD_NUM, m_vRecords.end());
	}

	return;
}

//����VIP����
void CRechargeManager::AddVipExp(int iAddExp)
{
	CGameRoleObj* pstRoleObj = GetOwner();
	if (!pstRoleObj)
	{
		LOGERROR("Failed to get game role obj, uin %u\n", m_uiUin);
		return;
	}

	//���Ӿ���
	pstRoleObj->AddVIPExp(iAddExp);

	//��ȡVIP����
	const VipLevelConfig* pstVIPConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetVipConfig(pstRoleObj->GetVIPLevel());
	while (pstVIPConfig)
	{
		if (pstVIPConfig->iNeedExp == 0)
		{
			//�ѵ���ߵȼ�
			break;
		}

		if (pstVIPConfig->iNeedExp > pstRoleObj->GetVIPExp())
		{
			//��������
			break;
		}

		//����
		pstRoleObj->SetVIPLevel(pstRoleObj->GetVIPLevel() + 1);
		pstRoleObj->SetUpdateRank(true);	//ǿ�Ƹ���������а���Ϣ
		pstVIPConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetVipConfig(pstRoleObj->GetVIPLevel());
		if (!pstVIPConfig)
		{
			break;
		}

		//������Ȩ
		int iVIPPriv = pstRoleObj->GetVIPPriv();
		for (unsigned i = 0; i < pstVIPConfig->vPrivs.size(); ++i)
		{
			iVIPPriv |= pstVIPConfig->vPrivs[i].iPrivType;

			if (pstVIPConfig->vPrivs[i].iPrivType == VIP_PRIV_GETITEM)
			{
				//���͹�������ʼ�
				RewardConfig stRewardConfig;
				stRewardConfig.iType = pstVIPConfig->vPrivs[i].aiParams[0];
				stRewardConfig.iRewardID = pstVIPConfig->vPrivs[i].aiParams[1];
				stRewardConfig.iRewardNum = pstVIPConfig->vPrivs[i].aiParams[2];

				int iRet = CMailUtility::SendVIPGiftMail(*pstRoleObj, stRewardConfig);
				if (iRet)
				{
					//ֻ��ӡ������־
					LOGERROR("Failed to send vip gift mail, ret %d, uin %u, vip level %d\n", iRet, pstRoleObj->GetUin(), pstRoleObj->GetVIPLevel());
				}
			}
		}

		pstRoleObj->SetVIPPriv(iVIPPriv);

		//��ȡV��ӦIP�ȼ���ϵͳ�ʼ�
		CMailUtility::GetSystemMail(*pstRoleObj, MAIL_REASON_VIPLV, 0);
	}

	//����������ӳ�ֵ����֪ͨ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_VIPEXPUPDATE_NOTIFY);
	Zone_VipExpUpdate_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_vipexpupdate_notify();
	
	//����Notify
	pstNotify->set_iviplevel(pstRoleObj->GetVIPLevel());
	pstNotify->set_ivipexp(pstRoleObj->GetVIPExp());
	pstNotify->set_ivippriv(pstRoleObj->GetVIPPriv());

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	return;
}

void CRechargeManager::Reset()
{
	m_uiUin = 0;
	m_iFirstRewardStat = FIRST_REWARD_NOTOK;

	m_vRechargeIDs.clear();
	m_vRecords.clear();
}
