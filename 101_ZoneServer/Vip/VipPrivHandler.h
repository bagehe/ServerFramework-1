#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class CGameRoleObj;
class CVipPrivHandler : public IHandler
{
public:
	virtual ~CVipPrivHandler();

	virtual int OnClientMsg();

private:

	//�����ȡ�ȼý������
	int OnRequestGetAlms();
};
