#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CMailWorldHandler : public IHandler
{
public:
	virtual ~CMailWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//�����ʼ�������
	int OnRequestSendMail();

	//�����ʼ��ķ���
	int OnResponseSendMail();

	//��ȡϵͳ�ʼ�����
	int OnRequestGetSystemMail();

private:
	GameProtocolMsg* m_pRequestMsg;
};
