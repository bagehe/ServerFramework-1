#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CLotteryHandler : public IHandler
{
public:
	virtual ~CLotteryHandler();

	virtual int OnClientMsg();

private:

	//��Һ�����������
	int OnRequestLottery();

	//��������齱����
	int OnRequestLimitLottery();

	//��������齱����
	int OnResponseLimitLottery();

	//�����ȡ��ֵ�齱����
	int OnRequestPayLotteryRecord();

	//�����ȡ��ֵ�齱����
	int OnResponsePayLotteryRecord();

	//��ҽ�����ת�̳齱����
	int OnRequestEnterLasvegas();

	//��Ҵ�ת����ע������
	int OnRequestBetLasvegas();

	//�����ȡ�н���¼������
	int OnRequestGetRewardInfo();

	//World���´�ת����Ϣ
	int OnNotifyUpdateLasvegas();
};
