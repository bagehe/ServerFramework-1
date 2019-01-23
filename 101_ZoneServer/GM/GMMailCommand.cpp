
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Mail/MailUtility.h"

#include "GMMailCommand.hpp"

using namespace ServerLib;

CGMMailCommand::CGMMailCommand()
{
	m_pRoleObj = NULL;
}

//ִ����ӦGM����Ĺ���
int CGMMailCommand::Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp)
{
	return T_SERVER_SUCCESS;
}
