
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/GameEventManager.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Resource/ResourceUtility.h"
#include "RepThings/RepThingsUtility.hpp"
#include "Chat/ChatUtility.h"

#include "FishUtility.h"
#include "FishAlgorithm.h"

#include "FishpondObj.h"

using namespace ServerLib;

//���͸��ͻ��˵���Ϣ
GameProtocolMsg CFishpondObj::ms_stZoneMsg;

IMPLEMENT_DYN(CFishpondObj)

CFishpondObj::CFishpondObj()
{
	Reset();
}

CFishpondObj::~CFishpondObj()
{
	Reset();
}

int CFishpondObj::Initialize()
{
	return T_SERVER_SUCCESS;
}

int CFishpondObj::Resume()
{
	return T_SERVER_SUCCESS;
}

//���������Ϣ
void CFishpondObj::SetTableInfo(unsigned uTableUniqID, int iFishRoomID, const FishRoomConfig& stConfig)
{
	//���ID
	m_uTableID = uTableUniqID;

	//������ڷ���ID
	m_iFishRoomID = iFishRoomID;

	//�������
	m_pstRoomConfig = &stConfig;

	return;
}

//��ҽ������
int CFishpondObj::EnterFishpond(CGameRoleObj& stRoleObj)
{
	//�������
	int iRet = PlayerSitDown(stRoleObj);
	if (iRet)
	{
		LOGERROR("Failed to sit in fishpond, room id %d, uin %u, ret %d\n", m_pstRoomConfig->iRoomID, stRoleObj.GetUin(), iRet);
		return iRet;
	}

	if (m_vSeatUserData.size() <= 1)
	{
		//���³�ʼ����
		iRet = InitFishInfo();
		if (iRet)
		{
			LOGERROR("Failed to init fishpond data, table id %d, ret %d\n", m_uTableID, iRet);
			return iRet;
		}

		LOGDEBUG("Success to init fishpond data, table id %d!\n", m_uTableID);
	}

	//���ͷ�����ʱ������
	CUnitUtility::SendSyncTimeNotify(&stRoleObj, CTimeUtility::GetMSTime());

	//�����Լ���λ��Ϣ������
	SendSeatUserInfo(NULL, &stRoleObj);

	//����������Ϣ�����
	SendFishpondInfoToUser(stRoleObj);

	//��������ʱ��
	SendFishFormTime(&stRoleObj, true);
	SendFishFormTime(&stRoleObj, false);

	//����������ϸ��Ϣ
	SendFishFormInfo(&stRoleObj);

	//�����������
	stRoleObj.SetTableID(m_uTableID);

	//��ȡ�������ù�����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	//�������������
	++stData.lPlayingNum;

	//����ȫ����̨����
	const GunConfig* pstGunConfig = stBaseCfgMgr.GetGunConfig(stRoleObj.GetWeaponInfo().iWeaponID);
	if (pstGunConfig)
	{
		stData.lTotalGunMultiple += pstGunConfig->iMultiple;
	}

	//��ӡ��Ӫ��־ todo jasonxiong �����ͳһ���
	//pGameLogic->OSSUserEnterLeaveTable(player, true, pstUserInfo->lTotalSilver, pstUserInfo->lTokens);

	return T_SERVER_SUCCESS;
}

//����˳����
void CFishpondObj::ExitFishpond(CGameRoleObj& stRoleObj, bool bForceExit)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return;
	}

	//ɾ����������ӵ�
	ClearUserBullets(pstUserData->iSeat);

	//������ڽ�����־
	CZoneOssLog::TraceCycleProfit(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), -pstUserData->iCycleWinNum, pstUserData->lCycleNewUsedNum,
		stRoleObj.GetResource(RESOURCE_TYPE_COIN), stRoleObj.GetResource(RESOURCE_TYPE_TICKET));

	//��ȡ�������ù�����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	//����ȫ����̨����
	const GunConfig* pstGunConfig = stBaseCfgMgr.GetGunConfig(stRoleObj.GetWeaponInfo().iWeaponID);
	if (pstGunConfig && stData.lTotalGunMultiple >= pstGunConfig->iMultiple)
	{
		stData.lTotalGunMultiple -= pstGunConfig->iMultiple;
	}

	//�����˳�������Ϣ
	SendExitFishpondAll(pstUserData->iSeat, bForceExit);

	//ɾ������������
	for (unsigned i = 0; i < m_vSeatUserData.size(); ++i)
	{
		if (m_vSeatUserData[i].iSeat == pstUserData->iSeat)
		{
			m_vSeatUserData.erase(m_vSeatUserData.begin() + i);
			break;
		}
	}

	stRoleObj.SetTableID(0);

	//������������
	--stData.lPlayingNum;

	//��ӡ��Ӫ��־ todo jasonxiong ͳһ��������
	//m_pGameLogic->OSSUserEnterLeaveTable(player, false, pstUserData->lTotalSilver, pstUserData->lTokens);

	return;
}

//����л���̨
int CFishpondObj::ChangeGun(CGameRoleObj& stRoleObj, int iNewGunID, bool bIsStyle)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		//���ڸ������
		LOGERROR("Failed to change gun, table id %u, uin %u\n", m_uTableID, stRoleObj.GetUin());
		return T_ZONE_PARA_ERROR;
	}

	//ˢ�»�Ծʱ��
	pstUserData->iActiveTime = CTimeUtility::GetNowTime();

	if (bIsStyle)
	{
		//�л���̨��ʽ
		TWEAPONINFO& stWeaponInfo = stRoleObj.GetWeaponInfo();

		bool bHasStyle = false;
		for (unsigned i = 0; i < sizeof(stWeaponInfo.aiUnlockStyleIDs) / sizeof(int); ++i)
		{
			if (stWeaponInfo.aiUnlockStyleIDs[i] == 0)
			{
				break;
			}

			if (stWeaponInfo.aiUnlockStyleIDs[i] == iNewGunID)
			{
				//�Ѿ�����
				bHasStyle = true;
				break;
			}
		}

		if (!bHasStyle)
		{
			LOGERROR("Failed to change gun style, new style id %d, uin %u\n", iNewGunID, stRoleObj.GetUin());
			return T_ZONE_PARA_ERROR;
		}

		//�л���ʽ
		stWeaponInfo.iStyleID = iNewGunID;
	}
	else
	{
		//�л���̨����

		if (iNewGunID<m_pstRoomConfig->iMinBatteryID || iNewGunID>m_pstRoomConfig->iMaxBatteryID)
		{
			LOGERROR("Failed to change gun, invalid gun id %d\n", iNewGunID);
			return T_ZONE_PARA_ERROR;;
		}

		//��ȡ�������ù�����
		BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

		//��ȡ��̨����
		const GunConfig* pstConfig = stBaseCfgMgr.GetGunConfig(iNewGunID);
		if (!pstConfig)
		{
			//�Ҳ�������
			return T_ZONE_INVALID_CFG;
		}

		//��ȡ�ϵ���̨����
		const GunConfig* pstOldConfig = stBaseCfgMgr.GetGunConfig(stRoleObj.GetWeaponInfo().iWeaponID);
		if (!pstOldConfig)
		{
			//�Ҳ�������
			return T_ZONE_INVALID_CFG;
		}

		stRoleObj.GetWeaponInfo().iWeaponID = iNewGunID;
		long8 lTimeNow = CTimeUtility::GetMSTime();
		if (pstOldConfig->iMultiple < pstConfig->iMultiple && pstUserData->lUnEffectTime >= lTimeNow)
		{
			//��������̨�����к��,���ʧЧ
			pstUserData->lUnEffectTime = 0;
		}

		//�޸�ȫ����̨����
		FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType].lTotalGunMultiple += (pstConfig->iMultiple - pstOldConfig->iMultiple);
	}

	//������̨�л���֪ͨ
	SendChangeGunNotify(*pstUserData, iNewGunID, bIsStyle);

	return T_SERVER_SUCCESS;
}

//��ҷ����ӵ�
int CFishpondObj::ShootBullet(CGameRoleObj& stRoleObj, long lShootTime, int iPosX, int iPosY, bool bAutoShoot)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return T_ZONE_PARA_ERROR;
	}

	long lTimeNow = CTimeUtility::GetMSTime();

	//ˢ�»�Ծʱ��
	pstUserData->iActiveTime = lTimeNow/1000;

	if ((bAutoShoot && !pstUserData->bAutoShoot))
	{
		//�Զ�����״̬����
		LOGERROR("Failed to shoot bullet, uin %u, auto shoot server:client %d:%d\n", stRoleObj.GetUin(), pstUserData->bAutoShoot, bAutoShoot);
		return T_ZONE_PARA_ERROR;
	}

	/*
	if(abs(iPosX)!=CLIENT_SCREEN_WIDTH/2 && abs(iPosY)!=CLIENT_SCREEN_HEIGHT/2)
	{
	//Ŀ��λ�ò���
	return GAME_ERRORNO_INVALIDPOS;
	}
	*/

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//�Ƿ񳬹������ӵ��������
	if (pstUserData->iBulletNum >= stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MAXBULLETNUM))
	{
		//�������������
		return T_ZONE_INVALID_BULLETNUM;
	}

	//�Ƿ���Ϸ����ٶ�Ҫ��
	if ((pstUserData->alShootTime[pstUserData->iIndex] + 1000) >= lTimeNow)
	{
		//�������ٶ�Ҫ��
		return T_ZONE_INVALID_BULLETNUM;
	}

	//�жϻ����Ƿ�����Ҫ��
	const GunConfig* pstGunConfig = stBaseCfgMgr.GetGunConfig(stRoleObj.GetWeaponInfo().iWeaponID);
	if (!pstGunConfig)
	{
		return T_ZONE_INVALID_CFG;
	}

	//�Ƿ��
	if (pstUserData->lWildEndTime + 100 < lTimeNow)
	{
		//���ڿ�״̬
		pstUserData->iWildNum = 0;
	}

	int iConsume = (pstUserData->iWildNum == 0) ? pstGunConfig->iConsume : (pstGunConfig->iConsume*pstUserData->iWildNum * 2);
	if (stRoleObj.GetResource(RESOURCE_TYPE_COIN) < iConsume)
	{
		if (stRoleObj.GetResource(RESOURCE_TYPE_COIN) <= 0 ||
			stRoleObj.GetWeaponInfo().iWeaponID != m_pstRoomConfig->iMinBatteryID)
		{
			return T_ZONE_INVALID_NOSILVER;
		}
		else
		{
			iConsume = stRoleObj.GetResource(RESOURCE_TYPE_COIN);
		}
	}

	//�۳�����
	if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, -iConsume))
	{
		//�۳�����ʧ��
		LOGERROR("Failed to add user silver, uin %u, add silver %d\n", stRoleObj.GetUin(), -iConsume);
		return T_ZONE_INVALID_NOSILVER;
	}

	pstUserData->lUserSilverCost += iConsume;	//������һ�������
	pstUserData->lUserFlow[CYCLE_TIME_NOW] += iConsume;
	pstUserData->iLossNum += iConsume;
	pstUserData->iCycleWinNum -= iConsume;

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];
	stData.lTotalRewardSilver += iConsume;
	stData.lRewardSilver += iConsume;
	stData.lTodayRewardSilver += iConsume;

	//���ܷ�����ӵ���
	++stData.lTotalBulletNum;

	//���º����Ϣ
	UpdateRedPacketInfo(stRoleObj, *pstUserData);

	//�����ӵ���Ϣ
	BulletData stBulletInfo;
	stBulletInfo.uUniqueID = CFishUtility::GetBulletUniqID();
	stBulletInfo.iGunID = stRoleObj.GetWeaponInfo().iWeaponID;
	stBulletInfo.iSeat = pstUserData->iSeat;
	stBulletInfo.stTargetPos.iX = iPosX;
	stBulletInfo.stTargetPos.iY = iPosY;
	stBulletInfo.lShootTime = lShootTime;
	stBulletInfo.uFishUniqID = (pstUserData->lAimEndTime + 100 < lTimeNow) ? 0 : pstUserData->uAimFishUniqID;
	stBulletInfo.iWildNum = pstUserData->iWildNum;
	stBulletInfo.iCost = iConsume;
	stBulletInfo.iFishIndex = pstUserData->iAimFishIndex;

	m_vBulletData.push_back(stBulletInfo);

	//��ӡ��Ӫ��־
	CZoneOssLog::TraceShootBullet(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), iConsume, m_pstRoomConfig->iRoomID);

	//������ҷ�����Ϣ
	++pstUserData->iBulletNum;

	pstUserData->alShootTime[pstUserData->iIndex] = lTimeNow;
	pstUserData->iIndex = (++pstUserData->iIndex) % MAX_BULLET_PER_SECOND;

	//��������ش���
	const ExpLineConfig* pstExpLineConfig = NULL;
	TEXPLINEINFO* pstExpLineInfo = GetRoleExpLineInfo(stRoleObj.GetUin(), pstExpLineConfig);
	if (pstExpLineInfo)
	{
		pstExpLineInfo->lUserWinNum -= iConsume;
		pstExpLineInfo->lIntervalWinNum -= iConsume;
		pstExpLineInfo->lCostNum += iConsume;
		++pstExpLineInfo->iBulletNum;
	}

	//�������ֺ��
	if (stRoleObj.GetNowNewRedNum() == 0 && stRoleObj.GetRemainNewRedNum()>0 &&
		stRoleObj.GetResource(RESOURCE_TYPE_COIN) <= stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREMAINCOINS))
	{
		int iAddNewRedNum = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREDNUM) / stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREDTIMES);
		if (stRoleObj.GetRemainNewRedNum() >= iAddNewRedNum)
		{
			//����������ֺ�����ö��
			stRoleObj.SetRemainNewRedNum(stRoleObj.GetRemainNewRedNum() - iAddNewRedNum);
			stRoleObj.SetNowNewRedNum(stRoleObj.GetNowNewRedNum() + iAddNewRedNum);
		}
	}

	//LOGDEBUG("ShootBullet, uin %u, seat %d, shoot time %ld, pos %d:%d, cost %d\n",
	//	stRoleObj.GetUin(), pstUserData->iSeat, lShootTime, iPosX, iPosY, iConsume);

	//todo jasonxiong ͳһ������
	//��ӡ��Ӫ��־ ��ҷ����ӵ� ID = 2
	//m_pGameLogic->OSSUserShootBullet(player, pstUserData->iGunID, stBulletInfo.uUniqueID, iConsume);

	//�����ӵ���Ϣ
	SendShootBulletNotify(stBulletInfo);

	if (!IsRoomPattern(ROOM_PATTERN_WARHEAD))
	{
		//�����ӵ���֪ͨ,�ǵ�ͷ����֪ͨ
		CGameEventManager::NotifyShootBullet(stRoleObj, stRoleObj.GetWeaponInfo().iWeaponID, iConsume);
	}

	return T_SERVER_SUCCESS;
}

