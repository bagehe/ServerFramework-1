#pragma once

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMMailCommand : public IGMBaseCommand
{
public:
	CGMMailCommand();
	~CGMMailCommand() { };

public:
	//ִ����ӦGM����Ĺ���
	virtual int Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp);
};
