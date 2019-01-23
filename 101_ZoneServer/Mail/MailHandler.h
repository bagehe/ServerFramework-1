#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CMailHandler : public IHandler
{
public:
	virtual ~CMailHandler();

	virtual int OnClientMsg();

private:

	//��Ҳ����ʼ�
	int OnRequestOperaMail();

	//�����ʼ�����
	int OnRequestSendMail();

	//�����ʼ��ķ���
	int OnResponseSendMail();

	//Worldϵͳ�ʼ����µ�֪ͨ
	int OnNotifySystemMailUniqID();

	//��ȡϵͳ�ʼ�����
	int OnResponseGetSystemMail();
};