//���������
int CFishpondObj::HitFish(CGameRoleObj& stRoleObj, long lHitTime, unsigned uBulletUniqID, unsigned uFishUniqID, int iFishIndex)
{
	int iFishID = 0;
	int iCostSilver = 0;
	int iRewardSilver = 0;

	//��ȡ�����Ϣ
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return T_ZONE_PARA_ERROR;
	}

	long lTimeNow = CTimeUtility::GetMSTime();

	//ˢ�»�Ծʱ��
	pstUserData->iActiveTime = lTimeNow / 1000;

	//��ȡ�ӵ���Ϣ
	BulletData* pBulletInfo = GetBulletData(uBulletUniqID);
	if (!pBulletInfo || pBulletInfo->iSeat != pstUserData->iSeat)
	{
		//�Ҳ����ӵ����ӵ�������ҷ�����
		return T_ZONE_INVALID_BULLETNUM;
	}

	//����ҵ��ӵ���
	if (pstUserData->iBulletNum > 0)
	{
		--pstUserData->iBulletNum;
	}

	iCostSilver = pBulletInfo->iCost;

	//��ȡ�����Ϣ
	FishData* pFishInfo = GetFishData(uFishUniqID);
	if (!pFishInfo)
	{
		//�Ҳ�����
		DeleteBulletData(uBulletUniqID);	//ɾ���ӵ�

											//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

		return 0;
	}

	iFishID = pFishInfo->iFishID;
	pFishInfo->aiCostSilver[pstUserData->iSeat] += iCostSilver;

	if (abs(lHitTime - lTimeNow) >= 1000)
	{
		//����ʱ��Ƿ�,ɾ���ӵ�
		DeleteBulletData(uBulletUniqID);
		
		//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

		return 0;
	}

	//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	const FishConfig* pstFishConfig = stBaseCfgMgr.GetFishConfig(pFishInfo->iFishSeqID, pFishInfo->iFishID);
	const GunConfig* pstGunConfig = stBaseCfgMgr.GetGunConfig(pBulletInfo->iGunID);
	if (!pstGunConfig || !pstFishConfig)
	{
		//�Ҳ�������
		DeleteBulletData(uBulletUniqID);

		//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);
		return 0;
	}

	//��ȡ������
	const ExpLineConfig* pstConfig = NULL;
	TEXPLINEINFO* pstExpLineInfo = GetRoleExpLineInfo(stRoleObj.GetUin(), pstConfig);
	if (!pstExpLineInfo)
	{
		LOGERROR("Failed to get exp line info, uin %u, algorithm type %d\n", stRoleObj.GetUin(), m_pstRoomConfig->iAlgorithmType);
		return T_ZONE_PARA_ERROR;
	}

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];
	if (pFishInfo->iType == FISH_TYPE_PHONETICKET)
	{
		//��ȯ��

		//��ȯ����ӵ������������״̬
		pstUserData->lUserSilverCost -= iCostSilver;	//������һ�������
		pstUserData->lUserTicketSilverCost += iCostSilver;//���ӵ�ȯ���������
		stData.lTotalRewardSilver -= iCostSilver;
		stData.lRewardSilver -= iCostSilver;
		stData.lTodayRewardSilver -= iCostSilver;

		//��ȯ���ӵ�������������
		pstExpLineInfo->lUserWinNum += iCostSilver;
		pstExpLineInfo->lIntervalWinNum += iCostSilver;

		//��ӡ��Ӫ��־ ��¼��ȯ����ˮ
		CZoneOssLog::TraceShootCostByFishType(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), m_pstRoomConfig->iRoomID,
			pBulletInfo->iCost, FISH_TYPE_PHONETICKET);

		//�����Ƿ�����
		if (rand() % 1000 < (pstGunConfig->iMultiple * 1000 / pstFishConfig->Multiple_i_max))
		{
			//��ȡ��ʼ��Ʊ��
			long8 lOldTicketNum = stRoleObj.GetResource(RESOURCE_TYPE_TICKET);

			//�����˵�ȯ��
			iRewardSilver = pstFishConfig->Multiple_i_min;
			if (!AddUserTicket(stRoleObj, uFishUniqID, false, pstGunConfig->iMultiple, iRewardSilver, true))
			{
				LOGERROR("Failed to add ticket fish user ticket, numid %u, ticket num %d\n", stRoleObj.GetUin(), iRewardSilver);
			}

			//���ӵ�ȯ�������Ʊ����
			stData.lTicketFishDropNum += iRewardSilver;

			//ɾ����
			DeleteFishData(uFishUniqID);

			//���е�ȯ��
			CGameEventManager::NotifyKillFish(stRoleObj, pstFishConfig->Sequence_i, pstFishConfig->Type_i, 1, RESOURCE_TYPE_TICKET, iRewardSilver);

			//��ӡ��Ӫ��־
			CZoneOssLog::TraceCatchFish(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), pBulletInfo->iCost, FISH_TYPE_PHONETICKET,
				RESOURCE_TYPE_TICKET, m_pstRoomConfig->iRoomID, lOldTicketNum, stRoleObj.GetResource(RESOURCE_TYPE_TICKET));
		}

		//ɾ���ӵ�
		DeleteBulletData(uBulletUniqID);

		//���ӵ�ȯ����ˮ
		stData.lTicketFishRewardSilver += iCostSilver;

		//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, iRewardSilver, false, iCostSilver, iFishIndex, 0);

		//���͵�ȯ�����
		const HorseLampConfig* pstHorseLampConfig = stBaseCfgMgr.GetHorseLampConfig(HORSELAMP_TYPE_TICKETS);
		if (pstHorseLampConfig && (iRewardSilver >= pstHorseLampConfig->aiNeeds[0]))
		{
			//���������
			SendFishHorseLamp(stRoleObj, HORSELAMP_TYPE_TICKETS, pstGunConfig->iMultiple, iRewardSilver, 0);
		}

		return 0;
	}
	else if (pFishInfo->iType == FISH_TYPE_WARHEAD)
	{
		//��ͷ��

		//��ͷ����ӵ������������״̬
		pstUserData->lUserSilverCost -= iCostSilver;	//������һ�������
		pstUserData->lUserTicketSilverCost += iCostSilver;	//���ӵ�ȯ���������
		stData.lTotalRewardSilver -= iCostSilver;
		stData.lRewardSilver -= iCostSilver;
		stData.lTodayRewardSilver -= iCostSilver;

		//��ͷ���ӵ�������������
		pstExpLineInfo->lUserWinNum += iCostSilver;
		pstExpLineInfo->lIntervalWinNum += iCostSilver;

		//��ӡ��Ӫ��־ ��¼��ͷ����ˮ
		CZoneOssLog::TraceShootCostByFishType(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), m_pstRoomConfig->iRoomID,
			pBulletInfo->iCost, FISH_TYPE_WARHEAD);

		//�����Ƿ�����,��ʽ P<1/����
		if ((rand() % 10000)*pFishInfo->iMultiple < 10000)
		{
			//�����˵�ͷ��,���ӵ�ͷ
			for (unsigned i = 0; i < sizeof(m_pstRoomConfig->astDrops) / sizeof(DropItemConfig); ++i)
			{
				const DropItemConfig& stDropConfig = m_pstRoomConfig->astDrops[i];
				if (stDropConfig.iItemID == 0)
				{
					break;
				}

				int iRet = CRepThingsUtility::AddItemNum(stRoleObj, stDropConfig.iItemID, stDropConfig.iItemNum, ITEM_CHANNEL_FISHADD);
				if (iRet)
				{
					LOGERROR("Failed to add fish drop item, ret %d, uin %u, item id %d, num %d\n", iRet, stRoleObj.GetUin(), stDropConfig.iItemID, stDropConfig.iItemNum);
					return iRet;
				}

				//������һ���
				const FishItemConfig* pstItemConfig = stBaseCfgMgr.GetFishItemConfig(stDropConfig.iItemID);
				if (pstItemConfig)
				{
					stRoleObj.AddFishScore(pstItemConfig->aiParam[0] * stDropConfig.iItemNum);
				}

				//��ȡ������ǰ������������ӡ��Ӫ��־ʹ��
				CRepThingsManager& rstThingsManager = stRoleObj.GetRepThingsManager();
				long8 lNewItemNum = rstThingsManager.GetRepItemNum(stDropConfig.iItemID);

				//��ӡ��Ӫ��־
				CZoneOssLog::TraceCatchWarHeadFish(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), pBulletInfo->iCost, FISH_TYPE_WARHEAD,
					FISH_ITEM_WARHEAD, stDropConfig.iItemID, m_pstRoomConfig->iRoomID, lNewItemNum - stDropConfig.iItemNum, lNewItemNum);

				//���͵�ͷ�����
				const HorseLampConfig* pstHorseLampConfig = stBaseCfgMgr.GetHorseLampConfig(HORSELAMP_TYPE_WARHEADS);
				if (pstHorseLampConfig &&
					(m_pstRoomConfig->iRoomID == pstHorseLampConfig->aiNeeds[0] || m_pstRoomConfig->iRoomID == pstHorseLampConfig->aiNeeds[1]))
				{
					//���������
					SendFishHorseLamp(stRoleObj, HORSELAMP_TYPE_WARHEADS, m_pstRoomConfig->iRoomID, stDropConfig.iItemNum, stDropConfig.iItemID);
				}
			}

			//�ָ�����
			m_bStopOutFish = false;

			//ɾ����
			DeleteFishData(uFishUniqID);

			//���е�ͷ��
			CGameEventManager::NotifyKillFish(stRoleObj, pstFishConfig->Sequence_i, pstFishConfig->Type_i, 1, 0, 0);

			//����Ϊ����
			iRewardSilver = 1;
		}

		//ɾ���ӵ�
		DeleteBulletData(uBulletUniqID);

		//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, iRewardSilver, false, iCostSilver, iFishIndex, 0);

		return 0;
	}
	else if (pFishInfo->iType == FISH_TYPE_SMALLGROUP)
	{
		//С����
		const SmallFishConfig* pstSmallConfig = stBaseCfgMgr.GetSmallFishConfig(pFishInfo->iFishID);
		if (!pstSmallConfig)
		{
			LOGERROR("Failed to get small fish config, fish id %d\n", pFishInfo->iFishID);
			return T_ZONE_INVALID_CFG;
		}

		//�����Ƿ���Ч
		if (iFishIndex < 0 || iFishIndex >= (int)pstSmallConfig->vTrackIDs.size())
		{
			//ɾ���ӵ�
			DeleteBulletData(uBulletUniqID);
			
			//�������������Ϣ
			SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

			return 0;
		}

		//�����Ƿ�����
		if (!(pFishInfo->cIndex&(0x01 << iFishIndex)))
		{
			//ɾ���ӵ�
			DeleteBulletData(uBulletUniqID);
			
			//�������������Ϣ
			SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

			return 0;
		}
	}
	else if (pFishInfo->iType == FISH_TYPE_MULTIPLE)
	{
		//���Ӷ౶��������ˮ
		stData.lMultiFishFlow += iCostSilver;
	}

	//����׼�ż��λ��
	if (pBulletInfo->uFishUniqID == 0)
	{
		//��֤�ӵ������λ��
		if (!CheckIsValidHit(lHitTime, *pBulletInfo, *pFishInfo, iFishIndex))
		{
			//��λ��δ������
			DeleteBulletData(uBulletUniqID);

			//�������������Ϣ
			SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

			return 0;
		}
	}

	//�������б���

	//��ӡ��Ӫ��־ ��ͨ�㷢����ˮ
	CZoneOssLog::TraceShootCostByFishType(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), m_pstRoomConfig->iRoomID,
		pBulletInfo->iCost, pFishInfo->iType);

	//������������и���
	if (!CheckIsLogicHit(stRoleObj, *pstUserData, *pstGunConfig, *pFishInfo, pstFishConfig->Adjust_i, false))
	{
		//�������߼�δ����
		DeleteBulletData(uBulletUniqID);

		//�������������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, 0, false, iCostSilver, iFishIndex, 0);

		return 0;
	}

	//���и���

	//ɾ���ӵ�
	DeleteBulletData(uBulletUniqID);

	int iFishCostSilver = pFishInfo->aiCostSilver[pstUserData->iSeat];
	pFishInfo->aiCostSilver[pstUserData->iSeat] = 0;

	if (pFishInfo->iType == FISH_TYPE_BOOMFISH || pFishInfo->iType == FISH_TYPE_FLASHFISH)
	{
		//���������ը�����߼�
		BoomFish(*pstUserData, pFishInfo, false);
	}

	//���㽱���ļƷ�
	int iFishMultiple = pFishInfo->iMultiple;
	iRewardSilver = iCostSilver * pFishInfo->iMultiple;

	//�౶�㽱������
	int iMultipleFish = 0;
	if (pFishInfo->iType == FISH_TYPE_MULTIPLE)
	{
		const MultipleFishConfig* pstMultipleFishConfig = stBaseCfgMgr.GetMultipleFishConfig();
		if (!pstMultipleFishConfig)
		{
			LOGERROR("Failed to get multiple fish config, uin %u\n", stRoleObj.GetUin());
			return T_ZONE_INVALID_CFG;
		}
		iRewardSilver = iRewardSilver * pstMultipleFishConfig->iRate;
		iMultipleFish = pstMultipleFishConfig->iRate;

		//�������ڶ౶��������ˮ
		stData.lMultiFishHitFlow += iRewardSilver;
	}

	if (pFishInfo->iType == FISH_TYPE_SMALLGROUP)
	{
		//С����
		pFishInfo->cIndex &= ~(0x01 << iFishIndex);
		if (pFishInfo->cIndex == 0)
		{
			//С���鱻����,ɾ����
			DeleteFishData(uFishUniqID);
		}
	}
	else
	{
		//ɾ����
		DeleteFishData(uFishUniqID);
	}

	//����������
	pstExpLineInfo->lUserWinNum += iRewardSilver;
	pstExpLineInfo->lIntervalWinNum += iRewardSilver;

	//������һ���
	stRoleObj.AddFishScore(iRewardSilver);

	//��ȡ��ҵ�ǰ���������������Ӫ��־ʹ��
	long8 lOldCoinNum = stRoleObj.GetResource(RESOURCE_TYPE_COIN);

	//���ӻ���
	if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, iRewardSilver))
	{
		LOGERROR("Failed to add user silver, uin %u, add silver %d\n", stRoleObj.GetUin(), iRewardSilver);
		return T_ZONE_INVALID_NOSILVER;
	}

	//�Ƿ�ʹ�ú��
	if (iRewardSilver > iFishCostSilver)
	{
		int iUseRedSilver = iRewardSilver - iFishCostSilver;
		if (pstUserData->uReturnID != 0 && pstUserData->lUnEffectTime >= lTimeNow && (stData.lTotalReturnSilver + stRoleObj.GetNowNewRedNum()) >= iRewardSilver)
		{
			if (stRoleObj.GetNowNewRedNum() >= iUseRedSilver)
			{
				//ֻʹ�����ֺ��
				stRoleObj.SetNowNewRedNum(stRoleObj.GetNowNewRedNum() - iUseRedSilver);
				stData.lTotalNewRedUseNum += iUseRedSilver;
				pstUserData->lCycleNewUsedNum += iUseRedSilver;
			}
			else
			{
				//�ȿ۳����ֺ��
				int iNewRedUsedNum = stRoleObj.GetNowNewRedNum();
				stRoleObj.SetNowNewRedNum(0);
				stData.lTotalNewRedUseNum += iNewRedUsedNum;
				pstUserData->lCycleNewUsedNum += iNewRedUsedNum;

				iUseRedSilver -= iNewRedUsedNum;

				//�ٿ۳���ͨ���
				stData.lTotalReturnSilver -= iUseRedSilver;
				stData.lUsedReturnSilver += iUseRedSilver;
			}
		}
		else if (stRoleObj.GetNowNewRedNum() >= iRewardSilver)
		{
			//���ֺ��
			stData.lUsedReturnSilver += iUseRedSilver;
			stRoleObj.SetNowNewRedNum(stRoleObj.GetNowNewRedNum() - iUseRedSilver);
			stData.lTotalNewRedUseNum += iUseRedSilver;
		}
	}

	pstUserData->iLossNum -= iRewardSilver;
	pstUserData->iCycleWinNum += iRewardSilver;
	pstUserData->lUserSilverReward += iRewardSilver;

	stData.lTotalCostSliver += iRewardSilver;
	stData.lCostSilver += iRewardSilver;
	stData.lTodayCostSliver += iRewardSilver;

	//��ͨ������ȯ
	if (!AddUserTicket(stRoleObj, uFishUniqID, false, pstGunConfig->iMultiple, iRewardSilver))
	{
		LOGERROR("Failed to add normal user ticket, numid %u, reward silver %d\n", stRoleObj.GetUin(), iRewardSilver);
		return T_ZONE_INVALID_CFG;
	}

	//�������������Ϣ
	SendHitFishInfoNotify(*pstUserData, uBulletUniqID, uFishUniqID, iRewardSilver, false, iCostSilver, iFishIndex, iMultipleFish);

	//���ͽ�������
	const HorseLampConfig* pstHorseLampConfig = stBaseCfgMgr.GetHorseLampConfig(HORSELAMP_TYPE_COINS);
	if (pstHorseLampConfig &&
		iRewardSilver >= pstHorseLampConfig->aiNeeds[0] &&
		iFishMultiple >= pstHorseLampConfig->aiNeeds[1])
	{
		//���������
		SendFishHorseLamp(stRoleObj, HORSELAMP_TYPE_COINS, pstGunConfig->iMultiple, iFishID, iRewardSilver);
	}

	//��ӡ��Ӫ��־
	CZoneOssLog::TraceCatchFish(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), pBulletInfo->iCost, pstFishConfig->Type_i,
		RESOURCE_TYPE_COIN, m_pstRoomConfig->iRoomID, lOldCoinNum, stRoleObj.GetResource(RESOURCE_TYPE_COIN));

	//������ͨ��
	CGameEventManager::NotifyKillFish(stRoleObj, pstFishConfig->Sequence_i, pstFishConfig->Type_i, 1, RESOURCE_TYPE_COIN, iRewardSilver);

	LOGDEBUG("HitFish, uin %u, reward %d, fish id %d, fish multiple %d, gun multiple %d\n", stRoleObj.GetUin(), iRewardSilver,
		iFishID, iFishMultiple, pstGunConfig->iMultiple);

	return 0;
}

