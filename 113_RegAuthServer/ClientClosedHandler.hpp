#pragma once

#include "MsgHandler.hpp"

// ��������LotusServer��MSGID_LOGOUTSERVER_REQUEST��Ϣ
class CClientClosedHandler : public CMsgHandler
{
public:
    CClientClosedHandler();

	//�ڶ�����������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);
};
