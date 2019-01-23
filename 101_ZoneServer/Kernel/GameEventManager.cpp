
#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "LogAdapter.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/GameSession.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/SessionManager.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "FishGame/FishAlgorithm.h"
#include "FishGame/FishUtility.h"
#include "FishGame/FishpondObj.h"
#include "Quest/QuestUtility.h"
#include "Rank/RankInfoManager.h"
#include "Lottery/LasvegasManager.h"
#include "Mail/MailUtility.h"
#include "Login/LogoutHandler.hpp"

#include "GameEventManager.hpp"

//����֪ͨ
void CGameEventManager::NotifyKillFish(CGameRoleObj& stRoleObj, int iFishID, int iFishType, int iKillNum, int iResType, int iResNum)
{
	CFishpondObj* pstFishpondObj = CFishUtility::GetFishpondByID(stRoleObj.GetTableID());
	if (!pstFishpondObj)
	{
		LOGERROR("Failed to notify kill fish, uin %u, table id %d\n", stRoleObj.GetUin(), stRoleObj.GetTableID());
		return;
	}

	int iModeID = pstFishpondObj->GetRoomConfig()->iRoomPattern;
	int iRoomTypeID = pstFishpondObj->GetRoomConfig()->iRoomTypeID;

	//��������
	CQuestUtility::OnKillFish(stRoleObj, iModeID, iRoomTypeID, iFishID, iKillNum, iFishType);

	//��������Դ
	CQuestUtility::OnGetFishRes(stRoleObj, iModeID, iRoomTypeID, iResType, iResNum);

	return;
}

//ʹ�ü���֪ͨ
void CGameEventManager::NotifyUseSkill(CGameRoleObj& stRoleObj, int iSkillID, int iUseNum)
{
	CFishpondObj* pstFishpondObj = CFishUtility::GetFishpondByID(stRoleObj.GetTableID());
	if (!pstFishpondObj)
	{
		LOGERROR("Failed to notify kill fish, uin %u, table id %d\n", stRoleObj.GetUin(), stRoleObj.GetTableID());
		return;
	}

	int iModeID = pstFishpondObj->GetRoomConfig()->iRoomPattern;
	int iRoomTypeID = pstFishpondObj->GetRoomConfig()->iRoomTypeID;

	//ʹ�ü���
	CQuestUtility::OnUseSkill(stRoleObj, iModeID, iRoomTypeID, iSkillID, iUseNum);

	return;
}

//�л�����ģʽ֪ͨ
void CGameEventManager::NotifyChangeOpera(CGameRoleObj& stRoleObj, int iOperaMode, int iNum)
{
	//�л�����ģʽ
	CQuestUtility::OnChangeOpera(stRoleObj, iOperaMode, iNum);
}

//�齱֪ͨ
void CGameEventManager::NotifyLottery(CGameRoleObj& stRoleObj, int iLotteryNum)
{
	//�齱
	CQuestUtility::OnUserLottery(stRoleObj, iLotteryNum);
}

//�������֪ͨ
void CGameEventManager::NotifyFinQuest(CGameRoleObj& stRoleObj, int iQuestType, int iQuestID)
{
	//�������
	CQuestUtility::OnFinQuest(stRoleObj, iQuestType, iQuestID);
}

//��õ���֪ͨ
void CGameEventManager::NotifyGetItem(CGameRoleObj& stRoleObj, int iItemType, int iItemID, int iItemNum)
{
	//ʹ�õ���
	CQuestUtility::OnGetItem(stRoleObj, iItemType, iItemID, iItemNum);
}