//���������е���
int CFishpondObj::HitFormFish(CGameRoleObj& stRoleObj, long lHitTime, unsigned uBulletUniqID, int iFishOutID, int iFishIndex)
{
	int iFishID = 0;
	int iCostSilver = 0;
	int iRewardSilver = 0;

	//��ȡ�����Ϣ
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return T_ZONE_PARA_ERROR;
	}

	//�ͻ���FishIndex��1��ʼ
	if (iFishIndex < 1 || iFishIndex >(int)MAX_FORM_FISHKILL_NUM)
	{
		return T_ZONE_INVALID_BULLET;
	}

	iFishIndex = iFishIndex - 1;

	//��ȡ�ӵ���Ϣ
	BulletData* pBulletInfo = GetBulletData(uBulletUniqID);
	if (!pBulletInfo || pBulletInfo->iSeat != pstUserData->iSeat)
	{
		//�Ҳ����ӵ����ӵ�������ҷ�����
		return T_ZONE_INVALID_BULLET;
	}

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	const GunConfig* pstGunConfig = stBaseCfgMgr.GetGunConfig(stRoleObj.GetWeaponInfo().iWeaponID);
	if (!pstGunConfig)
	{
		return T_ZONE_INVALID_CFG;
	}

	iCostSilver = pstGunConfig->iMultiple;

	//����ҵ��ӵ���
	if (pstUserData->iBulletNum > 0)
	{
		--pstUserData->iBulletNum;
	}

	long lTimeNow = CTimeUtility::GetMSTime();
	if (abs(lHitTime - lTimeNow) >= 1000)
	{
		//����ʱ��Ƿ�		

		LOGERROR("HitFormFish, uin %u, hittime:servertime %ld:%ld\n", stRoleObj.GetUin(), lHitTime, lTimeNow);
		DeleteBulletData(uBulletUniqID);	//ɾ���ӵ�

											//����������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, 0, true, iCostSilver, 0, 0);

		return T_SERVER_SUCCESS;
	}

	//��ȡ�����Ϣ
	FishData stFishInfo;
	FormFishOutData* pstFormOutData = GetFormFishInfo(iFishOutID, iFishIndex, stFishInfo);
	if (!pstFormOutData)
	{
		//��ȡ�������ʧ��

		DeleteBulletData(uBulletUniqID);	//ɾ���ӵ�

											//����������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, 0, true, iCostSilver, 0, 0);

		return 0;
	}

	pstFormOutData->aiCostSilver[pstUserData->iSeat][iFishIndex] += pstGunConfig->iMultiple;
	iFishID = stFishInfo.iFishID;

	//������׼�ż��λ��
	if (pBulletInfo->uFishUniqID == 0)
	{
		//��֤�ӵ������λ��
		if (!CheckIsValidHit(lHitTime, *pBulletInfo, stFishInfo))
		{
			//��λ��δ������
			DeleteBulletData(uBulletUniqID);

			//����������Ϣ
			SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, 0, true, iCostSilver, 0, 0);

			return 0;
		}
	}

	//�������б���
	const FishConfig* pstFishConfig = stBaseCfgMgr.GetFishConfig(stFishInfo.iFishSeqID, stFishInfo.iFishID);
	if (!pstGunConfig || !pstFishConfig)
	{
		//�Ҳ�������
		DeleteBulletData(uBulletUniqID);

		//����������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, 0, true, iCostSilver, 0, 0);

		return 0;
	}

	//������������и���
	if (!CheckIsLogicHit(stRoleObj, *pstUserData, *pstGunConfig, stFishInfo, pstFishConfig->Adjust_i, true))
	{
		//�������߼�δ����
		DeleteBulletData(uBulletUniqID);

		//����������Ϣ
		SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, 0, true, iCostSilver, 0, 0);

		return 0;
	}

	//���и���

	//ɾ���ӵ�
	DeleteBulletData(uBulletUniqID);

	//������������Ϣ
	pstFormOutData->SetFishAlive(iFishIndex, false);

	if (stFishInfo.iType == FISH_TYPE_BOOMFISH || stFishInfo.iType == FISH_TYPE_FLASHFISH)
	{
		//���������ը�����߼�
		FishData* pstFishInfo = &stFishInfo;
		BoomFish(*pstUserData, pstFishInfo, true);
	}

	//���㽱���ļƷ�
	iRewardSilver = pstGunConfig->iMultiple * stFishInfo.iMultiple;

	//���ӻ���
	if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, iRewardSilver))
	{
		LOGERROR("Failed to add user silver, uin %u, add silver %d\n", stRoleObj.GetUin(), iRewardSilver);
		return T_ZONE_INVALID_NOSILVER;
	}

	int iFishCostSilver = pstFormOutData->aiCostSilver[pstUserData->iSeat][iFishIndex];

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	//ʹ�ú��
	if (pstUserData->uReturnID != 0 && pstUserData->lUnEffectTime >= lTimeNow)
	{
		if (stData.lTotalReturnSilver >= iRewardSilver && iRewardSilver > iFishCostSilver)
		{
			//��ǰʹ���˺������������������𣬸��˿��𣬸�������
			stData.lTotalReturnSilver -= (iRewardSilver - iFishCostSilver);
			stData.lUsedReturnSilver += (iRewardSilver - iFishCostSilver);

			//numid|nickname|seat|iFishID|uFishUniqID|uBulletUniqID|iRewardSilver|iFishCostSilver|lTotalReturnSilver
			LOGDEBUG("UseRedPacket, %u|%s|%d|%d|%u|%u|%d|%d|%lld\n", stRoleObj.GetUin(), stRoleObj.GetNickName(), pstUserData->iSeat, iFishID,
				iFishOutID * 1000 + iFishIndex, uBulletUniqID, iRewardSilver, iFishCostSilver, stData.lTotalReturnSilver);
		}
	}

	pstUserData->iLossNum -= iRewardSilver;
	pstUserData->iCycleWinNum += iRewardSilver;
	pstUserData->lUserSilverReward += iRewardSilver;

	stData.lTotalCostSliver += iRewardSilver;
	stData.lCostSilver += iRewardSilver;
	stData.lTodayCostSliver += iRewardSilver;

	//��ͨ������ȯ
	if (!AddUserTicket(stRoleObj, stFishInfo.uUniqueID, true, pstGunConfig->iMultiple, iRewardSilver))
	{
		LOGERROR("Failed to add normal user ticket, numid %u, reward silver %d\n", stRoleObj.GetUin(), iRewardSilver);
		return T_ZONE_INVALID_CFG;
	}

	//����������Ϣ
	SendHitFishInfoNotify(*pstUserData, uBulletUniqID, iFishOutID * 1000 + iFishIndex, iRewardSilver, true, iCostSilver, 0, 0);

	LOGDEBUG("HitFormFish, uin %u, reward %d\n", stRoleObj.GetUin(), iRewardSilver);

	return 0;
}

//���ʹ�ü���
int CFishpondObj::UseSkill(CGameRoleObj& stRoleObj, const Zone_UseSkill_Request& stReq)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		LOGERROR("Failed to get seat user, uin %u, table id %u\n", stRoleObj.GetUin(), m_uTableID);
		return T_ZONE_PARA_ERROR;
	}

	//ˢ����һ�Ծʱ��
	pstUserData->iActiveTime = CTimeUtility::GetNowTime();

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_USESKILL_NOTIFY);
	Zone_UseSkill_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_useskill_notify();
	pstNotify->set_itype(stReq.itype());
	pstNotify->set_uin(stRoleObj.GetUin());

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	switch (stReq.itype())
	{
	case SKILL_TYPE_AUTOSHOOT:
	{
		//ʹ���Զ�����
		int iTimeNow = CTimeUtility::GetNowTime();
		if (!pstUserData->bAutoShoot && stRoleObj.GetMonthEndTime() < iTimeNow)
		{
			//�¿�״̬�����㣬���ܼ����Զ�����
			LOGERROR("Failed to use autoshoot, uin %u, monthend:now %d:%d\n", stRoleObj.GetUin(), stRoleObj.GetMonthEndTime(), iTimeNow);
			return T_ZONE_PARA_ERROR;
		}

		//��ӡ��Ӫ��־�� �Զ����ڲ����ĵ���
		CZoneOssLog::TraceUseSkill(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), SKILL_TYPE_AUTOSHOOT, m_pstRoomConfig->iRoomID, 0, 0);

		//�л�״̬
		pstUserData->bAutoShoot = ~pstUserData->bAutoShoot;
		pstNotify->set_bisopen(pstUserData->bAutoShoot);
	}
	break;

	case SKILL_TYPE_AIMFISH:
	{
		//��׼��
		long lTimeNow = CTimeUtility::GetMSTime();

		//�Ƿ�����׼��
		if (pstUserData->lAimEndTime > lTimeNow)
		{
			//����ʹ����׼
			LOGERROR("Failed to use aim item, in aim fish, uin %u\n", stRoleObj.GetUin());
			return T_ZONE_PARA_ERROR;
		}

		//��ȡ����
		const FishItemConfig* pstItemConfig = stBaseCfgMgr.GetFishItemConfig(AIM_FISH_ITEM_ID);
		if (!pstItemConfig)
		{
			LOGERROR("Failed to get fish item config, invalid id %d\n", AIM_FISH_ITEM_ID);
			return T_ZONE_INVALID_CFG;
		}

		//��ȡ��ҵ�ǰ��׼�������� ������Ӫ��־ʹ��
		long8 lOldItemNum = stRoleObj.GetRepThingsManager().GetRepItemNum(AIM_FISH_ITEM_ID);

		//�ȿ۳�����
		int iRet = CRepThingsUtility::AddItemNum(stRoleObj, AIM_FISH_ITEM_ID, -1, ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			//���ܿ۵��ߣ����Կ�Ǯ
			if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, -pstItemConfig->aiParam[0]))
			{
				//���ܿ۳�����
				LOGERROR("Failed to use aim item, no enough cost, uin %u\n", stRoleObj.GetUin());
				return T_ZONE_INVALID_NOSILVER;
			}
		}

		//��Ч��׼
		pstUserData->lAimEndTime = lTimeNow + pstItemConfig->aiParam[1] * 1000;

		//��ӡ��Ӫ��־ ʹ����׼����
		CZoneOssLog::TraceUseSkill(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), SKILL_TYPE_AIMFISH, m_pstRoomConfig->iRoomID, lOldItemNum, stRoleObj.GetRepThingsManager().GetRepItemNum(AIM_FISH_ITEM_ID));

		pstNotify->set_laimendtime(pstUserData->lAimEndTime);
	}
	break;

	case SKILL_TYPE_WARHEAD:
	{
		//ʹ�õ�ͷ

		//��ȡ����
		const FishItemConfig* pstItemConfig = stBaseCfgMgr.GetFishItemConfig(stReq.iitemid());
		if (!pstItemConfig || pstItemConfig->iType != FISH_ITEM_WARHEAD)
		{
			LOGERROR("Failed to get warhead item config, invalid id %d\n", stReq.iitemid());
			return T_ZONE_PARA_ERROR;
		}

		//�Ȼ�ȡʹ��ǰ��ͷ��������Ӫ��־ʹ��
		long8 lOldWarHeadNum = stRoleObj.GetRepThingsManager().GetRepItemNum(stReq.iitemid());
		long8 lOldCoinNum = stRoleObj.GetResource(RESOURCE_TYPE_COIN);

		//�ȿ۳�����
		int iRet = CRepThingsUtility::AddItemNum(stRoleObj, stReq.iitemid(), -1, ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			LOGERROR("Failed to use warhead, ret %d, uin %u, item id %d\n", iRet, stRoleObj.GetUin(), stReq.iitemid());
			return iRet;
		}

		//ɾ��ը������
		for (int i = 0; i < stReq.ufishuniqids_size(); ++i)
		{
			DeleteFishData(stReq.ufishuniqids(i));
		}

		//��ը�㽱��
		if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_COIN, pstItemConfig->aiParam[0]))
		{
			LOGERROR("Failed to add warhead reward, uin %u, item id %d\n", stRoleObj.GetUin(), stReq.iitemid());
			return T_ZONE_PARA_ERROR;
		}

		//��ӡ��Ӫ��־ ��ͷʹ����־
		CZoneOssLog::TraceUseWarHead(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), stReq.iitemid(), pstItemConfig->aiParam[0],
			m_pstRoomConfig->iRoomID, lOldWarHeadNum, stRoleObj.GetRepThingsManager().GetRepItemNum(stReq.iitemid()),
			lOldCoinNum, stRoleObj.GetResource(RESOURCE_TYPE_COIN));

		//����������Ϣ����
		pstNotify->mutable_stwarheadpos()->CopyFrom(stReq.stwarheadpos());
		pstNotify->mutable_ufishuniqids()->CopyFrom(stReq.ufishuniqids());
		pstNotify->set_lrewardcoins(pstItemConfig->aiParam[0]);
	}
	break;

	case SKILL_TYPE_WILD:
	{
		//��

		//���ж��Ƿ���ʹ��
		const VipLevelConfig* pstVipConfig = stBaseCfgMgr.GetVipConfig(stRoleObj.GetVIPLevel());
		if (!pstVipConfig)
		{
			LOGERROR("Failed to get vip config, invalid vip level %d, uin %u\n", stRoleObj.GetVIPLevel(), stRoleObj.GetUin());
			return T_ZONE_INVALID_CFG;
		}

		const VipPriv* pstPriv = NULL;
		for (unsigned i = 0; i < pstVipConfig->vPrivs.size(); ++i)
		{
			if (pstVipConfig->vPrivs[i].iPrivType == VIP_PRIV_WILDNUM)
			{
				pstPriv = &pstVipConfig->vPrivs[i];
				break;
			}
		}

		//�Ƿ������п���
		if (!pstPriv || pstPriv->aiParams[0] <= 0)
		{
			LOGERROR("Failed to use wild skill, no valid config, uin %u, vip level %d\n", stRoleObj.GetUin(), stRoleObj.GetVIPLevel());
			return T_ZONE_PARA_ERROR;
		}

		//�Ƿ�ﵽ�񱩴�������
		if (pstUserData->iWildNum >= pstPriv->aiParams[2])
		{
			LOGERROR("Failed to use wild skill, already reach max num, uin %u\n", stRoleObj.GetUin());
			return T_ZONE_PARA_ERROR;
		}

		pstUserData->lWildEndTime = CTimeUtility::GetMSTime() + pstPriv->aiParams[0] * 1000;
		pstUserData->iWildNum += 1;

		//ȡ����Һ��״̬
		pstUserData->lUnEffectTime = 0;

		//��ӡ��Ӫ��־ ����ʹ�� �� �����ĵ���
		CZoneOssLog::TraceUseSkill(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), SKILL_TYPE_WILD, m_pstRoomConfig->iRoomID, 0, 0);

		//����������Ϣ�Ĳ���
		pstNotify->set_lwildendtime(pstUserData->lWildEndTime);
		pstNotify->set_iwildnum(pstUserData->iWildNum);
	}
	break;

	case SKILL_TYPE_PREWARHEAD:
	{
		//Ԥʹ�õ�ͷ

		//��ȡ����
		const FishItemConfig* pstItemConfig = stBaseCfgMgr.GetFishItemConfig(stReq.iitemid());
		if (!pstItemConfig || pstItemConfig->iType != FISH_ITEM_WARHEAD)
		{
			LOGERROR("Failed to get warhead item config, invalid id %d\n", stReq.iitemid());
			return T_ZONE_PARA_ERROR;
		}

		//����������Ϣ����
		pstNotify->mutable_stwarheadpos()->CopyFrom(stReq.stwarheadpos());
	}
	break;

	default:
	{
		LOGERROR("Failed to use skill, uin %u, skill type %d\n", stRoleObj.GetUin(), stReq.itype());
		return T_ZONE_INVALID_SKILLTYPE;
	}
	break;
	}

	//���ͼ���ʹ��֪ͨ
	SendZoneMsgToFishpond(ms_stZoneMsg);

	//����ʹ�õ�֪ͨ
	if (stReq.itype() != SKILL_TYPE_AUTOSHOOT && stReq.itype() != SKILL_TYPE_PREWARHEAD)
	{
		//�Զ����ں�Ԥʹ�õ�ͷ������ʹ�ô���
		CGameEventManager::NotifyUseSkill(stRoleObj, stReq.itype(), 1);
	}

	return T_SERVER_SUCCESS;
}

