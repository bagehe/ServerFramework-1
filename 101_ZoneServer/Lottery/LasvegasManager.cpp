
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "TimeUtility.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Resource/ResourceUtility.h"

#include "LasvegasManager.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CLasvegasManager* CLasvegasManager::Instance()
{
	static CLasvegasManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CLasvegasManager;
	}

	return pInstance;
}

CLasvegasManager::~CLasvegasManager()
{

}

CLasvegasManager::CLasvegasManager()
{
	Reset();
}

//��ʼ��
void CLasvegasManager::Init()
{
	Reset();
}

//��ҽ���ת��
void CLasvegasManager::EnterLasvegas(CGameRoleObj& stRoleObj, bool bIsEnter)
{
	if (bIsEnter)
	{
		//����
		UserBetData stUserData;
		stUserData.uin = stRoleObj.GetUin();
		if (std::find(m_vUserList.begin(), m_vUserList.end(), stUserData) != m_vUserList.end())
		{
			//�Ѿ�����
			return;
		}

		//��ҽ���ת��
		m_vUserList.push_back(stUserData);

		//����ת����Ϣ
		SendLasvegasInfoNotify(&stRoleObj);
	}
	else
	{
		//�뿪
		UserBetData stUserData;
		stUserData.uin = stRoleObj.GetUin();
		std::vector<UserBetData>::iterator it = std::find(m_vUserList.begin(), m_vUserList.end(), stUserData);
		if (it != m_vUserList.end())
		{
			//�˻������ע
			int iRetCoins = 0;
			for (unsigned i = 0; i < it->vBets.size(); ++i)
			{
				iRetCoins += it->vBets[i].iBetCoins;
			}

			CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, iRetCoins);

			m_vUserList.erase(it);
		}
	}

	return;
}

//�����ע
int CLasvegasManager::BetLasvegas(CGameRoleObj& stRoleObj, int iNumber, int iBetCoins)
{
	//�Ƿ�����ע�׶�
	if (m_iStepType != LASVEGAS_STEP_BET)
	{
		//������ע
		LOGERROR("Failed to bet lasvegas, invalid step %d, uin %u\n", m_iStepType, stRoleObj.GetUin());
		return T_ZONE_INVALID_LOTTERYSTEP;
	}

	//��ȡת������
	const LasvegasConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetLasvegasConfigByNumber(iNumber);
	if (!pstConfig || iBetCoins<=0)
	{
		LOGERROR("Failed to bet lasvegas, num %d, coins %d, uin %u\n", iNumber, iBetCoins, stRoleObj.GetUin());
		return T_ZONE_PARA_ERROR;
	}

	//��ȡ�����ע��Ϣ
	LasvegasBetData* pstBetData = NULL;
	for (unsigned i = 0; i < m_vUserList.size(); ++i)
	{
		if (m_vUserList[i].uin != stRoleObj.GetUin())
		{
			continue;
		}

		//���������ע��Ϣ
		for (unsigned j = 0; j < m_vUserList[i].vBets.size(); ++j)
		{
			if (m_vUserList[i].vBets[j].iNumber == iNumber)
			{
				//�ҵ���ע��Ϣ
				pstBetData = &m_vUserList[i].vBets[j];
				break;
			}
		}

		if (!pstBetData)
		{
			//�Ҳ�����ע��Ϣ������һ��
			LasvegasBetData stOneData;
			stOneData.iNumber = iNumber;

			m_vUserList[i].vBets.push_back(stOneData);

			pstBetData = &(*m_vUserList[i].vBets.rbegin());
		}

		break;
	}

	if (!pstBetData)
	{
		//�Ҳ�����Ϣ
		LOGERROR("Failed to get user bet data, uin %u, number %d\n", stRoleObj.GetUin(), iNumber);
		return T_ZONE_PARA_ERROR;
	}

	//�Ƿ�������ע��Ҫ��
	if (iBetCoins<pstConfig->iMinBet || (pstBetData->iBetCoins + iBetCoins) >pstConfig->iMaxBet)
	{
		LOGERROR("Failed to bet number %d, current bet coins %d, bet coins %d, uin %u\n", iNumber, pstBetData->iBetCoins, iBetCoins, stRoleObj.GetUin());
		return T_ZONE_PARA_ERROR;
	}

	//�۳���ҽ��
	if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, -iBetCoins))
	{
		//��Ǯʧ��
		LOGERROR("Failed to bet lasvegas, uin %u, num %d, bet coins %d\n", stRoleObj.GetUin(), iNumber, iBetCoins);
		return T_ZONE_PARA_ERROR;
	}

	//�����ע
	pstBetData->iBetCoins += iBetCoins;
	
	//��ӡ��Ӫ��־ �����ע
	CZoneOssLog::TraceLasvegasBet(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), iNumber, iBetCoins);

	//����World��ע��Ϣ
	SendUpdateBetInfoToWorld(iNumber, iBetCoins);

	return T_SERVER_SUCCESS;
}

