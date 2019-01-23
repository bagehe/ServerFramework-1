#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CExchangeWorldHandler : public IHandler
{
public:
	virtual ~CExchangeWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//��ȡ�����һ���Ϣ
	int OnRequestGetExchange();

	//�޸������һ���Ϣ
	int OnRequestUpdateExchange();

	//���߶һ�����
	int OnRequestOnlineExchange();

	//���߶һ�����
	int OnResponseOnlineExchange();

	//���Ӷһ���¼����
	int OnRequestAddExchangeRec();

	//��ȡ�һ���¼����
	int OnRequestGetExchangeRec();

private:
	GameProtocolMsg* m_pRequestMsg;
};