//���ѡ����׼��
int CFishpondObj::ChooseAimFish(CGameRoleObj& stRoleObj, unsigned uFishUniqID, int iFishIndex)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		LOGERROR("Failed to get fish user, table id %d, uin %u\n", m_uTableID, stRoleObj.GetUin());
		return T_ZONE_GAMEROLE_NOT_EXIST;
	}

	long lTimeNow = CTimeUtility::GetMSTime();

	//������һ�Ծʱ��
	pstUserData->iActiveTime = lTimeNow / 1000;

	//�Ƿ�����׼״̬
	if (pstUserData->lAimEndTime + 100 < lTimeNow)
	{
		//������׼״̬
		pstUserData->uAimFishUniqID = 0;

		return T_ZONE_INVALID_AIMSTAT;
	}

	//���Ƿ����
	FishData* pstFishData = GetFishData(uFishUniqID);
	if (!pstFishData || pstFishData->iType == FISH_TYPE_PHONETICKET)
	{
		//�㲻���ڻ��ߵ�ȯ��
		pstUserData->uAimFishUniqID = 0;

		return T_ZONE_INVALID_FISH;
	}

	//�ж�iFishIndex
	if (iFishIndex < 0)
	{
		//�Ƿ���index
		pstUserData->uAimFishUniqID = 0;
		return T_ZONE_PARA_ERROR;
	}
	else if (iFishIndex == 0)
	{
		if (pstFishData->iType == FISH_TYPE_SMALLGROUP)
		{
			//�Ƿ���index
			pstUserData->uAimFishUniqID = 0;
			return T_ZONE_PARA_ERROR;
		}
	}
	else
	{
		if (pstFishData->iType != FISH_TYPE_SMALLGROUP || !(pstFishData->cIndex&(0x01 << (iFishIndex - 1))))
		{
			//�Ƿ���index
			pstUserData->uAimFishUniqID = 0;
			return T_ZONE_PARA_ERROR;
		}
	}

	//������׼����Ϣ
	pstUserData->uAimFishUniqID = uFishUniqID;
	pstUserData->iAimFishIndex = iFishIndex;

	//��������ӵ�����׼��Ϣ
	for (unsigned i = 0; i < m_vBulletData.size(); ++i)
	{
		if (m_vBulletData[i].iSeat != pstUserData->iSeat || m_vBulletData[i].uFishUniqID == 0)
		{
			continue;
		}

		m_vBulletData[i].uFishUniqID = uFishUniqID;
		m_vBulletData[i].iFishIndex = iFishIndex;
	}

	//��װ����
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_CHOOSEAIMFISH_NOTIFY);
	Zone_ChooseAimFish_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_chooseaimfish_notify();
	pstNotify->set_uin(stRoleObj.GetUin());
	pstNotify->set_ufishuniqid(uFishUniqID);
	pstNotify->set_ifishindex(iFishIndex);

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return T_SERVER_SUCCESS;
}

//��ȡ����ID
unsigned CFishpondObj::GetTableID()
{
	return m_uTableID;
}

//��ȡ���ӷ���ID
int CFishpondObj::GetFishRoomID()
{
	return m_iFishRoomID;
}

//��ȡ�������
int CFishpondObj::GetPlayerNum()
{
	return m_vSeatUserData.size();
}

//��ȡ�������
const FishRoomConfig* CFishpondObj::GetRoomConfig()
{
	return m_pstRoomConfig;
}

//��ʱ��
int CFishpondObj::OnTick(CGameRoleObj& stRoleObj)
{
	long lTimeNow = CTimeUtility::GetMSTime();

	//������Ҹ�����Ϣ
	UpdateSeatUserInfo(stRoleObj, lTimeNow);

	if (lTimeNow - m_lLastTickTime < 1000)
	{
		//1s��ִ��һ����ص�Tick
		return 0;
	}

	m_lLastTickTime = lTimeNow;

	//���³�����Ϣ
	int iRet = UpdateOutFishRule(lTimeNow);
	if (iRet)
	{
		return iRet;
	}

	//���������Ϣ
	iRet = UpdateFishInfo(lTimeNow);
	if (iRet)
	{
		return iRet;
	}

	//����������Ϣ
	iRet = UpdateFishFormInfo(lTimeNow);
	if (iRet)
	{
		return iRet;
	}

	//���²��������Ϣ
	iRet = UpdateFishUserInfo(lTimeNow);
	if (iRet)
	{
		return iRet;
	}

	return 0;
}

//���Ͳ��������Ϣ����
void CFishpondObj::SendFishUserUpdate(unsigned uiUin, int iResType, long8 lAddNum, int iItemID, int iItemNum)
{
	static GameProtocolMsg stMsg;

	SeatUserData* pstUserData = GetSeatUserByUin(uiUin);

	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_FISHUSERUPDATE_NOTIFY);
	Zone_FishUserUpdate_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_fishuserupdate_notify();
	pstNotify->set_iseat(pstUserData->iSeat);

	if (iResType == RESOURCE_TYPE_COIN)
	{
		pstNotify->set_iaddcoins(lAddNum);
	}
	else if (iResType == RESOURCE_TYPE_TICKET)
	{
		pstNotify->set_iaddtickets(lAddNum);
	}

	if (iItemID != 0)
	{
		pstNotify->mutable_stadditem()->set_iitemid(iItemID);
		pstNotify->mutable_stadditem()->set_iitemnum(iItemNum);
	}

	SendZoneMsgToFishpond(stMsg);

	return;
}

//����Fishpond
void CFishpondObj::ResetFishpond()
{
	//���ID
	m_uTableID = 0;

	//������ڷ���ID
	m_iFishRoomID = 0;

	//�������
	m_pstRoomConfig = NULL;

	//��λ��Ϣ
	m_vSeatUserData.clear();

	//��س������
	m_vOutFishRule.clear();

	//����е��ӵ�
	m_vBulletData.clear();

	//������Ϣ
	m_stFishFormData.Reset();

	//����������Ϣ
	m_vFishData.clear();

	//��س���������Ϣ
	m_mFishLimitData.clear();

	//�Ƿ�ֹͣ����
	m_bStopOutFish = false;

	//���͸��ͻ��˵���Ϣ
	ms_stZoneMsg.Clear();

	//����ϴ�tickʱ��,��λms
	m_lLastTickTime = 0;

	return;
}

//��ʼ������Ϣ
int CFishpondObj::InitFishInfo()
{
	//��ʼ��������Ϣ
	int iRet = InitOutFishRule();
	if (iRet)
	{
		LOGERROR("Failed to init out fish rule, ret %d\n", iRet);
		return iRet;
	}

	//��ʼ��������Ϣ
	iRet = InitFishFormRule(false);
	if (iRet)
	{
		LOGERROR("Failed to init fish form info, ret %d\n", iRet);
		return iRet;
	}

	return T_SERVER_SUCCESS;
}

bool CompareFishPower(const OutFishRule& stRule1, const OutFishRule& stRule2)
{
	return (stRule1.iPower > stRule2.iPower);
}

//��ʼ��������Ϣ
int CFishpondObj::InitOutFishRule()
{
	//��ȡ���ù�����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	OutFishRule stOneRule;
	const FishSeqType& stFishSeqConfigs = stBaseCfgMgr.GetAllFishConfig();

	//��������
	FishLimitData stLimit;
	for (FishSeqType::const_iterator it = stFishSeqConfigs.begin(); it != stFishSeqConfigs.end(); ++it)
	{
		UpdateOneOutFishRule(it->first, stOneRule, -10);

		if (stOneRule.iFishSeqID != 0)
		{
			m_vOutFishRule.push_back(stOneRule);
		}

		//��ʼ����������
		if (it->second.size() != 0)
		{
			stLimit.iLimitMaxNum = it->second[0].iLimitNum;
			stLimit.iRemainNum = it->second[0].iLimitNum;
			m_mFishLimitData[it->second[0].iLimitType] = stLimit;
		}
	}

	//������Ȩ������
	std::sort(m_vOutFishRule.begin(), m_vOutFishRule.end(), CompareFishPower);

	return 0;
}

//��ʼ��������Ϣ
int CFishpondObj::InitFishFormRule(bool bIsFormEnd)
{
	//��ǰû������
	return 0;

	if (!bIsFormEnd)
	{
		//�������������ճ�����Ϣ
		m_stFishFormData.vFishOutData.clear();

		//�������������ձ�����Ϣ
		m_stFishFormData.vFormFreezeInfo.clear();
	}

	m_stFishFormData.lNextUpdateTime = 0;	//�´γ��������ʱ��
	m_stFishFormData.iFormTypeID = 0;		//���������ID
	m_stFishFormData.lFormEndTime = 0;		//������ʧ��ʱ��
	m_stFishFormData.bIsCleared = false;	//�Ƿ��Ѿ��峡
	m_stFishFormData.bIsInForm = false;		//�Ƿ���������

											//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	int iRandTimeMin = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_OUTFORMMIN);
	int iRandTimeMax = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_OUTFORMMAX);
	if (iRandTimeMax == iRandTimeMin)
	{
		return -1;
	}

	m_stFishFormData.lNextUpdateTime = CTimeUtility::GetMSTime() + (rand() % (iRandTimeMax - iRandTimeMin) + iRandTimeMin) * 1000;

	//�����������ʱ����ͻ���
	SendFishFormTime(NULL, true);

	return 0;
}

//���µ���������Ϣ
void CFishpondObj::UpdateOneOutFishRule(int iFishSeqID, OutFishRule& stFishRule, int iAddTime)
{
	long lTimeNow = CTimeUtility::GetMSTime();

	stFishRule.Reset();

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//��������
	FishSeqType::const_iterator it = stBaseCfgMgr.GetAllFishConfig().find(iFishSeqID);
	if (it == stBaseCfgMgr.GetAllFishConfig().end())
	{
		//û�ҵ�����
		stFishRule.Reset();
		return;
	}

	const std::vector<FishConfig>& stSeqConfig = it->second;

	//����ID
	stFishRule.iFishSeqID = iFishSeqID;

	//û����Ч������
	if (stSeqConfig.size() == 0 || stSeqConfig[0].Time_max_i == stSeqConfig[0].Time_min_i)
	{
		stFishRule.Reset();
		return;
	}

	//�ж��Ƿ��Ӧ����
	if (!IsRoomType(stSeqConfig[0].iRoomTypeID))
	{
		//���Ƕ�Ӧ�������
		stFishRule.Reset();
		return;
	}

	//����ֵ�ʱ��
	int iTimeInterval = 1000 * (rand() % (stSeqConfig[0].Time_max_i - stSeqConfig[0].Time_min_i) + stSeqConfig[0].Time_min_i);

	//����������ʱ��ϵ����Ӱ��
	iTimeInterval = iTimeInterval*stSeqConfig[0].aiTimeParam[GetPlayerNum()] / 1000;

	if (iTimeInterval == 0)
	{
		//������
		stFishRule.Reset();
		return;
	}
	
	stFishRule.lOutTime = lTimeNow + iTimeInterval + iAddTime * 1000;
	stFishRule.iDeltaTime = iAddTime * 1000;

	stFishRule.iPower = stSeqConfig[0].iPower;
	stFishRule.iLimitType = stSeqConfig[0].iLimitType;
	stFishRule.iType = stSeqConfig[0].Type_i;

	//����һ�������е���
	int iRandMax = 0;
	for (unsigned i = 0; i<stSeqConfig.size(); ++i)
	{
		iRandMax += stSeqConfig[i].Occurrence_i;
	}

	int iRandNum = rand() % iRandMax;

	for (unsigned i = 0; i<stSeqConfig.size(); ++i)
	{
		if (iRandNum < stSeqConfig[i].Occurrence_i)
		{
			//���ɵ���
			stFishRule.iFishID = stSeqConfig[i].id;
			break;
		}

		iRandNum -= stSeqConfig[i].Occurrence_i;
	}

	return;
}

