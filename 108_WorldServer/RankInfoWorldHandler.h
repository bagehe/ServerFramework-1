#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CRankInfoWorldHandler : public IHandler
{
public:
	virtual ~CRankInfoWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//�������а�
	int OnRequestUpdateRank();

	//��ȡ������Ϣ
	int OnRequestGetWorldRank();

private:
	GameProtocolMsg* m_pRequestMsg;
};