#pragma once

#include "Handler.hpp"
#include "AppDefW.hpp"

class CRechargeWorldHandler : public IHandler
{
public:
	virtual ~CRechargeWorldHandler();

public:

	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

	//ϵͳ��ֵ������
	int OnRequestUserRecharge();

	//ϵͳ��ֵ�ķ���
	int OnResponseUserRecharge();

	//�����ȡ������
	int OnRequestGetPayOrder();

	//��ȡ�˺���Ϣ������
	int OnRequestGetUserInfo();

	//��ȡ�˺���Ϣ�ķ���
	int OnResponseGetUserInfo();

	//��ȡ��ֵ������
	std::string GetNewOrderID(unsigned uiUin);

private:
	GameProtocolMsg* m_pRequestMsg;
};