//���������Ϣ
void CFishpondObj::UpdateSeatUserInfo(CGameRoleObj& stRoleObj, long lTimeNow)
{
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return;
	}

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	//������ҷ�����ˮ��Ϣ
	if (pstUserData->uCycleID != stData.uCycleID)
	{
		//���������Ѿ�����
		pstUserData->uCycleID = stData.uCycleID;
		pstUserData->lUserFlow[CYCLE_TIME_LAST] = pstUserData->lUserFlow[CYCLE_TIME_NOW];
		pstUserData->lUserFlow[CYCLE_TIME_NOW] = 0;

		//������ڽ�����־
		CZoneOssLog::TraceCycleProfit(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), -pstUserData->iCycleWinNum, pstUserData->lCycleNewUsedNum,
			stRoleObj.GetResource(RESOURCE_TYPE_COIN), stRoleObj.GetResource(RESOURCE_TYPE_TICKET));

		pstUserData->iCycleWinNum = 0;
		pstUserData->lCycleNewUsedNum = 0;
	}

	//��������Ϣ
	if (pstUserData->iEffectCountdown > 0)
	{
		//��δ��Ч�ĺ��
		pstUserData->iEffectCountdown -= 1000;
		if (pstUserData->iEffectCountdown <= 0)
		{
			BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

			//��������ЧЧ��
			int iEffectTime = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_TIMEDURATION);
			pstUserData->lUnEffectTime = lTimeNow + iEffectTime * 1000;
		}
	}

	return;
}

//���³�����Ϣ
int CFishpondObj::UpdateOutFishRule(long lTimeNow)
{
	if (m_bStopOutFish)
	{
		//ֹͣ����
		return 0;
	}

	//���¼����������
	for (std::map<int, FishLimitData>::iterator it = m_mFishLimitData.begin(); it != m_mFishLimitData.end(); ++it)
	{
		it->second.iRemainNum = it->second.iLimitMaxNum;
	}

	for (unsigned i = 0; i < m_vFishData.size(); ++i)
	{
		--m_mFishLimitData[m_vFishData[i].iLimitType].iRemainNum;
	}

	//�³�������Ϣ
	std::vector<FishData> vNewFishInfo;

	//�������������Ϣ
	int iFishSeqID = 0;
	int iFishID = 0;
	long lOutTime = 0;
	int iRet = 0;
	for (unsigned i = 0; i<m_vOutFishRule.size(); ++i)
	{
		//�Ƿ��������
		if (m_vFishData.size() >= MAX_FISH_NUM)
		{
			break;
		}

		//�Ƿ񳬹���������
		if (m_mFishLimitData[m_vOutFishRule[i].iLimitType].iRemainNum <= 0)
		{
			continue;
		}

		if (m_vOutFishRule[i].lOutTime > lTimeNow)
		{
			//��δ������ʱ��
			continue;
		}

		iFishSeqID = m_vOutFishRule[i].iFishSeqID;
		iFishID = m_vOutFishRule[i].iFishID;
		lOutTime = lTimeNow + m_vOutFishRule[i].iDeltaTime;	//Ҫ�õ�ǰʱ��-�����ֵʱ��

															//���»�ȡ��һ����
		UpdateOneOutFishRule(m_vOutFishRule[i].iFishSeqID, m_vOutFishRule[i], 0);

		//����м���
		iRet = AddNewFish(iFishSeqID, iFishID, lOutTime);
		if (iRet)
		{
			LOGERROR("Failed to add new fish, ret %d, fish seq id %d, fish id %d, out time %ld, now %ld\n", iRet, iFishSeqID, iFishID,
				lOutTime, lTimeNow);
			continue;
		}

		//���ӵ��³����б�
		vNewFishInfo.push_back(*m_vFishData.rbegin());

		//�޸ĳ�������
		--m_mFishLimitData[m_vOutFishRule[i].iLimitType].iRemainNum;
	}

	if (vNewFishInfo.size() > 0)
	{
		//�����³�����Ϣ
		SendFishInfoToUser(NULL, vNewFishInfo);
	}

	return 0;
}

//��������������Ϣ
int CFishpondObj::UpdateFishInfo(long lTimeNow)
{
	//����������������
	int iToBeDeleteNum = 0;
	for (unsigned i = 0; i<(m_vFishData.size() - iToBeDeleteNum);)
	{
		if (m_vFishData[i].lDeadTime > lTimeNow)
		{
			//δ����
			++i;
			continue;
		}

		if (m_vFishData[i].iType == FISH_TYPE_WARHEAD)
		{
			//�������ǵ�ͷ��,�ָ�����
			m_bStopOutFish = false;
		}

		//��λ�õ����Ѿ�����
		m_vFishData[i] = m_vFishData[(m_vFishData.size() - 1) - iToBeDeleteNum];

		++iToBeDeleteNum;
	}

	m_vFishData.resize(m_vFishData.size() - iToBeDeleteNum);

	return 0;
}

//����������Ϣ
int CFishpondObj::UpdateFishFormInfo(long lTimeNow)
{
	//��ǰû������
	return 0;

	//��ȡ���ù�����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	if (!m_stFishFormData.bIsCleared && m_stFishFormData.lNextUpdateTime <= (lTimeNow + 5 * 1000))
	{
		m_stFishFormData.bIsCleared = true;

		//����ʼǰ5s������
		LOGDEBUG("No fish before fishform, time %ld, form time %ld\n", lTimeNow, m_stFishFormData.lNextUpdateTime);

		//���һ������
		const FormTimeConfig* pstFormConfig = stBaseCfgMgr.GetFormTimeConfigByWeight();
		if (!pstFormConfig)
		{
			LOGERROR("Failed to get formtime config!\n");
			return -1;
		}

		m_stFishFormData.iFormTypeID = pstFormConfig->ID;
		m_stFishFormData.lFormEndTime = lTimeNow + (pstFormConfig->iPauseTime * 1000 + 5 * 1000);

		//����������ͣ
		for (unsigned i = 0; i<m_vOutFishRule.size(); ++i)
		{
			m_vOutFishRule[i].lOutTime += (pstFormConfig->iPauseTime * 1000 + 5 * 1000);
		}
	}

	if (!m_stFishFormData.bIsInForm && m_stFishFormData.lNextUpdateTime <= lTimeNow)
	{
		//����ʼ
		m_stFishFormData.bIsInForm = true;

		//������
		m_vFishData.clear();

		//�������������Ϣ
		const std::vector<TrajectoryConfig>* pstTrajectoryConfig = stBaseCfgMgr.GetTrajectoryConfigByType(m_stFishFormData.iFormTypeID);
		if (!pstTrajectoryConfig)
		{
			LOGERROR("Failed to get trajectory config, form type id %d\n", m_stFishFormData.iFormTypeID);
			return -2;
		}

		m_stFishFormData.vFishOutData.clear();
		m_stFishFormData.vFormFreezeInfo.clear();

		FormFishOutData stOutData;
		for (unsigned i = 0; i<(*pstTrajectoryConfig).size(); ++i)
		{
			stOutData.iFishID = (*pstTrajectoryConfig)[i].iFishID;

			const FishConfig* pstFishConfig = stBaseCfgMgr.GetFishConfig(stOutData.iFishID);
			if (!pstFishConfig)
			{
				LOGERROR("Failed to get fish config, fish id %d\n", stOutData.iFishID);
				return -3;
			}

			stOutData.iOutID = (*pstTrajectoryConfig)[i].ID;
			stOutData.iFishSeqID = pstFishConfig->Sequence_i;
			stOutData.iTraceID = (*pstTrajectoryConfig)[i].iTraceID;
			stOutData.lBeginTime = lTimeNow + (*pstTrajectoryConfig)[i].iStartTime;
			stOutData.iRemainNum = (*pstTrajectoryConfig)[i].iFishNumMax;
			stOutData.iInterval = (*pstTrajectoryConfig)[i].iInterval;
			stOutData.iType = pstFishConfig->Type_i;

			memset(stOutData.szAliveFish, 0, sizeof(stOutData.szAliveFish));
			for (int j = 0; j < stOutData.iRemainNum / 64; ++j)
			{
				stOutData.szAliveFish[j] = ~0x0;
			}
			stOutData.szAliveFish[stOutData.iRemainNum / 64] = ((ulong8)0x01 << (stOutData.iRemainNum % 64)) - 1;

			//��ı���
			if (pstFishConfig->Multiple_i_min == pstFishConfig->Multiple_i_max)
			{
				stOutData.iMultiple = pstFishConfig->Multiple_i_min;
			}
			else
			{
				stOutData.iMultiple = pstFishConfig->Multiple_i_min + rand() % (pstFishConfig->Multiple_i_max - pstFishConfig->Multiple_i_min);
			}

			m_stFishFormData.vFishOutData.push_back(stOutData);
		}

		//�����������ʱ��
		SendFishFormTime(NULL, false);

		//����������ϸ��Ϣ
		SendFishFormInfo(NULL);
	}

	if (m_stFishFormData.bIsInForm && m_stFishFormData.lFormEndTime > lTimeNow)
	{
		//���������
	}
	else if (m_stFishFormData.bIsInForm && m_stFishFormData.lFormEndTime <= lTimeNow)
	{
		//�������
		InitFishFormRule(true);

		//������
		//m_vFishData.clear();
	}

	return 0;
}

//���������Ϣ
int CFishpondObj::UpdateFishUserInfo(long lTimeNow)
{
	int iTimeNow = lTimeNow / 1000;
	for (unsigned i = 0; i < m_vSeatUserData.size(); ++i)
	{
		CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(m_vSeatUserData[i].uiUin);
		if (!pstRoleObj)
		{
			continue;
		}

		//todo jasonxiong ��ʱȥ��������Ҫ�ָ�����
		/*
		if ((lTimeNow / 1000 - m_vSeatUserData[i].iActiveTime) >= 5 * 60)
		{
			//�Ѿ�����5���Ӳ���Ծ��ǿ���ߵ����
			ExitFishpond(*pstRoleObj, true);
		}
		*/

		if (!m_vSeatUserData[i].bAutoShoot)
		{
			//δ���Զ�����
			continue;
		}

		if (pstRoleObj->GetMonthEndTime() < iTimeNow)
		{
			//�¿��ѹ���.ȡ���Զ�����
			m_vSeatUserData[i].bAutoShoot = false;
		}
	}

	return T_SERVER_SUCCESS;
}

//����м���
int CFishpondObj::AddNewFish(int iFishSeqID, int iFishID, long lOutTime, int iTraceID)
{
	//�Ƿ��������
	if (m_vFishData.size() >= MAX_FISH_NUM)
	{
		return 0;
	}

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const FishConfig* pstFishConfig = stBaseCfgMgr.GetFishConfig(iFishSeqID, iFishID);
	if (!pstFishConfig)
	{
		//�Ҳ����������
		return T_ZONE_INVALID_CFG;
	}

	//����
	FishData stFishInfo;
	stFishInfo.uUniqueID = CFishUtility::GetFishUniqID();
	stFishInfo.iFishID = iFishID;
	stFishInfo.lBornTime = lOutTime;
	stFishInfo.iFishSeqID = iFishSeqID;
	stFishInfo.iType = pstFishConfig->Type_i;
	stFishInfo.iLimitType = pstFishConfig->iLimitType;

	if (stFishInfo.iType == FISH_TYPE_SMALLGROUP)
	{
		//С����
		const SmallFishConfig* pstSmallConfig = stBaseCfgMgr.GetSmallFishConfig(stFishInfo.iFishID);
		if (!pstSmallConfig)
		{
			LOGERROR("Failed to get small fish config, fish id %d\n", stFishInfo.iFishID);
			return T_ZONE_INVALID_CFG;
		}

		stFishInfo.cIndex = (0x01 << pstSmallConfig->vTrackIDs.size()) - 1;
		
		//���ù켣Ϊ��һ����Ĺ켣
		if (pstSmallConfig->vTrackIDs.size() > 0)
		{
			iTraceID = pstSmallConfig->vTrackIDs[0];
		}
	}
	else if (stFishInfo.iType == FISH_TYPE_WARHEAD)
	{
		//�ǵ�ͷ��,ֹͣ����
		m_bStopOutFish = true;
	}

	if (pstFishConfig->Multiple_i_min == pstFishConfig->Multiple_i_max)
	{
		stFishInfo.iMultiple = pstFishConfig->Multiple_i_min;
	}
	else
	{
		stFishInfo.iMultiple = pstFishConfig->Multiple_i_min + rand() % (pstFishConfig->Multiple_i_max - pstFishConfig->Multiple_i_min);
	}

	if (iTraceID == 0)
	{
		//����켣
		std::vector<int> vAllTraceIDs;
		std::vector<int> vOneTypeTraceIDs;
		for (unsigned j = 0; j < pstFishConfig->vTraceType_i.size(); ++j)
		{
			stBaseCfgMgr.GetTraceIDsByType(pstFishConfig->vTraceType_i[j], vOneTypeTraceIDs);
			vAllTraceIDs.insert(vAllTraceIDs.end(), vOneTypeTraceIDs.begin(), vOneTypeTraceIDs.end());
		}

		if (vAllTraceIDs.size() == 0)
		{
			//�Ҳ����Ϸ��Ĺ켣
			return -2;
		}

		//�����Ĺ켣
		iTraceID = vAllTraceIDs[rand() % vAllTraceIDs.size()];
	}

	stFishInfo.iTraceID = iTraceID;

	//��ȡ�켣����
	const TraceConfig* pstTraceConfig = stBaseCfgMgr.GetTraceConfigByID(stFishInfo.iTraceID);
	if (!pstTraceConfig)
	{
		//�Ҳ�������
		return -3;
	}

	//��������ʱ��
	stFishInfo.lDeadTime = stFishInfo.lBornTime + pstTraceConfig->vPoints[pstTraceConfig->vPoints.size() - 1].iTime;

	//���ҹ켣������ֵ�ʱ��
	long lTraceLastTime = GetLastTraceBornTime(stFishInfo.iTraceID);
	long lIntervalTime = stFishInfo.lBornTime - lTraceLastTime;
	if (lIntervalTime < 5 * 1000)
	{
		stFishInfo.lBornTime += 5 * 1000;
		stFishInfo.lDeadTime += 5 * 1000;
	}

	//��ӡ������־
	//LOGDEBUG("NewFish Info, table %u, fish id %d, seq id %d, trace id %d, born time %ld\n", m_uTableID, stFishInfo.iFishID,
	//	stFishInfo.iFishID, stFishInfo.iTraceID, stFishInfo.lBornTime);

	//���ӵ����б���
	m_vFishData.push_back(stFishInfo);

	return 0;
}

