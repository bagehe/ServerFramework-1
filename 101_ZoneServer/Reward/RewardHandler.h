#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CRewardHandler : public IHandler
{
public:
	virtual ~CRewardHandler();

	virtual int OnClientMsg();

private:

	//�����ȡ��¼����
	int OnRequestGetLoginReward();
};