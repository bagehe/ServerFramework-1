#pragma once

#include <string>

#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "CommDefine.h"
#include "Kernel/Handler.hpp"
#include "GMHorseLampCommand.hpp"
#include "GMReposityCommand.hpp"
#include "GMResourceCommand.hpp"
#include "GMMailCommand.hpp"

class CGameRoleObj;
class CGameMasterHandler: public IHandler
{
public:
    virtual ~CGameMasterHandler(void);
	CGameMasterHandler();

    int OnClientMsg();

private:

	//����ԱGM����
	int OnRequestGM();

	//Worldת��GM
	int OnRequestWorldGM();

	//Worldת��GM����
	int OnResponseWorldGM();

    //����Ƿ���GM�û�
    int CheckIsGMUser();

	//ע��GMCommand
	void RegisterGMCommand();

private:

	//GM Command
	IGMBaseCommand* m_apGMCommand[GM_OPERA_MAX];

	//GM Handler
	CGMHorseLampCommand m_stHorseLampHandler;
	CGMReposityCommand m_stReposityHandler;
	CGMResourceCommand m_stResouceHandler;
	CGMMailCommand m_stMailHandler;
};
