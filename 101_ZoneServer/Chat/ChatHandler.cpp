
#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ConfigManager.hpp"

#include "Chat/ChatHandler.hpp"

static GameProtocolMsg stMsg;

CChatHandler::~CChatHandler(void)
{

}

CChatHandler::CChatHandler()
{

}

int CChatHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_CHAT_REQUEST:
	{
		OnRequestChat();
	}
	break;

	case MSGID_WORLD_CHAT_NOTIFY:
	{
		OnNotifyChat();
	}
	break;

	case MSGID_ZONE_HORSELAMP_NOTIFY:
	{
		OnNotifyHorseLamp();
	}
	break;

	default:
	{
		LOGERROR("Failed to handler request msg, invalid msgid: %u\n", m_pRequestMsg->sthead().uimsgid());
		return -1;
	}
	break;
	}

	return 0;
}

int CChatHandler::OnRequestChat()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);

		SendFailedResponse(MSGID_ZONE_CHAT_RESPONSE, T_ZONE_SECURITY_CHECK_FAILED, *m_pNetHead);

		return -1;
	}

	unsigned int uiUin = m_pRoleObj->GetUin();
	CGameRoleObj* pstRoleObj = GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
	ASSERT_AND_LOG_RTN_INT(pstRoleObj);

	//�����������Ĵ���
	const Zone_Chat_Request& rstRequest = m_pRequestMsg->stbody().m_stzone_chat_request();
	SendChatMsg(*pstRoleObj, rstRequest.ichannel(), rstRequest.strmessage().c_str());

	//����������Ϣ�ɹ�������
	SendSuccessfulResponse();

	return T_SERVER_SUCCESS;
}

int CChatHandler::OnNotifyChat()
{
	//�㲥�����ߵ������û�
	CZoneMsgHelper::SendZoneMsgToZoneAll(*m_pRequestMsg);

	return T_SERVER_SUCCESS;
}

//��������Ƶ�����
int CChatHandler::OnNotifyHorseLamp()
{
	//�㲥�����ߵ������û�
	CZoneMsgHelper::SendZoneMsgToZoneAll(*m_pRequestMsg);

	return T_SERVER_SUCCESS;
}

//����������Ϣ
int CChatHandler::SendChatMsg(CGameRoleObj& stRoleObj, int iChannel, const char* pMsg)
{
	if (!pMsg)
	{
		return T_ZONE_PARA_ERROR;
	}

	static GameProtocolMsg stChatNotify;
	CZoneMsgHelper::GenerateMsgHead(stChatNotify, MSGID_WORLD_CHAT_NOTIFY);

	World_Chat_Notify* pstNotify = stChatNotify.mutable_stbody()->mutable_m_stworld_chat_notify();

	switch (iChannel)
	{
	case CHAT_CHANNEL_WORLD:
	{
		pstNotify->set_ichannel((ChatChannelType)iChannel);
		pstNotify->mutable_stroleid()->CopyFrom(stRoleObj.GetRoleID());
		pstNotify->set_sznickname(stRoleObj.GetNickName());
		pstNotify->set_izoneid(CModuleHelper::GetZoneID());
		pstNotify->set_szmessage(pMsg);

		//�ȹ㲥�����ߵ����
		CZoneMsgHelper::SendZoneMsgToZoneAll(stChatNotify);

		//ת�������������
		CZoneMsgHelper::SendZoneMsgToWorld(stChatNotify);
	}
	break;

	case CHAT_CHANNEL_SYSTEM:
	{
		//ϵͳ��Ϣ
		pstNotify->set_ichannel((ChatChannelType)iChannel);
		pstNotify->set_szmessage(pMsg);

		//���͸��ͻ���
		CZoneMsgHelper::SendZoneMsgToRole(stChatNotify, &stRoleObj);
	}
	break;

	case CHAT_CHANNEL_PRIVATE:
	{
		//���˽�ģ���ʱ������
		;
	}
	break;

	default:
	{
		LOGERROR("Failed to do chat, invalid channel %d, uin %u\n", iChannel, stRoleObj.GetUin());
		return T_ZONE_PARA_ERROR;
	}
	break;
	}

	return T_SERVER_SUCCESS;
}

//����ʧ�ܵĻظ�
int CChatHandler::SendFailedResponse(int iMsgID, int iResultID, const TNetHead_V2& rstNetHead)
{
	CZoneMsgHelper::GenerateMsgHead(stMsg, iMsgID);

	stMsg.mutable_stbody()->mutable_m_stzone_chat_response()->set_iresult(iResultID);

	CZoneMsgHelper::SendZoneMsgToClient(stMsg, rstNetHead);

	return 0;
}

//���ͳɹ��Ļظ�
int CChatHandler::SendSuccessfulResponse()
{
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_CHAT_RESPONSE);

	CGameRoleObj* pRoleObj = m_pSession->GetBindingRole();
	ASSERT_AND_LOG_RTN_INT(pRoleObj);

	stMsg.mutable_stbody()->mutable_m_stzone_chat_response()->set_iresult(T_SERVER_SUCCESS);

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pRoleObj);

	return 0;
}
