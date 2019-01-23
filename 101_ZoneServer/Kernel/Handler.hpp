
#ifndef __I_HANDLER_HPP__
#define __I_HANDLER_HPP__

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "CommDefine.h"

class CGameRoleObj;
class CGameSessionObj;
class IHandler
{
public:
	IHandler();
	virtual ~IHandler();

	// �����������
	void SetClientMsg(const GameProtocolMsg* pRequestMsg, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer = GAME_SERVER_LOTUS);

	void SetMsgID(int iMsgID)
	{
		m_iMsgID = iMsgID;
	}

	int GetMsgID()const
	{
		return m_iMsgID;
	}

public:
    // ��Ϣ�������
	virtual int OnClientMsg() = 0;

protected:
    // ��鷢����
    virtual int SecurityCheck();

protected:
	// ��Ϣ����
	GameProtocolMsg* m_pRequestMsg;
	TNetHead_V2* m_pNetHead;
	CGameSessionObj* m_pSession;
	CGameRoleObj* m_pRoleObj;
	EGameServerID m_enMsgPeer;	

	int m_iMsgID;
};



#endif


