#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"
#include "ExchangeHandler.h"

class CGameRoleObj;
class CExchangeHandler : public IHandler
{
public:
	virtual ~CExchangeHandler();

	virtual int OnClientMsg();

private:

	//����޸���Ϣ������
	int OnRequestChangeUserInfo();

	//��Ҷһ�������
	int OnRequestExchange();

	//�����ȡ������������
	int OnRequestGetLimitInfo();

	//World��ȡ�����ķ���
	int OnResponseGetLimitInfo();

	//World���������ķ���
	int OnResponseUpdateLimit();

	//World��ȡ������Ϣ�ķ���
	int OnResponseGetCardNo();

	//�����ȡ�һ���¼������
	int OnRequestGetExchangeRec();

	//World��ȡ�һ���¼�ķ���
	int OnResponseGetExchangeRec();
};