long CFishpondObj::GetLastTraceBornTime(int iTraceID)
{
	long lLastTraceTime = 0;
	for (unsigned i = 0; i < m_vFishData.size(); ++i)
	{
		if ((m_vFishData[i].iTraceID == iTraceID) && (lLastTraceTime < m_vFishData[i].lBornTime))
		{
			lLastTraceTime = m_vFishData[i].lBornTime;
		}
	}

	return lLastTraceTime;
}

//�Ƿ��Ӧ���͵ķ���
bool CFishpondObj::IsRoomType(int iRoomType)
{
	return (m_pstRoomConfig->iRoomTypeID & iRoomType);
}

//�Ƿ��Ӧ����ģʽ
bool CFishpondObj::IsRoomPattern(int iRoomPattern)
{
	return (m_pstRoomConfig->iRoomPattern == iRoomPattern);
}

//�������
int CFishpondObj::PlayerSitDown(CGameRoleObj& stRoleObj)
{
	SeatUserData stUserData;
	stUserData.uiUin = stRoleObj.GetUin();

	for (int iSeat = 0; iSeat < m_pstRoomConfig->iPlayerNum; ++iSeat)
	{
		bool bHasPlayer = false;
		for (unsigned i = 0; i < m_vSeatUserData.size(); ++i)
		{
			if (iSeat == m_vSeatUserData[i].iSeat)
			{
				//��λ���Ѿ�����
				bHasPlayer = true;
				break;
			}
		}

		if (!bHasPlayer)
		{
			//��λ�ÿ���
			stUserData.iSeat = iSeat;
			stUserData.iActiveTime = CTimeUtility::GetNowTime();
			m_vSeatUserData.push_back(stUserData);

			return T_SERVER_SUCCESS;
		}
	}

	return T_ZONE_PARA_ERROR;
}

//ɾ����������ӵ�
void CFishpondObj::ClearUserBullets(int iSeat)
{
	unsigned iLastIndex = m_vBulletData.size();
	for (unsigned i = 0; i < iLastIndex;)
	{
		if (m_vBulletData[i].iSeat != iSeat)
		{
			++i;
			continue;
		}

		m_vBulletData[i] = m_vBulletData[iLastIndex - 1];
		--iLastIndex;
	}

	m_vBulletData.resize(iLastIndex);

	return;
}

//��ȡ�����λ��Ϣ
SeatUserData* CFishpondObj::GetSeatUserByUin(unsigned uiUin)
{
	for (unsigned i = 0; i < m_vSeatUserData.size(); ++i)
	{
		if (m_vSeatUserData[i].uiUin == uiUin)
		{
			return &m_vSeatUserData[i];
		}
	}

	return NULL;
}

//��ȡ����ӵ�
BulletData* CFishpondObj::GetBulletData(unsigned uBulletUniqID)
{
	for (unsigned i = 0; i < m_vBulletData.size(); ++i)
	{
		if (m_vBulletData[i].uUniqueID == uBulletUniqID)
		{
			return &m_vBulletData[i];
		}
	}

	return NULL;
}

//��ȡ�������
FishData* CFishpondObj::GetFishData(unsigned uUniqID)
{
	for (unsigned i = 0; i<m_vFishData.size(); ++i)
	{
		if (m_vFishData[i].uUniqueID == uUniqID)
		{
			return &(m_vFishData[i]);
		}
	}

	return NULL;
}

//��ȡ�����������Ϣ
FormFishOutData* CFishpondObj::GetFormFishInfo(int iFishOutID, int iFishIndex, FishData& stFishInfo)
{
	stFishInfo.Reset();

	std::vector<FormFishOutData>& vFormOutData = m_stFishFormData.vFishOutData;
	std::vector<FreezeData>& vFreezeInfo = m_stFishFormData.vFormFreezeInfo;
	for (unsigned i = 0; i < vFormOutData.size(); ++i)
	{
		if (vFormOutData[i].iOutID != iFishOutID)
		{
			continue;
		}

		//���Index
		if (iFishIndex < 0 || iFishIndex >= vFormOutData[i].iRemainNum)
		{
			return NULL;
		}

		//�����Ѿ�����
		if (!vFormOutData[i].IsFishAlive(iFishIndex))
		{
			return NULL;
		}

		//��ȡ��
		stFishInfo.uUniqueID = iFishOutID * 1000 + (iFishIndex + 1);
		stFishInfo.iFishID = vFormOutData[i].iFishID;
		stFishInfo.iFishSeqID = vFormOutData[i].iFishSeqID;
		stFishInfo.iTraceID = vFormOutData[i].iTraceID;
		stFishInfo.lBornTime = vFormOutData[i].lBeginTime + vFormOutData[i].iInterval*iFishIndex;
		stFishInfo.lDeadTime = 0;
		stFishInfo.iMultiple = vFormOutData[i].iMultiple;
		stFishInfo.iType = vFormOutData[i].iType;

		//��ȡ������Ϣ
		for (unsigned j = 0; j < vFreezeInfo.size(); ++j)
		{
			if (vFreezeInfo[j].uUniqID == stFishInfo.uUniqueID)
			{
				//�ҵ�
				stFishInfo.lFreezeBeginTime = vFreezeInfo[j].lFreezeBeginTime;
				stFishInfo.iFreezeContTime = vFreezeInfo[j].iFreezeContTime;
				stFishInfo.iTotalFreezeTime = vFreezeInfo[j].iTotalFreezeTime;

				break;
			}
		}

		return &vFormOutData[i];
	}

	return NULL;
}

//ɾ����
void CFishpondObj::DeleteFishData(unsigned uUniqID)
{
	for (std::vector<FishData>::iterator it = m_vFishData.begin(); it != m_vFishData.end(); ++it)
	{
		if ((*it).uUniqueID == uUniqID)
		{
			m_vFishData.erase(it);

			return;
		}
	}

	return;
}

//ɾ���ӵ�
void CFishpondObj::DeleteBulletData(unsigned uUniqID)
{
	for (std::vector<BulletData>::iterator it = m_vBulletData.begin(); it != m_vBulletData.end(); ++it)
	{
		if ((*it).uUniqueID == uUniqID)
		{
			m_vBulletData.erase(it);
			return;
		}
	}

	return;
}

//����Ƿ���Ч����
bool CFishpondObj::CheckIsValidHit(long lHitTime, BulletData& stBulletInfo, FishData& stFishInfo, int iFishIndex)
{
	//todo jasonxiong ������������λ������
	return true;

	/*
	//��ȡ���λ��
	FISHPOS stFishPos;
	int iRatio = 0;
	if (!GetFishPosByTime(stFishInfo, lHitTime, stFishPos, iRatio))
	{
	return false;
	}

	if (stFishInfo.iType == FISH_TYPE_SMALLGROUP)
	{
	//С����
	const SmallFishConfig* pstSmallConfig = stBaseCfgMgr.GetSmallFishConfig(stFishInfo.iFishID);
	if (!pstSmallConfig)
	{
	return false;
	}

	stFishPos.iX += pstSmallConfig->vTrackIDs[iFishIndex].iX;
	stFishPos.iY += pstSmallConfig->vTrackIDs[iFishIndex].iY;
	}

	//��ȡ�ӵ�λ��
	FISHPOS stBulletPos;
	if (!GetBulletPosByTime(stBulletInfo, lHitTime, stBulletPos))
	{
	return false;
	}

	const FishConfig* pstFishConfig = stBaseCfgMgr.GetFishConfig(stFishInfo.iFishSeqID, stFishInfo.iFishID);
	if (!pstFishConfig)
	{
	//�Ҳ�������
	return false;
	}

	int iLength = (stFishPos.iX - stBulletPos.iX)*(stFishPos.iX - stBulletPos.iX) + (stFishPos.iY - stBulletPos.iY)*(stFishPos.iY - stBulletPos.iY);
	int iValidLength = pstFishConfig->iWidth * pstFishConfig->iWidth + pstFishConfig->iHeight * pstFishConfig->iHeight;
	iValidLength = iValidLength * iRatio / 4000 + 100 * 100;	//����100�������

	//�ж�λ���Ƿ�����
	if (iLength < iValidLength)
	{
	return true;
	}

	LOGERROR("hit fish pos %d:%d, trace %d, born time %ld\n", stFishPos.iX, stFishPos.iY,
	stFishInfo.iTraceID, stFishInfo.lBornTime);

	LOGERROR("bullet pos %d:%d, seat %d, target pos %d:%d, shoot time %ld, hit time %ld, server time %ld\n",
	stBulletPos.iX, stBulletPos.iY, stBulletInfo.iSeat, stBulletInfo.stTargetPos.iX, stBulletInfo.stTargetPos.iY, stBulletInfo.lShootTime,
	lHitTime, CTimeUtility::GetMSTime());

	return false;
	*/
}

//����߼��Ƿ�����
bool CFishpondObj::CheckIsLogicHit(CGameRoleObj& stRoleObj, SeatUserData& stUserData, const GunConfig& stGunConfig, FishData& stFishInfo, int iAdjust, bool bIsForm)
{
	//���㹫ʽ�� P=(1-X+����ӳɸ���+���ֺ���ӳɸ���-������״̬-����״̬ + �����߼ӳ�)*Fish->Adjust/Fish->Multiple

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	//�������ܼӳɸ���
	int iRedPacketPercent = 0;
	int iNowNewRedNum = 0;
	if (m_pstRoomConfig->iAlgorithmType == 1)
	{
		//���ֺ��ֻ���㷨����1��Ч
		iNowNewRedNum = stRoleObj.GetNowNewRedNum();
	}

	if (stUserData.uReturnID != 0 && stUserData.lUnEffectTime >= CTimeUtility::GetMSTime())
	{
		if ((stData.lTotalReturnSilver + iNowNewRedNum) >= (stGunConfig.iMultiple*stFishInfo.iMultiple))
		{
			if (iNowNewRedNum >= (stGunConfig.iMultiple*stFishInfo.iMultiple))
			{
				//���ֺ���ܸ���������
				iRedPacketPercent = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_RATIOBONUS) + stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWADDRATE);
			}
			else
			{
				//û�����ֺ��
				iRedPacketPercent = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_RATIOBONUS);
			}
		}
	}
	else if (iNowNewRedNum >= (stGunConfig.iMultiple*stFishInfo.iMultiple))
	{
		//���ֺ������
		iRedPacketPercent = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWADDRATE);
	}

	//�������״̬
	int iUserX = GetUserStatus(stUserData);

	int iBasePercent = 1000;

	//��ȡ����������
	int iExpLinePercent = 0;
	const ExpLineConfig* pstConfig = NULL;
	TEXPLINEINFO* pstExpLineInfo = GetRoleExpLineInfo(stUserData.uiUin, pstConfig);
	if (pstExpLineInfo)
	{
		iExpLinePercent = pstConfig->iAddRate;
	}

	long8 lHitPercent = (iBasePercent - stData.iX + iRedPacketPercent - stData.iServerX - iUserX + iExpLinePercent) * iAdjust;
	lHitPercent = lHitPercent / (stFishInfo.iMultiple * 1000);

	//�౶����Ҫ������ı���
	if (stFishInfo.iType == FISH_TYPE_MULTIPLE)
	{
		int iMultiple = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MULTIPLEFISH);
		lHitPercent = lHitPercent * 1000 / iMultiple;

		if (stData.lMultiFishLossNum < 0)
		{
			//�ϸ����ڷ������౶�������߶౶��ϵ���ӳ�
			int iMultipleAddtion = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MULTIPLEFISH_ADD);
			lHitPercent = lHitPercent * 1000 / iMultipleAddtion;
		}
	}

	//��ӡ��Ӫ��־ ������и��� ID = 10004
	RoomAlgorithmData& stAlgorithmData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];
	CZoneOssLog::TraceHitFish(stRoleObj.GetUin(), stRoleObj.GetChannel(), stGunConfig.iGunID, stFishInfo.iFishID, stFishInfo.uUniqueID, 
		bIsForm, stAlgorithmData.lTotalReturnSilver, stAlgorithmData.iX, iRedPacketPercent, stAlgorithmData.iServerX, iUserX, iExpLinePercent, stUserData.iReturnType);

	if (rand() % 1000 < lHitPercent)
	{
		return true;
	}

	return false;
}

//��ȡ��Ҹ���״̬
int CFishpondObj::GetUserStatus(SeatUserData& stUserData)
{
	return 0;

	/*
	if (stUserData.lUserSilverCost == 0)
	{
		return 0;
	}

	int iRewardRate = (int)(stUserData.lUserSilverReward * 1000 / stUserData.lUserSilverCost);
	const StatControlConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetStatConfig(iRewardRate);
	if (!pstConfig)
	{
		//ȡ��������
		LOGERROR("Failed to get user state config, reward rate %d\n", iRewardRate);
		return 0;
	}

	if (pstConfig->iStep <= 0)
	{
		return 0;
	}

	return (iRewardRate - pstConfig->iValueMin)*pstConfig->iDecrease / pstConfig->iStep;
	*/
}

bool FishCompareFunc(const FishData& stFish1, const FishData& stFish2)
{
	return stFish1.iMultiple < stFish2.iMultiple;
}

bool FormFishOutCompareFunc(const FormFishOutData& stFormOut1, const FormFishOutData& stFormOut2)
{
	return stFormOut1.iMultiple < stFormOut2.iMultiple;
}

