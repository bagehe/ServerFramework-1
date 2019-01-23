#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CRankInfoHandler : public IHandler
{
public:
	virtual ~CRankInfoHandler();

	virtual int OnClientMsg();

private:

	//�����ȡ���а���Ϣ
	int OnRequestGetRankInfo();

	//��ȡWorld���а�ķ���
	int OnResponseGetWorldRank();
};