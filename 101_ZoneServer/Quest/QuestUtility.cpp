
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"

#include "QuestUtility.h"

//��������
void CQuestUtility::OnKillFish(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iFishID, int iKillNum, int iFishType)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_KILLFISH, iModeID, iRoomTypeID, iFishID, iKillNum, iFishType);
}

//��������Դ
void CQuestUtility::OnGetFishRes(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iResType, int iResNum)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_GETFISHRES, iModeID, iRoomTypeID, iResType, iResNum);
}

//ʹ�ü���
void CQuestUtility::OnUseSkill(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iSkillID, int iUseNum)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_USESKILL, iModeID, iRoomTypeID, iSkillID, iUseNum);
}

//�л�����ģʽ
void CQuestUtility::OnChangeOpera(CGameRoleObj& stRoleObj, int iOperaMode, int iNum)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_CHANGEOPERA, iOperaMode, 0, 0, iNum);
}

//�齱
void CQuestUtility::OnUserLottery(CGameRoleObj& stRoleObj, int iLotteryNum)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_LOTTERY, 0, 0, 0, iLotteryNum);
}

//�������
void CQuestUtility::OnFinQuest(CGameRoleObj& stRoleObj, int iQuestType, int iQuestID)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_FINQUEST, iQuestType, iQuestID, 0, 1);
}

//��õ���
void CQuestUtility::OnGetItem(CGameRoleObj& stRoleObj, int iItemType, int iItemID, int iItemNum)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_GETITEM, iItemType, iItemID, 0, iItemNum);
}

//��¼����
void CQuestUtility::OnLoginDay(CGameRoleObj& stRoleObj)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	//����1��1��ʾ�ۼƵ�½��2��ʾ������½
	stQuestMgr.OnQuestNeedChange(QUEST_NEED_LOGINDAY, 1, 0, 0, 1);
	stQuestMgr.OnQuestNeedChange(QUEST_NEED_LOGINDAY, 2, 0, 0, 1);
}

//����ʱ��
void CQuestUtility::OnRoleOnlineTime(CGameRoleObj& stRoleObj, int iAddOnlineTime)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnQuestNeedChange(QUEST_NEED_ONLINETIME, 1, 0, 0, iAddOnlineTime);
}

//�����ӵ�
void CQuestUtility::OnShootBullet(CGameRoleObj& stRoleObj, int iGunID, int iCost)
{
	CQuestManager& stQuestMgr = stRoleObj.GetQuestManager();

	stQuestMgr.OnShootBullet(iGunID, iCost);
}
