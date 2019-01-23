
#include <string.h>
#include <arpa/inet.h>
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ServerBusManager.h"
#include "AppThreadManager.hpp"
#include "SessionManager.hpp"
#include "RegAuthProtocolEngine.hpp"

#include "RegAuthMsgProcessor.hpp"

using namespace ServerLib;

static GameProtocolMsg stMsg;
static TNetHead_V2 stNetHead;

CRegAuthMsgProcessor* CRegAuthMsgProcessor::Instance()
{
	static CRegAuthMsgProcessor* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CRegAuthMsgProcessor;
	}

	return pInstance;
}

CRegAuthMsgProcessor::CRegAuthMsgProcessor()
{
	//ʵ����Ϣ����
	m_iCodeLen = 0;

	//���ͻ�����
	memset(m_szCodeBuff, 0, sizeof(m_szCodeBuff));
}

CRegAuthMsgProcessor::~CRegAuthMsgProcessor()
{

}

//��ʼ��
int CRegAuthMsgProcessor::Initialize()
{
	return m_stHandlerSet.Initialize();
}

//���մ�����Ϣ
void CRegAuthMsgProcessor::ProcessRecvMsg(int& iNewMsgCount)
{
	iNewMsgCount = 0;
	m_iCodeLen = 0;

	// ������������
	SERVERBUSID stFromBusID;
	int iRet = CServerBusManager::Instance()->RecvOneMsg((char*)m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen, stFromBusID);
	if ((iRet == 0) && (m_iCodeLen != 0))
	{
		LOGDEBUG("Receive net code OK, len: %d\n", m_iCodeLen);

		iRet = ProcessOneMsg(stFromBusID.usServerID, stFromBusID.usInstanceID);
		if (iRet)
		{
			//ֻ��ӡ������־��������
			LOGERROR("Failed to process net msg, ret %d, from server:instance %u:%u\n", iRet, stFromBusID.usServerID, stFromBusID.usInstanceID);
		}
		else
		{
			//������Ϣ����
			++iNewMsgCount;
		}
	}

	//����ƽ̨�߳�����
	for (int i = 0; i < PLATFORM_AUTH_THREAD_NUM; ++i)
	{
		m_iCodeLen = 0;
		iRet = CAppThreadManager::Instance()->PopCode(i, m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen);
		if ((iRet == 0) && (m_iCodeLen != 0))
		{
			TRACESVR("Receive thread code OK, len: %d, thread index %d\n", m_iCodeLen, i);

			//������Ϣ
			iRet = ProcessOneMsg(GAME_SERVER_PLATFORM, 1);
			if (iRet)
			{
				//ֻ��ӡ������־��������
				LOGERROR("Failed to process thread msg, ret %d, from server:instance %u:%u\n", iRet, GAME_SERVER_PLATFORM, 1);
			}
			else
			{
				//������Ϣ����
				++iNewMsgCount;
			}
		}
	}

	return;
}

//���͵�Gateway
int CRegAuthMsgProcessor::SendRegAuthToLotus(TNetHead_V2* pNetHead, const GameProtocolMsg& stMsg)
{
	ASSERT_AND_LOG_RTN_INT(pNetHead);

	TNetHead_V2 stTmpNetHead = *pNetHead;
	stTmpNetHead.m_uiSocketFD = htonl(ntohl(pNetHead->m_uiSocketFD) % MAX_FD_NUMBER);

	//Э�����
	int iRet = m_stProtocolEngine.Encode(stMsg, m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen, -1, &stTmpNetHead, GAME_SERVER_LOTUS);
	if (iRet)
	{
		return -1;
	}

	// ������������
	SERVERBUSID stToBusID;
	stToBusID.usServerID = GAME_SERVER_LOTUS;
	stToBusID.usInstanceID = ntohl(pNetHead->m_uiSocketFD) / MAX_FD_NUMBER;
	
	iRet = CServerBusManager::Instance()->SendOneMsg((char*)m_szCodeBuff, m_iCodeLen, stToBusID);
	if (iRet < 0)
	{
		return -2;
	}

	return 0;
}

int CRegAuthMsgProcessor::SendRegAuthToLotus(unsigned uiSessionFd, const GameProtocolMsg& stMsg)
{
	//���һ�ȡSession
	CSessionObj* pSession = CSessionManager::Instance()->GetSession(uiSessionFd);
	ASSERT_AND_LOG_RTN_INT(pSession);

	//���һ�ȡNetHead
	TNetHead_V2* pstNetHead = pSession->GetNetHead();
	ASSERT_AND_LOG_RTN_INT(pstNetHead);

	return SendRegAuthToLotus(pstNetHead, stMsg);
}

//���͵�RegAuthDB
int CRegAuthMsgProcessor::SendRegAuthToDB(const GameProtocolMsg& stMsg)
{
	//Э�����
	int iRet = m_stProtocolEngine.Encode(stMsg, m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen, -1, NULL, GAME_SERVER_REGAUTHDB);
	if (iRet)
	{
		return -1;
	}

	// ������������
	SERVERBUSID stToBusID;
	stToBusID.usServerID = GAME_SERVER_REGAUTHDB;
	stToBusID.usInstanceID = 1;

	iRet = CServerBusManager::Instance()->SendOneMsg((char*)m_szCodeBuff, m_iCodeLen, stToBusID);
	if (iRet < 0)
	{
		return -2;
	}

	return 0;
}

//���͵�Platform
int CRegAuthMsgProcessor::SendRegAuthToPlatform(const GameProtocolMsg& stMsg)
{
	//������֤�߳̽��д���

	//Э�����
	int iRet = m_stProtocolEngine.Encode(stMsg, m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen);
	if (iRet)
	{
		return -1;
	}

	iRet = CAppThreadManager::Instance()->PollingPushCode(m_szCodeBuff, m_iCodeLen);
	if (iRet)
	{
		return -2;
	}

	return 0;
}

//��������Ϣ
int CRegAuthMsgProcessor::ProcessOneMsg(int iMsgPeer, unsigned uiInstanceID)
{
	//��ʼ��
	memset(&stNetHead, 0, sizeof(TNetHead_V2));
	stMsg.Clear();

	// ������������Ϊ��������
	int iRet = m_stProtocolEngine.Decode((unsigned char*)m_szCodeBuff, m_iCodeLen, stMsg, -1, &stNetHead, (EGameServerID)iMsgPeer);
	if (iRet)
	{
		return -1;
	}

	// ����������
	CMsgHandler* pHandler = NULL;
	TNetHead_V2* pNetHead = NULL;
	if (iMsgPeer == GAME_SERVER_LOTUS)
	{
		pHandler = m_stHandlerSet.GetHandler(stMsg.sthead().uimsgid(), EKMT_CLIENT);
		unsigned  int uiSessionID = ntohl(stNetHead.m_uiSocketFD);
		uiSessionID += uiInstanceID * MAX_FD_NUMBER;
		stNetHead.m_uiSocketFD = htonl(uiSessionID);

		pNetHead = &stNetHead;
	}
	else
	{
		pHandler = m_stHandlerSet.GetHandler(stMsg.sthead().uimsgid(), EKMT_SERVER);
	}

	if (!pHandler)
	{
		TRACESVR("Failed to find a message handler, msg id: %u\n", stMsg.sthead().uimsgid());
		return -2;
	}

	pHandler->OnClientMsg(stMsg, NULL, pNetHead);

	return 0;
}