//��¼֪ͨ
void CGameEventManager::NotifyLogin(CGameRoleObj& stRoleObj)
{
	int iLastLoginTime = stRoleObj.GetLastLoginTime();
	int iLoginTime = stRoleObj.GetLoginTime();

	//����ͷ��
	CGameSessionObj* pstSessionObj = CModuleHelper::GetSessionManager()->FindSessionByRoleID(stRoleObj.GetRoleID());
	if (pstSessionObj)
	{
		const char* pstrNewPicID = pstSessionObj->GetPictureID();
		if (strlen(pstrNewPicID) != 0 && stRoleObj.GetPicID() != pstrNewPicID)
		{
			//����ͷ��
			stRoleObj.SetPicID(pstrNewPicID);

			//����Ϊǿ�Ƹ������а�
			stRoleObj.SetUpdateRank(true);
		}
	}

	//����Ϊû��VIP��Ҳ���
	stRoleObj.SetIsVIPAddCoins(false);

	if (!CTimeUtility::IsInSameDay(iLastLoginTime, iLoginTime))
	{
		//�����һ�ε�¼

		//ˢ��10Ԫ���Ѷһ�����
		stRoleObj.GetExchangeManager().SetPersonLimit(PERSONLIMIT_TYPE_TENBILL, 0);

		//���ӵ�¼����
		CQuestUtility::OnLoginDay(stRoleObj);

		//������ҵ�½����
		stRoleObj.SetLoginDays(stRoleObj.GetLoginDays()+1);

		//���ú������س齱����
		stRoleObj.SetLotteryNum(0);

		//VIP��Ȩ��Ҳ���
		const VipLevelConfig* pstVipConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetVipConfig(stRoleObj.GetVIPLevel());
		if (pstVipConfig)
		{
			for (unsigned i = 0; i < pstVipConfig->vPrivs.size(); ++i)
			{
				if (pstVipConfig->vPrivs[i].iPrivType != VIP_PRIV_ADDCOINS)
				{
					continue;
				}

				if (stRoleObj.GetResource(RESOURCE_TYPE_COIN) < pstVipConfig->vPrivs[i].aiParams[2])
				{
					//��ȡ������֮ǰ�Ľ������ ��Ӫ��־ʹ��
					long8 lOldCoinNum = stRoleObj.GetResource(RESOURCE_TYPE_COIN);

					//������
					stRoleObj.SetResource(RESOURCE_TYPE_COIN, pstVipConfig->vPrivs[i].aiParams[2]);
					stRoleObj.SetIsVIPAddCoins(true);

					//��ӡ��Ӫ��־ ���߲�����
					CZoneOssLog::TraceVipReward(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), stRoleObj.GetVIPLevel(),
						lOldCoinNum, pstVipConfig->vPrivs[i].aiParams[2]);
				}

				break;
			}
		}

		//�¿�����ʼ�
		int iBoxMailEndTime = (stRoleObj.GetMonthEndTime() < iLoginTime) ? stRoleObj.GetMonthEndTime() : (iLoginTime-24*60*60);
		int iLastMonthCardTime = stRoleObj.GetLastMonthTime();
		int iMonthBoxID = CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_MONTHCARD_BOXID);
		while (!CTimeUtility::IsInSameDay(iLastMonthCardTime, iBoxMailEndTime))
		{
			if (iLastMonthCardTime > iBoxMailEndTime)
			{
				break;
			}

			//�����ʼ�
			CMailUtility::SendMonthCardMail(stRoleObj, iMonthBoxID, 1);
			iLastMonthCardTime += 24 * 60 * 60;
		}

		stRoleObj.SetLastMonthTime(iLastMonthCardTime);
	}
}

//����֪ͨ
void CGameEventManager::NotifyLogout(CGameRoleObj& stRoleObj)
{
	int iTimeNow = CTimeUtility::GetNowTime();

	//����ʱ��
	CQuestUtility::OnRoleOnlineTime(stRoleObj, iTimeNow-stRoleObj.GetLoginTime());
}

//�����ӵ�֪ͨ
void CGameEventManager::NotifyShootBullet(CGameRoleObj& stRoleObj, int iGunID, int iCost)
{
	//�����ӵ�
	CQuestUtility::OnShootBullet(stRoleObj, iGunID, iCost);
}

//����ʱ�����֪ͨ
void CGameEventManager::NotifyOnlineTime(CGameRoleObj& stRoleObj, int iTimeNow, int iAddOnlineTime)
{
	if (!CTimeUtility::IsInSameDay(stRoleObj.GetLastOnlineUpdate(), iTimeNow))
	{
		//�Ѿ�����
		stRoleObj.SetLastOnlineUpdate(iTimeNow);
		stRoleObj.SetDayOnlineTime(0);

		return;
	}

	stRoleObj.SetLastOnlineUpdate(iTimeNow);
	stRoleObj.SetDayOnlineTime(stRoleObj.GetDayOnlineTime()+iAddOnlineTime);
	stRoleObj.SetOnlineTotalTime(stRoleObj.GetOnlineTotalTime() + iAddOnlineTime);

	//�����Կ����Ƿ���
	if (CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_WALLOWSWITCH))
	{
		//���ǳ������ҵ�������ʱ������3Сʱ
		if (stRoleObj.GetRealNameStat() != REAL_STAT_ADULT && stRoleObj.GetDayOnlineTime() >= 3 * 60 * 60)
		{
			//���������
			CLogoutHandler::LogoutRole(&stRoleObj, LOGOUT_REASON_WALLOW);
		}
	}

	return;
}

void CGameEventManager::NotifyTick()
{
    // ����ϵͳϵͳ����æ״̬, ��̬����ϵͳ����
    EGameServerStatus enServerStatus = g_enServerStatus;

	//ȫ���㷨Tick
	FishAlgorithm::OnTick();

	//���а�Tick
	CRankInfoManager::Instance()->OnTick();

	//��ת��Tick
	CLasvegasManager::Instance()->OnTick();

    int iNumber = (enServerStatus == GAME_SERVER_STATUS_BUSY) ? 2 : 200;
    for (int i = 0; i < iNumber; i++)
    {
        int iRoleIdx = CUnitUtility::IterateRoleIdx();
        if (iRoleIdx < 0)
        {
            break;
        }

        CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iRoleIdx);
        if (!pRoleObj)
        {
            continue;
        }

        TUNITINFO* pUnitInfo = &pRoleObj->GetRoleInfo().stUnitInfo;
        // ����ɾ����λ
        if (CUnitUtility::IsUnitStatusSet(pUnitInfo, EGUS_DELETE))
        {
            CUnitUtility::DeleteUnit(pUnitInfo);
            continue;
        }

        // ��ɫTick
        pRoleObj->OnTick();
    }

    return;
}