//����ת����Ϣ
void CLasvegasManager::UpdateLasvegasInfo(const World_UpdateLasvegas_Notify& stNotify)
{
	int iOldStepType = m_iStepType;

	m_iStepType = stNotify.stinfo().isteptype();
	m_iStepEndTime = stNotify.stinfo().istependtime();

	//������¼
	m_vLotteryIDs.clear();
	for (int i = 0; i < stNotify.stinfo().ilotteryids_size(); ++i)
	{
		m_vLotteryIDs.push_back(stNotify.stinfo().ilotteryids(i));
	}

	//�н���Ϣ
	m_vPrizeDatas.clear();
	LotteryPrizeData stOnePrize;
	for (int i = 0; i < stNotify.stinfo().stprizes_size(); ++i)
	{
		stOnePrize.strNickName = stNotify.stinfo().stprizes(i).strname();
		stOnePrize.iNumber = stNotify.stinfo().stprizes(i).inumber();
		stOnePrize.iRewardCoins = stNotify.stinfo().stprizes(i).irewardcoins();

		m_vPrizeDatas.push_back(stOnePrize);
	}

	//��ע��Ϣ
	m_vBetDatas.clear();
	LasvegasBetData stOneBet;
	for (int i = 0; i < stNotify.stinfo().stbets_size(); ++i)
	{
		stOneBet.iNumber = stNotify.stinfo().stbets(i).inumber();
		stOneBet.iBetCoins = stNotify.stinfo().stbets(i).lbetcoins();

		m_vBetDatas.push_back(stOneBet);
	}

	if (iOldStepType != m_iStepType)
	{
		//�л��׶�,ǿ������ת����Ϣ
		SendLasvegasInfoNotify(NULL);

		if (m_iStepType == LASVEGAS_STEP_LOTTERY)
		{
			//���뿪���׶�

			//�����ע��Ϣ
			m_vBetDatas.clear();
			
			//���ÿ�������ʱ
			BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
			m_iLotteryRewardTime = CTimeUtility::GetNowTime() + stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LASVEGAS_REWARDTIME);

			m_bNeedSendReward = true;
			m_iLotteryID = *(m_vLotteryIDs.rbegin());
		}

		return;
	}

	//��Ҫ���͸��¸����
	m_bSendUserNotify = true;

	return;
}

//��ȡת���н���Ϣ
int CLasvegasManager::GetRewardInfo(CGameRoleObj& stRoleObj, Zone_GetRewardInfo_Response& stResp, int iFrom, int iNum)
{
	//������
	if (iFrom <= 0 || iNum <= 0)
	{
		LOGERROR("Failed to get lasvegas reward info, uin %u, from %d, num %d\n", stRoleObj.GetUin(), iFrom, iNum);
		return T_ZONE_PARA_ERROR;
	}

	for (int i = 0; i < iNum; ++i)
	{
		if ((iFrom + i) > (int)m_vPrizeDatas.size())
		{
			//�Ѿ�û��������
			break;
		}

		//��װ����
		PrizeInfo* pstOneInfo = stResp.add_stinfos();
		pstOneInfo->set_strname(m_vPrizeDatas[iFrom+i-1].strNickName);
		pstOneInfo->set_inumber(m_vPrizeDatas[iFrom + i - 1].iNumber);
		pstOneInfo->set_irewardcoins(m_vPrizeDatas[iFrom + i - 1].iRewardCoins);
		pstOneInfo->set_iindex(iFrom + i);
	}

	return T_SERVER_SUCCESS;
}

//��ʱ��
void CLasvegasManager::OnTick()
{
	int iTimeNow = CTimeUtility::GetNowTime();

	//����ת����Ϣ�����,2s����1��
	if (m_bSendUserNotify && (m_iLastSendNotifyTime + 2) < iTimeNow)
	{
		//����ת����Ϣ
		SendLasvegasInfoNotify(NULL);
	}

	//�Ƿ񷢷Ž���
	if (m_bNeedSendReward && m_iLotteryRewardTime <= iTimeNow)
	{
		//���Ž���
		SendLasvegasReward();
	}

	return;
}

