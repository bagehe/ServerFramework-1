#pragma once

#include "GameProtocol.hpp"
#include "ProtocolEngine.hpp"

class CRoleDBProtocolEngine : public CProtocolEngine
{
public:

	//��Ϣ����,���2����������Ҫ����
	virtual int Decode(const unsigned char* pszCodeBuff, int iCodeLen, GameProtocolMsg& stMsg, int iThreadIndex = -1,
		const TNetHead_V2* pNetHead = NULL, EGameServerID enMsgPeer = GAME_SERVER_UNDEFINE);

	//��Ϣ����,���2����������Ҫ����
	virtual int Encode(const GameProtocolMsg& stMsg, unsigned char* pszCodeBuff, int iBuffLen, int& iCodeLen, int iThreadIndex = -1,
		const TNetHead_V2* pNetHead = NULL, EGameServerID enMsgPeer = GAME_SERVER_UNDEFINE);
};
