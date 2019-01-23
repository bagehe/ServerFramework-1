#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CLasvegasWorldHandler : public IHandler
{
public:
	virtual ~CLasvegasWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//����������ע��Ϣ
	int OnRequestUpdateBetInfo();

	//�����н���Ϣ
	int OnRequestUpdatePrizeInfo();

private:
	GameProtocolMsg* m_pRequestMsg;
};
