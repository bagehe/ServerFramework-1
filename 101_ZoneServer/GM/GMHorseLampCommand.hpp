#pragma once

#include "GMBaseCommand.hpp"

class CGameRoleObj;
class CGMHorseLampCommand : public IGMBaseCommand
{
public:
	CGMHorseLampCommand();
	~CGMHorseLampCommand() { };

public:
	//ִ����Ӧ��GM����Ĺ���
	virtual int Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp);

	//����GM�����
	void SendGMHorseLamp(int iLampID, const std::string& strHorseLamp);
};