//��ȡ���������
TEXPLINEINFO* CFishpondObj::GetRoleExpLineInfo(unsigned uiUin, const ExpLineConfig*& pstConfig)
{
	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uiUin);
	if (!pstRoleObj)
	{
		return NULL;
	}

	TEXPLINEINFO* pstExpLineInfo = pstRoleObj->GetExpLineInfo(m_pstRoomConfig->iAlgorithmType);
	if (!pstExpLineInfo)
	{
		return NULL;
	}

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//û��������
	if (pstExpLineInfo->iExpLineID == 0)
	{
		//ѡ��һ��������
		pstConfig = stBaseCfgMgr.GetExpLineConfig(pstExpLineInfo->iExpLineType, m_pstRoomConfig->iAlgorithmType);
		if (!pstConfig)
		{
			LOGERROR("Failed to get exp line config, uin %u, type %d, algorithm type %d\n", uiUin, pstExpLineInfo->iExpLineType,
				m_pstRoomConfig->iAlgorithmType);
			return NULL;
		}

		pstExpLineInfo->iExpLineID = pstConfig->iID;

		return pstExpLineInfo;
	}

	//�Ѿ���������
	pstConfig = stBaseCfgMgr.GetExpLineConfig(pstExpLineInfo->iExpLineID);
	if (!pstConfig)
	{
		LOGERROR("Failed to get exp line config, uin %u, id %d\n", uiUin, pstExpLineInfo->iExpLineID);
		return NULL;
	}

	if (pstExpLineInfo->lCostNum < (pstConfig->iBulletMax * pstConfig->iStandardNum))
	{
		//����Ҫ�л�������
		return pstExpLineInfo;
	}

	//��Ҫ�л�������

	//��������Ӫ��־
	CZoneOssLog::TraceExpLine(*pstRoleObj, pstExpLineInfo->lIntervalWinNum, pstExpLineInfo->iExpLineID, pstExpLineInfo->iBulletNum, pstExpLineInfo->lCostNum);

	if (pstConfig->iNextLine != 0)
	{
		//�л�����һ��
		pstExpLineInfo->iExpLineID = pstConfig->iNextLine;
		pstExpLineInfo->iBulletNum = 0;
		pstExpLineInfo->lCostNum = 0;
		pstExpLineInfo->lIntervalWinNum = 0;

		return pstExpLineInfo;
	}

	int iNewExpLineType = pstExpLineInfo->iExpLineType;
	if (pstExpLineInfo->lUserWinNum > 0)
	{
		iNewExpLineType = pstExpLineInfo->iExpLineType + 1;
	}
	else
	{
		iNewExpLineType = pstExpLineInfo->iExpLineType - 1;
	}

	pstConfig = stBaseCfgMgr.GetExpLineConfig(iNewExpLineType, m_pstRoomConfig->iAlgorithmType);
	if (!pstConfig)
	{
		//�Ҳ����µģ��˻ص��ϵ�
		iNewExpLineType = pstExpLineInfo->iExpLineType;
		pstConfig = stBaseCfgMgr.GetExpLineConfig(iNewExpLineType, m_pstRoomConfig->iAlgorithmType);
	}

	if (!pstConfig)
	{
		LOGERROR("Failed to get exp line config, type %d, algorithm type %d\n", iNewExpLineType, m_pstRoomConfig->iAlgorithmType);
		return NULL;
	}

	//������������
	pstExpLineInfo->iExpLineID = pstConfig->iID;
	pstExpLineInfo->iExpLineType = iNewExpLineType;
	pstExpLineInfo->lUserWinNum = 0;
	pstExpLineInfo->lIntervalWinNum = 0;
	pstExpLineInfo->iBulletNum = 0;
	pstExpLineInfo->lCostNum = 0;

	return pstExpLineInfo;
}

//���������ը�����߼�
void CFishpondObj::BoomFish(SeatUserData& stUserData, FishData*& pstFishInfo, bool bIsFormFish)
{
	//�������¼���
	int iMultiple = 0;
	unsigned uBoomFishUniqID = pstFishInfo->uUniqueID;

	//ը����������߼���ը��BOSS��,��Ʊ��,��ͷ��֮���������
	std::vector<unsigned> vFishUniqIDs;
	std::vector<unsigned> vFormUniqIDs;
	std::vector<BYTE> vSmallFishs;

	std::vector<FishData> vNewFishData;
	for (unsigned i = 0; i < m_vFishData.size(); ++i)
	{
		if (m_vFishData[i].iType == FISH_TYPE_BIGBOSS ||
			m_vFishData[i].iType == FISH_TYPE_PHONETICKET ||
			m_vFishData[i].iType == FISH_TYPE_WARHEAD)
		{
			//��ը��
			vNewFishData.push_back(m_vFishData[i]);
			continue;
		}

		if (m_vFishData[i].uUniqueID == pstFishInfo->uUniqueID)
		{
			//��ը���Լ�
			vNewFishData.push_back(m_vFishData[i]);
			continue;
		}

		//����ը���㱶��
		iMultiple += m_vFishData[i].iMultiple;

		vFishUniqIDs.push_back(m_vFishData[i].uUniqueID);
	}

	//ɾ����
	m_vFishData = vNewFishData;

	//�������ͨ�㣬stFishInfo��Ҫ���»�ȡ
	if (!bIsFormFish)
	{
		pstFishInfo = GetFishData(uBoomFishUniqID);
	}

	pstFishInfo->iMultiple = iMultiple;
	if (pstFishInfo->iMultiple == 0)
	{
		//��д�����û�и�10��
		pstFishInfo->iMultiple = 10;
	}

	//���ͱ�ը��������Ϣ
	SendBoomFishInfoNotify(stUserData, vFishUniqIDs, vSmallFishs, vFormUniqIDs);

	LOGDEBUG("BoomFish, fish uniq id %u, formfish %d, fish id %d, multiple %d, fish num %zu, formfish num %zu\n", pstFishInfo->uUniqueID,
		bIsFormFish, pstFishInfo->iFishID, pstFishInfo->iMultiple, vFishUniqIDs.size(), vFormUniqIDs.size());

	return;

	/*
	//�ȴ����㱶��
	int iLeftMultiple = pstFishInfo->iMultiple;
	std::vector<unsigned> vFishUniqIDs;
	std::vector<unsigned> vFormUniqIDs;
	std::vector<BYTE> vSmallFishs;
	std::vector<unsigned> vDelFishIndexes;
	std::vector<FormFishOutData>& vFishOutData = m_stFishFormData.vFishOutData;
	unsigned uBoomFishUniqID = pstFishInfo->uUniqueID;

	//������
	std::sort(m_vFishData.begin(), m_vFishData.end(), FishCompareFunc);
	std::sort(vFishOutData.begin(), vFishOutData.end(), FormFishOutCompareFunc);

	long lTimeNow = CTimeUtility::GetMSTime();
	for (unsigned i = 0; i < m_vFishData.size(); ++i)
	{
	if (!bIsFormFish && m_vFishData[i].uUniqueID == uBoomFishUniqID)
	{
	//��ը�����Լ�
	continue;
	}

	for (unsigned j = 0; j < vFishOutData.size(); ++j)
	{
	if (vFishOutData[j].iMultiple <= m_vFishData[i].iMultiple)
	{
	//�������е�������
	if (iLeftMultiple <= vFishOutData[j].iMultiple*vFishOutData[j].GetLiveFishNum())
	{
	//�����е��㹻��
	for (int iFishIndex = 0; iFishIndex < vFishOutData[j].iRemainNum; ++iFishIndex)
	{
	if (bIsFormFish && (unsigned)(vFishOutData[j].iOutID * 1000 + iFishIndex + 1) == uBoomFishUniqID)
	{
	//��ը�����Լ�
	continue;
	}

	if (iLeftMultiple < vFishOutData[j].iMultiple)
	{
	break;
	}

	if (!vFishOutData[j].IsFishAlive(iFishIndex))
	{
	//�Ѿ�����
	continue;
	}

	//��ȡ�����Ϣ
	FishData stFishInfo;
	FormFishOutData* pstFormOutData = GetFormFishInfo(vFishOutData[j].iOutID, iFishIndex, stFishInfo);
	if (!pstFormOutData)
	{
	//��ȡ�������ʧ��
	continue;
	}

	//���Ƿ�����Ļ��
	if (!IsFishInScreen(stFishInfo, lTimeNow))
	{
	//������Ļ��
	continue;
	}

	iLeftMultiple -= vFishOutData[j].iMultiple;
	vFormUniqIDs.push_back(vFishOutData[j].iOutID * 1000 + (iFishIndex + 1));
	vFishOutData[j].SetFishAlive(iFishIndex, false);
	}
	}
	else
	{
	//�����е���ղ���
	iLeftMultiple -= (vFishOutData[j].iMultiple * vFishOutData[j].GetLiveFishNum());
	for (int iFishIndex = 0; iFishIndex < vFishOutData[j].iRemainNum; ++iFishIndex)
	{
	if (!vFishOutData[j].IsFishAlive(iFishIndex))
	{
	//�Ѿ�����
	continue;
	}

	if (bIsFormFish && (unsigned)(vFishOutData[j].iOutID * 1000 + iFishIndex + 1) == uBoomFishUniqID)
	{
	//��ը�����Լ�
	continue;
	}

	//��ȡ�����Ϣ
	FishData stFishInfo;
	FormFishOutData* pstFormOutData = GetFormFishInfo(vFishOutData[j].iOutID, iFishIndex, stFishInfo);
	if (!pstFormOutData)
	{
	//��ȡ�������ʧ��
	continue;
	}

	//���Ƿ�����Ļ��
	if (!IsFishInScreen(stFishInfo, lTimeNow))
	{
	//������Ļ��
	continue;
	}

	vFormUniqIDs.push_back(vFishOutData[j].iOutID * 1000 + (iFishIndex + 1));
	vFishOutData[j].SetFishAlive(iFishIndex, false);
	}
	}
	}
	else
	{
	//���������е���
	break;
	}
	}

	//ʹ����������
	if (iLeftMultiple < m_vFishData[i].iMultiple)
	{
	//�Ѿ��Ҳ�������������
	break;
	}
	else
	{
	//���Ƿ�����Ļ��
	if (!IsFishInScreen(m_vFishData[i], lTimeNow))
	{
	//������Ļ��
	continue;
	}

	if (m_vFishData[i].iType == FISH_TYPE_PHONETICKET)
	{
	//��ȯ�㲻��ը
	continue;
	}

	if (m_vFishData[i].iType != FISH_TYPE_SMALLGROUP)
	{
	//����С����
	iLeftMultiple -= m_vFishData[i].iMultiple;
	vFishUniqIDs.push_back(m_vFishData[i].uUniqueID);
	vSmallFishs.push_back(0);
	vDelFishIndexes.push_back(i);
	}
	else
	{
	//С����
	vFishUniqIDs.push_back(m_vFishData[i].uUniqueID);

	BYTE ucOldIndex = m_vFishData[i].cIndex;
	for (unsigned j = 0; j < 8; ++j)
	{
	if (iLeftMultiple < m_vFishData[i].iMultiple)
	{
	//�Ѿ��Ҳ�������������
	break;
	}

	//С���������5����
	if (m_vFishData[i].cIndex & (0x01 << j))
	{
	//��λ������
	iLeftMultiple -= m_vFishData[i].iMultiple;
	m_vFishData[i].cIndex &= ~(0x01 << j);
	}
	}

	vSmallFishs.push_back(ucOldIndex - m_vFishData[i].cIndex);
	if (m_vFishData[i].cIndex == 0)
	{
	//û�����ˣ�ɾ��
	vDelFishIndexes.push_back(i);
	}
	}

	}
	}

	//ɾ����
	for (unsigned i = 0; i < vDelFishIndexes.size(); ++i)
	{
	m_vFishData[vDelFishIndexes[i]] = m_vFishData[m_vFishData.size() - i - 1];
	}
	m_vFishData.resize(m_vFishData.size() - vDelFishIndexes.size());

	//�������ͨ�㣬stFishInfo��Ҫ���»�ȡ
	if (!bIsFormFish)
	{
	pstFishInfo = GetFishData(uBoomFishUniqID);
	}

	//���±�ը�㱶��
	pstFishInfo->iMultiple -= iLeftMultiple;

	//���ͱ�ը��������Ϣ
	SendBoomFishInfoNotify(vFishUniqIDs, vSmallFishs, vFormUniqIDs);

	LOGDEBUG("BoomFish, fish uniq id %u, formfish %d, fish id %d, multiple %d, left multiple %d, fish num %zu, formfish num %zu\n", pstFishInfo->uUniqueID,
	bIsFormFish, pstFishInfo->iFishID, pstFishInfo->iMultiple, iLeftMultiple, vFishUniqIDs.size(), vFormUniqIDs.size());

	return;
	*/
}

//���Ƿ�����Ļ��
bool CFishpondObj::IsFishInScreen(FishData& stFishInfo, long lTimeNow)
{
	//FISHPOS stPos;
	//int iRatio = 0;

	//todo jasonxiong ��������
	return true;

	/*
	if (!GetFishPosByTime(stFishInfo, lTimeNow, stPos, iRatio))
	{
	//��ȡλ��ʧ��
	return false;
	}

	if (abs(stPos.iX) > CLIENT_SCREEN_WIDTH / 2 || abs(stPos.iY) > CLIENT_SCREEN_HEIGHT / 2)
	{
	//������Ļ��
	return false;
	}
	*/

	return true;
}

//�����ȯ
bool CFishpondObj::AddUserTicket(CGameRoleObj& stRoleObj, unsigned uFishUniqID, bool bIsFormFish, int iMultiple, int iNum, bool bIsTicketFish)
{
	if (iNum == 0)
	{
		return true;
	}

	long8 lRandTicketNum = 0;
	if (bIsTicketFish)
	{
		//��ȯ�����
		lRandTicketNum = iNum;
	}
	else
	{
		//���㹫ʽΪ: ���� = iRewardSilver * ConfigY /(100*1000); ���»�����ȡ���������������
		int iConfigY = CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_PHONETICKETRATE);
		lRandTicketNum = iNum * iConfigY;

		if (lRandTicketNum % (100 * 1000) > rand() % (100 * 1000))
		{
			lRandTicketNum = lRandTicketNum / (100 * 1000) + 1;
		}
		else
		{
			lRandTicketNum = lRandTicketNum / (100 * 1000);
		}
	}

	if (lRandTicketNum == 0)
	{
		return true;
	}

	//��ӵ�ȯ
	if (!CResourceUtility::AddUserRes(stRoleObj, RESOURCE_TYPE_TICKET, lRandTicketNum))
	{
		return false;
	}

	//���ͻ�õ�ȯ����Ϣ
	SendAddTicketNotify(stRoleObj.GetUin(), uFishUniqID, bIsFormFish, lRandTicketNum);

	//��ӡ��Ӫ��־ �����õ�ȯ ID = 10
	//todo jasonxiong ��ͳһ����
	//GetGameLogic()->OssFishPhoneTicketInfo(player, iMultiple, lRandTicketNum, bIsTicketFish);

	return true;
}

//���º����Ϣ
void CFishpondObj::UpdateRedPacketInfo(CGameRoleObj& stRoleObj, SeatUserData& stUserData)
{
	//��ȡ�����㷨����
	RoomAlgorithmData& stData = FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType];

	if (stUserData.uCycleID != stData.uCycleID)
	{
		//���δ���뵱ǰ��������
		return;
	}

	//���ж��Ƿ��Ѿ��к��
	if (stUserData.uReturnID == stData.uReturnID)
	{
		//�Ѿ������º��
		return;
	}

	//�ж��Ƿ�������֮ǰ��¼
	if (stRoleObj.GetLoginTime() >= stData.lLastUpdateTime)
	{
		//������ɺ��¼
		return;
	}

	//���Ի�ȡһ�����
	FishAlgorithm::GetOneRedPacket(stRoleObj, stUserData, m_pstRoomConfig->iAlgorithmType);

	return;
}

//��������ʱ��
void CFishpondObj::SendFishFormTime(CGameRoleObj* pstRoleObj, bool bIsBegin)
{
	//��ʱû������
	return;

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_FORMTIME_NOTIFY);

	Zone_FormTime_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_formtime_notify();
	pstNotify->set_bisbegin(bIsBegin);
	pstNotify->set_ltime((bIsBegin ? m_stFishFormData.lNextUpdateTime : m_stFishFormData.lFormEndTime));

	if (pstRoleObj)
	{
		//���͸����
		CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pstRoleObj);
	}
	else
	{
		//���͸�����
		SendZoneMsgToFishpond(ms_stZoneMsg);
	}
}

