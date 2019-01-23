#pragma once

//GM�������
#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"

class IGMBaseCommand
{
public:
    virtual ~IGMBaseCommand();

    //ִ��GM����
    virtual int Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp) = 0;

protected:

	//GM����Ŀ�����
	CGameRoleObj* m_pRoleObj;
};
