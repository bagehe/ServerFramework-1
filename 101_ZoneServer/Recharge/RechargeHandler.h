#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CRechargeHandler : public IHandler
{
public:
	virtual ~CRechargeHandler();

	virtual int OnClientMsg();

private:

	//ϵͳ��ֵ����
	int OnRequestRecharge();

	//�����ȡ��ֵ��¼����
	int OnRequestGetRecord();

	//�����ȡ��ֵ�������
	int OnRequestGetPayGift();

	//��һ�ȡ����������
	int OnRequestGetPayOrder();

	//��һ�ȡ�����ŷ���
	int OnResponseGetPayOrder();

	//��ҽ�ҳ�ֵ����
	int OnRequestRechargeCoin();
};
