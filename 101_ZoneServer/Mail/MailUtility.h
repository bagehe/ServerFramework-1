#pragma once

#include "GameProtocol.hpp"
#include "BaseConfigManager.hpp"
#include "Kernel/GameRole.hpp"

//�ʼ�������
class CMailUtility
{
public:

	//���͵����ʼ�
	static int SendGiftMail(CGameRoleObj& stRoleObj, unsigned uiToUin, int iItemID, int iItemNum);

	//�������ʼ�
	static int SendRepFullMail(CGameRoleObj& stRoleObj, int iItemID, int iItemNum);

	//��������ʼ�
	static int SendVIPGiftMail(CGameRoleObj& stRoleObj, const RewardConfig& stReward);

	//�¿�����ʼ�
	static int SendMonthCardMail(CGameRoleObj& stRoleObj, int iMonthBoxID, int iBoxNum);

	//�һ������ʼ�
	static int SendCardNoMail(CGameRoleObj& stRoleObj, int iExchangeID, const std::string& strCardNo, const std::string& strCardPwd);

	//ϵͳ�ʼ�
	static int SendSystemMail(CGameRoleObj& stRoleObj, const World_GetSystemMail_Response& stResp);

	//�����ʼ�
	static int SendPersonalMail(const World_SendMail_Request& stReq, bool bSendNotify=true);

	//��ȡϵͳ�ʼ�
	static void GetSystemMail(CGameRoleObj& stRoleObj, int iReason, unsigned uRoleSystemUniqID);
};
