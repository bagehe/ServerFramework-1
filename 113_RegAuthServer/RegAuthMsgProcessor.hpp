#pragma once

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "RegAuthPublic.hpp"
#include "RegAuthProtocolEngine.hpp"
#include "RegAuthHandlerSet.hpp"

//���߳���Ϣ����
class CRegAuthMsgProcessor
{
public:
	static CRegAuthMsgProcessor* Instance();
	~CRegAuthMsgProcessor();

	//��ʼ��
	int Initialize();

	//���մ�����Ϣ
	void ProcessRecvMsg(int& iNewMsgCount);

	//���͵�Gateway
	int SendRegAuthToLotus(TNetHead_V2* pNetHead, const GameProtocolMsg& stMsg);
	int SendRegAuthToLotus(unsigned uiSessionFd, const GameProtocolMsg& stMsg);

	//���͵�RegAuthDB
	int SendRegAuthToDB(const GameProtocolMsg& stMsg);

	//���͵�Platform
	int SendRegAuthToPlatform(const GameProtocolMsg& stMsg);

private:
	CRegAuthMsgProcessor();

	//��������Ϣ
	int ProcessOneMsg(int iMsgPeer, unsigned uiInstanceID = 1);

private:

	//ʵ����Ϣ����
	int m_iCodeLen;

	//���ͻ�����
	unsigned char m_szCodeBuff[MAX_CODE_LEN];

	//Э���������
	CRegAuthProtocolEngine m_stProtocolEngine;

	//Э��Handler
	CRegAuthHandlerSet m_stHandlerSet;
};