//���������λ��Ϣ
void CFishpondObj::SendSeatUserInfo(CGameRoleObj* pstToRole, CGameRoleObj* pstSeatRole)
{
	//pstToRole		���͵�Ŀ����ң�Ϊ�������͸�������
	//pstSeatRole	���͵������Ϣ�����Ϊ��������������λ����ҵ���Ϣ

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_SEATUSER_NOTIFY);
	Zone_SeatUser_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_seatuser_notify();

	if (pstSeatRole)
	{
		//����pstSeatRole����Ϣ
		SeatUserData* pstUserData = GetSeatUserByUin(pstSeatRole->GetUin());
		if (!pstUserData)
		{
			return;
		}

		SEATUSERINFO* pstOneInfo = pstNotify->add_stusers();
		pstOneInfo->set_iseat(pstUserData->iSeat);
		pstOneInfo->set_uin(pstSeatRole->GetUin());
		pstOneInfo->set_igunid(pstSeatRole->GetWeaponInfo().iWeaponID);
		pstOneInfo->set_lcoins(pstSeatRole->GetResource(RESOURCE_TYPE_COIN));
		pstOneInfo->set_ltickets(pstSeatRole->GetResource(RESOURCE_TYPE_TICKET));
		pstOneInfo->set_strnickname(pstSeatRole->GetNickName());
		pstOneInfo->set_igunstyleid(pstSeatRole->GetWeaponInfo().iStyleID);
		pstOneInfo->set_strpicid(pstSeatRole->GetPicID());
		pstOneInfo->set_iviplevel(pstSeatRole->GetVIPLevel());
	}
	else
	{
		//���������������˵���Ϣ
		for (unsigned i = 0; i < m_vSeatUserData.size(); ++i)
		{
			CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(m_vSeatUserData[i].uiUin);
			if (!pstRoleObj)
			{
				//�Ҳ��������Ϣ
				continue;
			}

			SEATUSERINFO* pstOneInfo = pstNotify->add_stusers();
			pstOneInfo->set_iseat(m_vSeatUserData[i].iSeat);
			pstOneInfo->set_uin(pstRoleObj->GetUin());
			pstOneInfo->set_igunid(pstRoleObj->GetWeaponInfo().iWeaponID);
			pstOneInfo->set_lcoins(pstRoleObj->GetResource(RESOURCE_TYPE_COIN));
			pstOneInfo->set_ltickets(pstRoleObj->GetResource(RESOURCE_TYPE_TICKET));
			pstOneInfo->set_strnickname(pstRoleObj->GetNickName());
			pstOneInfo->set_igunstyleid(pstRoleObj->GetWeaponInfo().iStyleID);
			pstOneInfo->set_strpicid(pstRoleObj->GetPicID());
			pstOneInfo->set_iviplevel(pstRoleObj->GetVIPLevel());
		}
	}

	if (pstToRole)
	{
		//���͸��������
		CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pstToRole);
	}
	else
	{
		//���͸��������������
		SendZoneMsgToFishpond(ms_stZoneMsg);
	}

	return;
}

//��������Ϣ
void CFishpondObj::SendFishInfoToUser(CGameRoleObj* pstTargetRole, const std::vector<FishData>& vFishes)
{
	if (vFishes.size() == 0)
	{
		return;
	}

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_FISHINFO_NOTIFY);
	Zone_FishInfo_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_fishinfo_notify();

	for (unsigned i = 0; i < vFishes.size(); ++i)
	{
		FISHINFO* pstFishInfo = pstNotify->add_stfishes();
		pstFishInfo->set_uuniqid(vFishes[i].uUniqueID);
		pstFishInfo->set_ifishid(vFishes[i].iFishID);
		pstFishInfo->set_ifishseqid(vFishes[i].iFishSeqID);
		pstFishInfo->set_itraceid(vFishes[i].iTraceID);
		pstFishInfo->set_lbortime(vFishes[i].lBornTime);
		pstFishInfo->set_ldeadtime(FishAlgorithm::astData[m_pstRoomConfig->iAlgorithmType].lTotalReturnSilver);
		pstFishInfo->set_cindex(vFishes[i].cIndex);
	}

	if (pstTargetRole)
	{
		//���͸�����
		CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pstTargetRole);
	}
	else
	{
		//���͸�����
		SendZoneMsgToFishpond(ms_stZoneMsg);
	}

	return;
}

//�����������ӵ���Ϣ�����
void CFishpondObj::SendBulletInfoToUser(CGameRoleObj& stRoleObj)
{
	if (m_vBulletData.size() == 0)
	{
		//û���ӵ�
		return;
	}

	//��ȡ�����λ��Ϣ
	SeatUserData* pstUserData = GetSeatUserByUin(stRoleObj.GetUin());
	if (!pstUserData)
	{
		return;
	}

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_BULLETINFO_NOTIFY);
	Zone_BulletInfo_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_bulletinfo_notify();

	for (unsigned i = 0; i < m_vBulletData.size(); ++i)
	{
		if (m_vBulletData[i].iSeat == pstUserData->iSeat)
		{
			//�������Լ���
			continue;
		}

		BULLETINFO* pstOneInfo = pstNotify->add_stbullets();
		pstOneInfo->set_uuniqid(m_vBulletData[i].uUniqueID);
		pstOneInfo->set_igunid(m_vBulletData[i].iGunID);
		pstOneInfo->set_iseat(m_vBulletData[i].iSeat);
		pstOneInfo->mutable_sttargetpos()->set_ix(m_vBulletData[i].stTargetPos.iX);
		pstOneInfo->mutable_sttargetpos()->set_iy(m_vBulletData[i].stTargetPos.iY);
		pstOneInfo->set_lshoottime(m_vBulletData[i].lShootTime);
		pstOneInfo->set_ufishuniqid(m_vBulletData[i].uFishUniqID);
		pstOneInfo->set_bisaimformfish(m_vBulletData[i].bIsAimFormFish);
		pstOneInfo->set_ifishindex(m_vBulletData[i].iFishIndex);
	}

	CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, &stRoleObj);

	return;
}

//����������Ϣ�����
void CFishpondObj::SendFishpondInfoToUser(CGameRoleObj& stRoleObj)
{
	//���������������˸����
	SendSeatUserInfo(&stRoleObj, NULL);

	//��������������������
	SendFishInfoToUser(&stRoleObj, m_vFishData);

	//�����������ӵ���Ϣ�����
	SendBulletInfoToUser(stRoleObj);

	return;
}

//����������ϸ��Ϣ
void CFishpondObj::SendFishFormInfo(CGameRoleObj* pstRoleObj)
{
	if (m_stFishFormData.vFishOutData.size() == 0)
	{
		return;
	}

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_FORMINFO_NOTIFY);
	Zone_FormInfo_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_forminfo_notify();
	for (unsigned i = 0; i < m_stFishFormData.vFishOutData.size(); ++i)
	{
		FORMFISHOUTINFO* pstFormInfo = pstNotify->add_stforminfos();
		pstFormInfo->set_ioutid(m_stFishFormData.vFishOutData[i].iOutID);
		pstFormInfo->set_ifishid(m_stFishFormData.vFishOutData[i].iFishID);
		pstFormInfo->set_ifishseqid(m_stFishFormData.vFishOutData[i].iFishSeqID);
		pstFormInfo->set_itraceid(m_stFishFormData.vFishOutData[i].iTraceID);
		pstFormInfo->set_lbegintime(m_stFishFormData.vFishOutData[i].lBeginTime);
		pstFormInfo->set_iremainum(m_stFishFormData.vFishOutData[i].iRemainNum);
		pstFormInfo->set_iinterval(m_stFishFormData.vFishOutData[i].iInterval);

		for (int i = 0; i < 4; ++i)
		{
			pstFormInfo->add_szalivefish(m_stFishFormData.vFishOutData[i].szAliveFish[i]);
		}
	}

	if (pstRoleObj)
	{
		//���͸����
		CZoneMsgHelper::SendZoneMsgToRole(ms_stZoneMsg, pstRoleObj);
	}
	else
	{
		//���͸�����
		SendZoneMsgToFishpond(ms_stZoneMsg);
	}

	return;
}

//�����˳�������Ϣ
void CFishpondObj::SendExitFishpondAll(int iSeat, bool bForceExit)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_EXITFISH_NOTIFY);

	Zone_ExitFish_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_exitfish_notify();
	pstNotify->set_iseat(iSeat);
	pstNotify->set_bforceexit(bForceExit);

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return;
}

//������̨�л���֪ͨ
void CFishpondObj::SendChangeGunNotify(SeatUserData& stUserData, int iNewGunID, bool bIsStyle)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_CHANGEGUN_NOTIFY);

	Zone_ChangeGun_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_changegun_notify();
	pstNotify->set_iseat(stUserData.iSeat);
	pstNotify->set_inewgunid(iNewGunID);
	pstNotify->set_bisstyle(bIsStyle);

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return;
}

//�����ӵ���Ϣ
void CFishpondObj::SendShootBulletNotify(BulletData& stBulletInfo)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_SHOOTBULLET_NOTIFY);

	Zone_ShootBullet_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_shootbullet_notify();
	pstNotify->set_uuniqid(stBulletInfo.uUniqueID);
	pstNotify->set_igunid(stBulletInfo.iGunID);
	pstNotify->set_iseat(stBulletInfo.iSeat);
	pstNotify->mutable_sttargetpos()->set_ix(stBulletInfo.stTargetPos.iX);
	pstNotify->mutable_sttargetpos()->set_iy(stBulletInfo.stTargetPos.iY);
	pstNotify->set_lshoottime(stBulletInfo.lShootTime);
	pstNotify->set_ufishuniqid(stBulletInfo.uFishUniqID);
	pstNotify->set_bisaimformfish(stBulletInfo.bIsAimFormFish);
	pstNotify->set_ifishindex(stBulletInfo.iFishIndex);
	pstNotify->set_biswildbullet((stBulletInfo.iWildNum != 0));

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return;
}

//�������������Ϣ
void CFishpondObj::SendHitFishInfoNotify(SeatUserData& stUserData, unsigned uBulletUniqID, unsigned uFishUniqID, int iRewardSilver,
	bool bIsForm, int iCost, int iFishIndex, int iMultipleFish)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_HITFISH_NOTIFY);

	Zone_HitFish_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_hitfish_notify();
	pstNotify->set_iseat(stUserData.iSeat);
	pstNotify->set_ubulletuniqid(uBulletUniqID);
	pstNotify->set_ufishuniqid(uFishUniqID);
	pstNotify->set_irewardcoins(iRewardSilver);
	pstNotify->set_bisform(bIsForm);
	pstNotify->set_ifishindex(iFishIndex);
	pstNotify->set_imultifish(iMultipleFish);
	pstNotify->set_ibulletlivetime(stUserData.lUnEffectTime > CTimeUtility::GetMSTime());

	SendZoneMsgToFishpond(ms_stZoneMsg);
}

//���ͱ�ը��������Ϣ
void CFishpondObj::SendBoomFishInfoNotify(SeatUserData& stUserData, const std::vector<unsigned>& vFishUniqIDs, const std::vector<BYTE>& vSmallFishs,
	const std::vector<unsigned>& vFormUniqIDs)
{
	if (vFishUniqIDs.empty() && vFormUniqIDs.empty())
	{
		return;
	}

	/*
	if (vFishUniqIDs.size() != vSmallFishs.size())
	{
		return;
	}
	*/

	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_BOOMFISHINFO_NOTIFY);

	Zone_BoomFishInfo_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_boomfishinfo_notify();
	pstNotify->set_iseat(stUserData.iSeat);

	for (unsigned i = 0; i < vFishUniqIDs.size(); ++i)
	{
		pstNotify->add_ufishuniqids(vFishUniqIDs[i]);
		//pstNotify->add_usmallfishindex(vSmallFishs[i]);
	}

	for (unsigned i = 0; i < vFormUniqIDs.size(); ++i)
	{
		pstNotify->add_uformuniqids(vFormUniqIDs[i]);
	}

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return;
}

//���ͻ�õ�ȯ����Ϣ
void CFishpondObj::SendAddTicketNotify(unsigned uiUin, unsigned uFishUniqID, bool bIsFormFish, long8 lRandTicketNum)
{
	CZoneMsgHelper::GenerateMsgHead(ms_stZoneMsg, MSGID_ZONE_ADDTICKET_NOTIFY);

	Zone_AddTicket_Notify* pstNotify = ms_stZoneMsg.mutable_stbody()->mutable_m_stzone_addticket_notify();
	pstNotify->set_uiuin(uiUin);
	pstNotify->set_ufishuniqid(uFishUniqID);
	pstNotify->set_bisformfish(bIsFormFish);
	pstNotify->set_iaddnum(lRandTicketNum);

	SendZoneMsgToFishpond(ms_stZoneMsg);

	return;
}

//������Ϣ��������������
void CFishpondObj::SendZoneMsgToFishpond(GameProtocolMsg& stMsg)
{
	TRoleObjList stRoleList;
	stRoleList.m_iRoleNumber = m_vSeatUserData.size();
	for (int i = 0; i < stRoleList.m_iRoleNumber; ++i)
	{
		stRoleList.m_apstRoleObj[i] = CUnitUtility::GetRoleByUin(m_vSeatUserData[i].uiUin);
	}

	//������Ϣ
	CZoneMsgHelper::SendZoneMsgToRoleList(stMsg, stRoleList);

	return;
}

//���������
void CFishpondObj::SendFishHorseLamp(CGameRoleObj& stRoleObj, int iLampID, int iParam1, int iParam2, int iParam3)
{
	std::vector<HorseLampData> vDatas;

	HorseLampData stOneData;
	stOneData.iLampID = iLampID;

	char szParam[64] = { 0 };

	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%s", stRoleObj.GetNickName());
	stOneData.vParams.push_back(szParam);

	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%d", iParam1);
	stOneData.vParams.push_back(szParam);

	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%d", iParam2);
	stOneData.vParams.push_back(szParam);

	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%d", iParam3);
	stOneData.vParams.push_back(szParam);

	vDatas.push_back(stOneData);

	CChatUtility::SendHorseLamp(vDatas);

	return;
}

//�������
void CFishpondObj::Reset()
{
	m_pPrev = NULL;
	m_pNext = NULL;

	//���ID
	m_uTableID = 0;

	//������ڷ���ID
	m_iFishRoomID = 0;

	//�������
	m_pstRoomConfig = NULL;

	//��λ��Ϣ
	m_vSeatUserData.clear();

	//��س������
	m_vOutFishRule.clear();

	//����е��ӵ�
	m_vBulletData.clear();

	//������Ϣ
	m_stFishFormData.Reset();

	//��س���������Ϣ
	m_mFishLimitData.clear();

	//����������Ϣ
	m_vFishData.clear();

	m_bStopOutFish = false;

	m_lLastTickTime = 0;

	return;
}
