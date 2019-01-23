
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneOssLog.hpp"

#include "MailUtility.h"

using namespace ServerLib;

//���͵����ʼ�
int CMailUtility::SendGiftMail(CGameRoleObj& stRoleObj, unsigned uiToUin, int iItemID, int iItemNum)
{
	//��ȡ�ʼ�����
	const MailConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetMailConfig(MAIL_SEND_ITEMGIFT);
	if (!pstConfig)
	{
		LOGERROR("Failed to send item, invalid mail config %d\n", MAIL_SEND_ITEMGIFT);
		return T_ZONE_INVALID_CFG;
	}

	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(uiToUin);
	stReq.set_uifromuin(stRoleObj.GetUin());

	//��װ�ʼ�����
	OneMailInfo* pstMailInfo = stReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());
	pstMailInfo->set_imailid(MAIL_SEND_ITEMGIFT);
	//pstMailInfo->set_strtitle(pstConfig->strTitle);
	
	//����1����������
	pstMailInfo->add_strparams(stRoleObj.GetNickName());

	//����2����������ID
	char szParam[32] = { 0 };
	SAFE_SPRINTF(szParam, sizeof(szParam)-1, "%u", stRoleObj.GetUin());
	pstMailInfo->add_strparams(szParam);

	//���Ӹ���
	MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
	pstAppendix->set_itype(REWARD_TYPE_ITEM);
	pstAppendix->set_iid(iItemID);
	pstAppendix->set_inum(iItemNum);

	//�����ʼ�
	return SendPersonalMail(stReq);
}

//�������ʼ�
int CMailUtility::SendRepFullMail(CGameRoleObj& stRoleObj, int iItemID, int iItemNum)
{
	//��ȡ�ʼ�����
	const MailConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetMailConfig(MAIL_SEND_REPFULL);
	if (!pstConfig)
	{
		LOGERROR("Failed to send item, invalid mail config %d\n", MAIL_SEND_REPFULL);
		return T_ZONE_INVALID_CFG;
	}

	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(stRoleObj.GetUin());
	stReq.set_uifromuin(stRoleObj.GetUin());

	//��װ�ʼ�����
	OneMailInfo* pstMailInfo = stReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());
	pstMailInfo->set_imailid(MAIL_SEND_REPFULL);
	//pstMailInfo->set_strtitle(pstConfig->strTitle);

	//���Ӹ���
	MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
	pstAppendix->set_itype(REWARD_TYPE_ITEM);
	pstAppendix->set_iid(iItemID);
	pstAppendix->set_inum(iItemNum);

	//�����ʼ�
	return SendPersonalMail(stReq);
}

//��������ʼ�
int CMailUtility::SendVIPGiftMail(CGameRoleObj& stRoleObj, const RewardConfig& stReward)
{
	//��ȡ�ʼ�����
	const MailConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetMailConfig(MAIL_SEND_VIPGIFT);
	if (!pstConfig)
	{
		LOGERROR("Failed to send item, invalid mail config %d\n", MAIL_SEND_VIPGIFT);
		return T_ZONE_INVALID_CFG;
	}

	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(stRoleObj.GetUin());
	stReq.set_uifromuin(stRoleObj.GetUin());

	//��װ�ʼ�����
	OneMailInfo* pstMailInfo = stReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());
	pstMailInfo->set_imailid(MAIL_SEND_VIPGIFT);
	//pstMailInfo->set_strtitle(pstConfig->strTitle);

	//����1 ��ҹ���ȼ�
	char szParam[32] = { 0 };
	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%d", (stRoleObj.GetVIPLevel()-1));
	pstMailInfo->add_strparams(szParam);

	//���Ӹ���
	MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
	pstAppendix->set_itype(stReward.iType);
	pstAppendix->set_iid(stReward.iRewardID);
	pstAppendix->set_inum(stReward.iRewardNum);

	//�����ʼ�
	return SendPersonalMail(stReq);
}

//�¿�����ʼ�
int CMailUtility::SendMonthCardMail(CGameRoleObj& stRoleObj, int iMonthBoxID, int iBoxNum)
{
	//��ȡ�ʼ�����
	const MailConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetMailConfig(MAIL_SEND_MONTHCARD);
	if (!pstConfig)
	{
		LOGERROR("Failed to send item, invalid mail config %d\n", MAIL_SEND_MONTHCARD);
		return T_ZONE_INVALID_CFG;
	}

	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(stRoleObj.GetUin());
	stReq.set_uifromuin(stRoleObj.GetUin());

	//��װ�ʼ�����
	OneMailInfo* pstMailInfo = stReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());
	pstMailInfo->set_imailid(MAIL_SEND_MONTHCARD);
	//pstMailInfo->set_strtitle(pstConfig->strTitle);

	//���Ӹ���
	MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
	pstAppendix->set_itype(REWARD_TYPE_ITEM);
	pstAppendix->set_iid(iMonthBoxID);
	pstAppendix->set_inum(iBoxNum);

	//�����ʼ�,������֪ͨ
	return SendPersonalMail(stReq, false);
}

