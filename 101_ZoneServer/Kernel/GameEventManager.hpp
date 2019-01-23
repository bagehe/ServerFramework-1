#pragma once

#include "GameProtocol.hpp"
#include "GameObjCommDef.hpp"
#include "Kernel/GameRole.hpp"
#include "CommDefine.h"

//����������¼�֪ͨ��������

class CGameEventManager
{
public:

	//����֪ͨ
	static void NotifyKillFish(CGameRoleObj& stRoleObj, int iFishID, int iFishType, int iKillNum, int iResType, int iResNum);

	//ʹ�ü���֪ͨ
	static void NotifyUseSkill(CGameRoleObj& stRoleObj, int iSkillID, int iUseNum);

	//�л�����ģʽ֪ͨ
	static void NotifyChangeOpera(CGameRoleObj& stRoleObj, int iOperaMode, int iNum);

	//�齱֪ͨ
	static void NotifyLottery(CGameRoleObj& stRoleObj, int iLotteryNum);

	//�������֪ͨ
	static void NotifyFinQuest(CGameRoleObj& stRoleObj, int iQuestType, int iQuestID);

	//��õ���֪ͨ
	static void NotifyGetItem(CGameRoleObj& stRoleObj, int iItemType, int iItemID, int iItemNum);

	//��¼֪ͨ
	static void NotifyLogin(CGameRoleObj& stRoleObj);

	//����֪ͨ
	static void NotifyLogout(CGameRoleObj& stRoleObj);

	//�����ӵ�֪ͨ
	static void NotifyShootBullet(CGameRoleObj& stRoleObj, int iGunID, int iCost);

	//����ʱ�����֪ͨ
	static void NotifyOnlineTime(CGameRoleObj& stRoleObj, int iTimeNow, int iAddOnlineTime);

	//��ʱ��֪ͨ
    static void NotifyTick();
};
