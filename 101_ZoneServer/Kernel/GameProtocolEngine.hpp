#pragma once

#include "UnixTime.hpp"
#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "Kernel/GameRole.hpp"
#include "MsgStatistic.hpp"

const int MAX_MSGBUFFER_SIZE = 2048000;

const int MAX_SOCKETFD_NUMBER = 200;

const unsigned int SEND_MULTI_SESSION = 0xFFFFFFFF;
const unsigned int SEND_ALL_SESSION = 0xFFFFFFFF;
const unsigned int SEND_ALL_SESSION_BUTONE = 0xFFFFFFFE;

class CAppLoop;
class CZoneSceneObj;

class CGameProtocolEngine
{
public:
	int Initialize(bool bResumeMode);

public:
    // �����ݽ��, ������Handler����
    int OnRecvCode(char* pszMsgBuffer, int iMsgLength, EGameServerID enMsgPeer, int iInstanceID = 0);

    // �����ݴ��, �����͵��Զ�
	int SendZoneMsg(GameProtocolMsg& rstZoneMsg, const TNetHead_V2& rstNetHead, EGameServerID enMsgPeer);

	// ���������ݷ��͵����lotus
	int SendZoneMsgListToMultiLotus(GameProtocolMsg& rstZoneMsg, 
			unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
			unsigned int uiSocketNumber);

	// ���������ݷ��͵�Lotus
	int SendZoneMsgListToLotus(GameProtocolMsg& rstZoneMsg, 
		unsigned int auiSocketFD[MAX_SOCKETFD_NUMBER], 
		unsigned int uiSocketNumber, 
		int iInstanceID, int iStaticSocketNumber);

private:
    // ͨ��protobuf�����յ�������
	int DecodeClient(const char* pszMsgBuffer, const int iMsgLength, GameProtocolMsg& rstMsg, EGameServerID enMsgPeer);

private:
	char m_szGameMsg[MAX_MSGBUFFER_SIZE];

	GameProtocolMsg m_stMsg;
};
