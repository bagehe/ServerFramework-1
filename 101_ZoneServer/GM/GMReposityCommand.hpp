#pragma once

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMReposityCommand : public IGMBaseCommand
{
public:
	CGMReposityCommand();
    ~CGMReposityCommand() { };

public:
    //ִ����ӦGM����Ĺ���
    virtual int Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp);
};
