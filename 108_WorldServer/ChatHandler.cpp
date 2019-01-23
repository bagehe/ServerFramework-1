
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"
#include "ModuleHelper.hpp"

#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"

#include "ChatHandler.hpp"

using namespace ServerLib;

CChatHandler::~CChatHandler()
{

}

int CChatHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_CHAT_NOTIFY:
	{
		OnRequestChatNotify();
	}
	break;

	case MSGID_ZONE_HORSELAMP_NOTIFY:
	{
		//�������Ϣ
		OnRequestHorseLampNotify();
	}
	break;

	default:
	{
	}
	break;
	}

	return 0;
}

int CChatHandler::OnRequestChatNotify()
{
	const World_Chat_Notify& rstNotify = m_pMsg->stbody().m_stworld_chat_notify();

	//�㲥��ZoneServer
	CWorldMsgHelper::SendWorldMsgToAllZone(*m_pMsg, rstNotify.izoneid());

	return T_SERVER_SUCCESS;
}

//�������Ϣ
int CChatHandler::OnRequestHorseLampNotify()
{
	const Zone_HorseLamp_Notify& rstNotify = m_pMsg->stbody().m_stzone_horselamp_notify();

	//ֱ��ת����ZoneServer
	CWorldMsgHelper::SendWorldMsgToAllZone(*m_pMsg, rstNotify.izoneid());

	return T_SERVER_SUCCESS;
}
