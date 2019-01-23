
#include <string.h>
#include <arpa/inet.h>
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ServerBusManager.h"
#include "AppThreadManager.hpp"
#include "ExchangeProtocolEngine.hpp"

#include "ExchangeMsgProcessor.hpp"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CExchangeMsgProcessor* CExchangeMsgProcessor::Instance()
{
	static CExchangeMsgProcessor* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CExchangeMsgProcessor;
	}

	return pInstance;
}

CExchangeMsgProcessor::CExchangeMsgProcessor()
{
	//ʵ����Ϣ����
	m_iCodeLen = 0;

	//���ͻ�����
	memset(m_szCodeBuff, 0, sizeof(m_szCodeBuff));
}

CExchangeMsgProcessor::~CExchangeMsgProcessor()
{

}

//��ʼ��
int CExchangeMsgProcessor::Initialize()
{
	return 0;
}

//���մ�����Ϣ
void CExchangeMsgProcessor::ProcessRecvMsg(int& iNewMsgCount)
{
	iNewMsgCount = 0;
	m_iCodeLen = 0;

	// ������������
	SERVERBUSID stFromBusID;
	int iRet = CServerBusManager::Instance()->RecvOneMsg((char*)m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen, stFromBusID);
	if ((iRet == 0) && (m_iCodeLen != 0))
	{
		LOGDEBUG("Receive net code OK, len: %d\n", m_iCodeLen);

		//���͵������߳�
		iRet = CAppThreadManager::Instance()->PollingPushCode(m_szCodeBuff, m_iCodeLen);
		if (iRet)
		{
			LOGERROR("Failed to send msg to work thread, ret %d\n", iRet);
		}
		else
		{
			//������Ϣ����
			++iNewMsgCount;
		}
	}

	//����ƽ̨�߳�����
	for (int i = 0; i < WORK_THREAD_NUM; ++i)
	{
		m_iCodeLen = 0;
		iRet = CAppThreadManager::Instance()->PopCode(i, m_szCodeBuff, MAX_CODE_LEN, m_iCodeLen);
		if ((iRet == 0) && (m_iCodeLen != 0))
		{
			TRACESVR("Receive thread code OK, len: %d, thread index %d\n", m_iCodeLen, i);

			//���͵�World
			SERVERBUSID stToBusID;
			stToBusID.usWorldID = 1;
			stToBusID.usServerID = GAME_SERVER_WORLD;
			stToBusID.usInstanceID = 1;

			iRet = CServerBusManager::Instance()->SendOneMsg((char*)m_szCodeBuff, m_iCodeLen, stToBusID);
			if (iRet)
			{
				//ֻ��ӡ������־��������
				LOGERROR("Failed to process thread msg, ret %d, from thread %d\n", iRet, i);
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