//�һ������ʼ�
int CMailUtility::SendCardNoMail(CGameRoleObj& stRoleObj, int iExchangeID, const std::string& strCardNo, const std::string& strCardPwd)
{
	//��ȡ�ʼ�����
	const MailConfig* pstConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetMailConfig(MAIL_SEND_CARDNO);
	if (!pstConfig)
	{
		LOGERROR("Failed to send card number mail, invalid mail config %d\n", MAIL_SEND_CARDNO);
		return T_ZONE_INVALID_CFG;
	}

	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(stRoleObj.GetUin());
	stReq.set_uifromuin(stRoleObj.GetUin());

	//��װ�ʼ�����
	OneMailInfo* pstMailInfo = stReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());
	pstMailInfo->set_imailid(MAIL_SEND_CARDNO);
	//pstMailInfo->set_strtitle(pstConfig->strTitle);

	//����1 ��Ҷһ�ID
	char szParam[32] = { 0 };
	SAFE_SPRINTF(szParam, sizeof(szParam) - 1, "%d", iExchangeID);
	pstMailInfo->add_strparams(szParam);

	//����2�ǿ��ܿ���
	pstMailInfo->add_strparams(strCardNo);
	
	//����3�ǿ�������
	pstMailInfo->add_strparams(strCardPwd);

	//�����ʼ�,����֪ͨ
	return SendPersonalMail(stReq);
}

//ϵͳ�ʼ�
int CMailUtility::SendSystemMail(CGameRoleObj& stRoleObj, const World_GetSystemMail_Response& stResp)
{
	World_SendMail_Request stReq;

	//��װ���ͽ���
	stReq.set_uitouin(stRoleObj.GetUin());
	stReq.set_uifromuin(0);

	int iRet = T_SERVER_SUCCESS;
	for (int i = 0; i < stResp.stmails_size(); ++i)
	{
		//��װ�ʼ�����
		stReq.mutable_stmailinfo()->CopyFrom(stResp.stmails(i));

		//�����ʼ�
		iRet = SendPersonalMail(stReq);
		if (iRet)
		{
			LOGERROR("Failed to send system mail, ret %d, uin %u, mail id %d\n", iRet, stRoleObj.GetUin(), stResp.stmails(i).imailid());
			return iRet;
		}
	}

	stRoleObj.GetMailManager().SetRoleSystemMailUniqID(stResp.usystemuniqid());

	return T_SERVER_SUCCESS;
}

//�����ʼ�
int CMailUtility::SendPersonalMail(const World_SendMail_Request& stReq, bool bSendNotify)
{
	static GameProtocolMsg stMsg;

	//�Ƿ��ڱ�Zone
	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stReq.uitouin());
	if (pstRoleObj)
	{
		//�ڱ�Zone
		pstRoleObj->GetMailManager().AddNewMail(stReq.stmailinfo(), bSendNotify);
	}
	else
	{
		//���ڱ�Zone,ת����World
		CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_SENDMAIL_REQUEST);
		
		World_SendMail_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_sendmail_request();
		pstReq->CopyFrom(stReq);

		return CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
	}

	return T_SERVER_SUCCESS;
}

//��ȡϵͳ�ʼ�
void CMailUtility::GetSystemMail(CGameRoleObj& stRoleObj, int iReason, unsigned uRoleSystemUniqID)
{
	static GameProtocolMsg stSystemMailMsg;

	//��ȡϵͳ�ʼ�
	CZoneMsgHelper::GenerateMsgHead(stSystemMailMsg, MSGID_WORLD_GETSYSTEMMAIL_REQUEST);

	World_GetSystemMail_Request* pstReq = stSystemMailMsg.mutable_stbody()->mutable_m_stworld_getsystemmail_request();
	pstReq->set_uin(stRoleObj.GetUin());
	pstReq->set_iviplevel(stRoleObj.GetVIPLevel());
	pstReq->set_strchannel(stRoleObj.GetChannel());
	pstReq->set_urolesystemuniqid(uRoleSystemUniqID);
	pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());
	pstReq->set_ireason(iReason);

	CZoneMsgHelper::SendZoneMsgToWorld(stSystemMailMsg);

	return;
}
