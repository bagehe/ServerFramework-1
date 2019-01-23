#pragma once

#include "GameProtocol.hpp"
#include "Kernel/Handler.hpp"
#include "CommDefine.h"
#include "ErrorNumDef.hpp"

class CGameRoleObj;
class CNewGuideHandler : public IHandler
{
public:
	virtual ~CNewGuideHandler(void);
	CNewGuideHandler();

public:
	int OnClientMsg();

private:

	//���������������
	int OnRequestFinGuide();

	//����޸���������
	int OnRequestUpdateName();
};
