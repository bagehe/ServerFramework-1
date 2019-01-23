
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ServerBusManager.h"

#include "RechargeProxy.h"

using namespace ServerLib;

//���͵���Ӧ������
int CRechargeProxy::SendRechargeMsgToWorld(GameProtocolMsg& stMsg, int iWorldID)
{
	//2bytes(len) + msg
	char* pszRealBuff = m_szSendBuff + sizeof(unsigned short);

	//���л���Ϣ
	if (!stMsg.SerializeToArray(pszRealBuff, sizeof(m_szSendBuff) - sizeof(unsigned short)))
	{
		LOGERROR("Failed to encode proto msg!\n");
		return -1;
	}

	//��䳤��
	m_iSendLen = stMsg.ByteSize() + sizeof(unsigned short);
	m_szSendBuff[0] = m_iSendLen / 256;
	m_szSendBuff[1] = m_iSendLen % 256;

	//���͵�Bus
	SERVERBUSID stToBusID;
	stToBusID.usWorldID = iWorldID;
	stToBusID.usServerID = GAME_SERVER_WORLD;
	
	return CServerBusManager::Instance()->SendOneMsg(m_szSendBuff, m_iSendLen, stToBusID);
}
