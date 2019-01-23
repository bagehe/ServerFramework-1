#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "GameProtocol.hpp"

class CChatHandler : public IHandler
{
public:
    virtual ~CChatHandler();

public:
    virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//������Ϣ
    int OnRequestChatNotify();

	//�������Ϣ
	int OnRequestHorseLampNotify();

private:
    GameProtocolMsg* m_pMsg;
};
