
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "MailUtility.h"

#include "MailHandler.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CMailHandler::~CMailHandler()
{

}

int CMailHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_MAILOPERA_REQUEST:
	{
		//�����ʼ�
		OnRequestOperaMail();
	}
	break;

	case MSGID_WORLD_SENDMAIL_REQUEST:
	{
		//World�����ʼ�����
		OnRequestSendMail();
	}
	break;

	case MSGID_WORLD_SENDMAIL_RESPONSE:
	{
		//�����ʼ��ķ���
		OnResponseSendMail();
	}
	break;

	case MSGID_WORLD_SYSTEMMAILID_NOTIFY:
	{
		//Worldϵͳ�ʼ����µ�֪ͨ
		OnNotifySystemMailUniqID();
	}
	break;

	case MSGID_WORLD_GETSYSTEMMAIL_RESPONSE:
	{
		//��ȡϵͳ�ʼ�����
		OnResponseGetSystemMail();
	}
	break;

	default:
		break;
	}

	return 0;
}

//�����ʼ�
int CMailHandler::OnRequestOperaMail()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_MAILOPERA_RESPONSE);
	Zone_MailOpera_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_mailopera_response();

	//��ȡ����
	const Zone_MailOpera_Request& stRequest = m_pRequestMsg->stbody().m_stzone_mailopera_request();
	pstResp->set_ioperatype(stRequest.ioperatype());
	pstResp->set_uuniqid(stRequest.uuniqid());

	//�����ʼ�
	iRet = m_pRoleObj->GetMailManager().OperateMail(stRequest.ioperatype(), stRequest.uuniqid());
	if (iRet)
	{
		LOGERROR("Failed to opera mail, ret %d, type %d, mail uniqid %u, uin %u\n", iRet, stRequest.ioperatype(), stRequest.uuniqid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//���ͷ���
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//�����ʼ�����
int CMailHandler::OnRequestSendMail()
{
	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_SENDMAIL_RESPONSE);
	World_SendMail_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stworld_sendmail_response();

	//��ȡ����
	const World_SendMail_Request& stReq = m_pRequestMsg->stbody().m_stworld_sendmail_request();
	pstResp->set_uifromuin(stReq.uifromuin());
	pstResp->set_uitouin(stReq.uitouin());
	pstResp->mutable_stmailinfo()->CopyFrom(stReq.stmailinfo());
	pstResp->set_itrytimes(stReq.itrytimes()+1);

	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stReq.uitouin());
	if (!pstRoleObj)
	{
		LOGERROR("Failed to send mail to role, to uin %u, mail id %d\n", stReq.uitouin(), stReq.stmailinfo().imailid());

		pstResp->set_iresult(T_ZONE_GAMEROLE_NOT_EXIST);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return -2;
	}

	int iRet = CMailUtility::SendPersonalMail(stReq);
	if (iRet)
	{
		LOGERROR("Failed to send person mail, ret %d, to uin %u, mail id %d\n", iRet, stReq.uitouin(), stReq.stmailinfo().imailid());

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return -3;
	}

	//���ͳɹ�
	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	return T_SERVER_SUCCESS;
}

//�����ʼ��ķ���
int CMailHandler::OnResponseSendMail()
{
	//�����ʼ��ķ���
	const World_SendMail_Response& stResp = m_pRequestMsg->stbody().m_stworld_sendmail_response();
	if (stResp.iresult())
	{
		const OneMailInfo& stMailInfo = stResp.stmailinfo();
		if (stMailInfo.imailid() == MAIL_SEND_ITEMGIFT && stMailInfo.stappendixs_size()>0)
		{
			//����ʧ�ܣ��������߸����
			CRepThingsUtility::OnSendGiftFailed(stResp.uifromuin(), stResp.uitouin(), stMailInfo.stappendixs(0).iid(), stMailInfo.stappendixs(0).inum(), stResp.iresult());

			return T_SERVER_SUCCESS;
		}

		//��ӡ������־
		LOGERROR("Failed to send mail, from uin %u, to uin %u, mail id %d, ret %d\n", stResp.uifromuin(), stResp.uitouin(), stMailInfo.imailid(), stResp.iresult());
	}
	else if (stResp.stmailinfo().imailid() == MAIL_SEND_ITEMGIFT)
	{
		//�ʼ����ͳɹ��� �ж��Ƿ��� �����ʼ������������ʼ�����ӡ��Ӫ��־
		const OneMailInfo& stMailInfo = stResp.stmailinfo();
		if (stMailInfo.stappendixs_size()>0)
		{
			for (int i = 0; i < stMailInfo.stappendixs_size(); ++i)
			{
				MailAppendix stMailAppendix = stMailInfo.stappendixs(i);
				CZoneOssLog::TracePresent(stResp.uifromuin(), stResp.uitouin(), stMailAppendix.iid(), stMailAppendix.inum());
			}
		}
	}

	return T_SERVER_SUCCESS;
}

//Worldϵͳ�ʼ����µ�֪ͨ
int CMailHandler::OnNotifySystemMailUniqID()
{
	const World_SystemMailID_Notify& stNotify = m_pRequestMsg->stbody().m_stworld_systemmailid_notify();
	CMailManager::SetServerMailUniqID(stNotify.usystemuniqid());

	return T_SERVER_SUCCESS;
}

//��ȡϵͳ�ʼ�����
int CMailHandler::OnResponseGetSystemMail()
{
	//��Response����Ҫ����result
	const World_GetSystemMail_Response& stResp = m_pRequestMsg->stbody().m_stworld_getsystemmail_response();

	//���÷�������ǰΨһID
	CMailManager::SetServerMailUniqID(stResp.usystemuniqid());

	//��ȡ���
	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stResp.uin());
	if (!pstRoleObj)
	{
		return -1;
	}

	int iRet = CMailUtility::SendSystemMail(*pstRoleObj, stResp);
	if (iRet)
	{
		LOGERROR("Failed to send system mail, uin %u, ret %d\n", stResp.uin(), iRet);
		return -2;
	}

	return 0;
}
