#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CLimitLotteryWorldHandler : public IHandler
{
public:
	virtual ~CLimitLotteryWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//��������齱
	int OnRequestLimitLottery();

	//�����ȡ���ó齱��¼
	int OnRequestPayLotteryRecord();

private:
	GameProtocolMsg* m_pRequestMsg;
};
