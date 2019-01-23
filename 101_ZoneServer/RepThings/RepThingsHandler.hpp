#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"
#include "RepThingsManager.hpp"

class CGameRoleObj;

class CRepThingsHandler : public IHandler
{
public:
	virtual ~CRepThingsHandler();

	virtual int OnClientMsg();

private:

    //��ҶԱ�����Ʒ�Ĳ���
    int OnRequestOperaRepItem();

	//��ҵ���ʹ�÷���
	int OnResponseUseRepItem();
};
