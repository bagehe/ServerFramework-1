#pragma once

//���������

class CGameRoleObj;
class CQuestUtility
{
public:

	//��������
	static void OnKillFish(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iFishID, int iKillNum, int iFishType);

	//��������Դ
	static void OnGetFishRes(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iResType, int iResNum);

	//ʹ�ü���
	static void OnUseSkill(CGameRoleObj& stRoleObj, int iModeID, int iRoomTypeID, int iSkillID, int iUseNum);

	//�л�����ģʽ
	static void OnChangeOpera(CGameRoleObj& stRoleObj, int iOperaMode, int iNum);

	//�齱
	static void OnUserLottery(CGameRoleObj& stRoleObj, int iLotteryNum);

	//�������
	static void OnFinQuest(CGameRoleObj& stRoleObj, int iQuestType, int iQuestID);

	//��õ���
	static void OnGetItem(CGameRoleObj& stRoleObj, int iItemType, int iItemID, int iItemNum);

	//��¼����
	static void OnLoginDay(CGameRoleObj& stRoleObj);

	//����ʱ��
	static void OnRoleOnlineTime(CGameRoleObj& stRoleObj, int iAddOnlineTime);

	//�����ӵ�
	static void OnShootBullet(CGameRoleObj& stRoleObj, int iGunID, int iCost);
};