//���Ž���
void CLasvegasManager::SendLasvegasReward()
{
	//��ȡ��������
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const LasvegasConfig* pstConfig = stBaseCfgMgr.GetLasvegasConfig(m_iLotteryID);
	if (!pstConfig)
	{
		LOGERROR("Failed to send lasvegas reward, invalid lottery id %d\n", m_iLotteryID);
		
		m_bNeedSendReward = false;
		m_iLotteryID = 0;
		return;
	}

	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_UPDATEPRIZEINFO_REQUEST);
	World_UpdatePrizeInfo_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_updateprizeinfo_request();

	for (unsigned i=0; i<m_vUserList.size(); ++i)
	{
		for (unsigned j = 0; j < m_vUserList[i].vBets.size(); ++j)
		{
			if (m_vUserList[i].vBets[j].iNumber != pstConfig->iNumber)
			{
				continue;
			}

			//�ҵ���Ӧ��ע,���ӽ���
			CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(m_vUserList[i].uin);
			if (pstRoleObj)
			{
				int iRewardCoins = m_vUserList[i].vBets[j].iBetCoins * (pstConfig->iNumber + 1);
				if (!CResourceUtility::AddUserRes(*pstRoleObj, RESOURCE_TYPE_COIN, iRewardCoins))
				{
					LOGERROR("Failed to add lasvegas reward, uin %u, coins %d\n", pstRoleObj->GetUin(), iRewardCoins);
					continue;
				}

				LasvegasBetData stBetDate = m_vUserList[i].vBets[j];

				//��ȡ��ǰ���������Ӫ��־ʹ��
				long8 lNewCoinNum = pstRoleObj->GetResource(RESOURCE_TYPE_COIN);

				//��ӡ��Ӫ��־  ��˹ά��˹ת��
				CZoneOssLog::TraceLasvegas(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), stBetDate.iNumber, stBetDate.iBetCoins,
					pstConfig->iNumber, iRewardCoins, lNewCoinNum - iRewardCoins, lNewCoinNum);

				//�Ƿ����ϰ�
				if (iRewardCoins >= stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LASVEGAS_REWARDNUM))
				{
					//�����ϰ�����
					PrizeInfo* pstOneInfo = pstReq->add_stprizes();
					pstOneInfo->set_strname(pstRoleObj->GetNickName());
					pstOneInfo->set_inumber(pstConfig->iNumber);
					pstOneInfo->set_irewardcoins(iRewardCoins);
				}
			}

			break;
		}

		//���������ע��Ϣ
		m_vUserList[i].vBets.clear();
	}

	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	//����Ϊ�ѷ���
	m_bNeedSendReward = false;
	m_iLotteryID = 0;

	return;
}

//����ת����Ϣ
void CLasvegasManager::SendLasvegasInfoNotify(CGameRoleObj* pstRoleObj)
{
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UPDATELASVEGAS_NOTIFY);
	
	Zone_UpdateLasvegas_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_updatelasvegas_notify();
	PackLasvegasInfo(*pstNotify->mutable_stinfo());

	if (!pstRoleObj)
	{
		//��ʱ���͵�
		TRoleObjList stRoleList;
		stRoleList.m_iRoleNumber = m_vUserList.size();
		for (unsigned i = 0; i < m_vUserList.size(); ++i)
		{
			stRoleList.m_apstRoleObj[i] = CUnitUtility::GetRoleByUin(m_vUserList[i].uin);
		}

		CZoneMsgHelper::SendZoneMsgToRoleList(stMsg, stRoleList);

		m_bSendUserNotify = false;
		m_iLastSendNotifyTime = CTimeUtility::GetNowTime();
	}
	else
	{
		//���͸�ָ�����
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);
	}

	return;
}

//����World��ע��Ϣ
void CLasvegasManager::SendUpdateBetInfoToWorld(int iNumber, int iBetCoins)
{
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_UPDATEBETINFO_REQUEST);

	World_UpdateBetInfo_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_updatebetinfo_request();
	NumberBetInfo* pstOneInfo = pstReq->add_stbetinfos();
	pstOneInfo->set_inumber(iNumber);
	pstOneInfo->set_lbetcoins(iBetCoins);

	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	return;
}

//���ת����Ϣ,�������н���¼
void CLasvegasManager::PackLasvegasInfo(LasvegasInfo& stInfo)
{
	stInfo.set_isteptype(m_iStepType);
	stInfo.set_istependtime(m_iStepEndTime);

	//������¼
	for (unsigned i = 0; i < m_vLotteryIDs.size(); ++i)
	{
		stInfo.add_ilotteryids(m_vLotteryIDs[i]);
	}

	//��ע��Ϣ
	for (unsigned i = 0; i < m_vBetDatas.size(); ++i)
	{
		NumberBetInfo* stOneInfo = stInfo.add_stbets();
		stOneInfo->set_inumber(m_vBetDatas[i].iNumber);
		stOneInfo->set_lbetcoins(m_vBetDatas[i].iBetCoins);
	}

	return;
}

void CLasvegasManager::Reset()
{
	//��ǰ�׶�
	m_iStepType = 0;

	//��ǰ�׶ν���ʱ��
	m_iStepEndTime = 0;

	//�Ƿ����͸��¸����
	m_bSendUserNotify = false;

	//������͸��¸���ҵ�ʱ��
	m_iLastSendNotifyTime = 0;

	//�Ƿ���Ҫ���Ž���
	m_bNeedSendReward = false;

	//�������Ž���ʱ��
	m_iLotteryRewardTime = 0;

	//���ο�����Ϣ
	m_iLotteryID = 0;

	//���������Ϣ
	m_vLotteryIDs.clear();

	//����н���¼
	m_vPrizeDatas.clear();

	//��ǰ��ע��Ϣ
	m_vBetDatas.clear();

	//ת������б�
	m_vUserList.clear();
}
