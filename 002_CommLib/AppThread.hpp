#pragma once

#include <pthread.h>

#include "GameProtocol.hpp"
#include "CodeQueue.hpp"
#include "ProtocolEngine.hpp"
#include "MsgHandlerSet.hpp"

using namespace ServerLib;

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_THREAD_MAX_SLEEP_USEC = 5 * 1000;    //�߳�sleepʱ��
#else
const int APP_THREAD_MAX_SLEEP_USEC = 5 * 1000;	//�߳�sleepʱ��
#endif

static const int MAX_MSGBUFFER_SIZE = 204800;		//����������

//App�߳�
class CAppThread
{
public:
    int Initialize(int iThreadIdx, CProtocolEngine* pstProtocolEngine, CMsgHandlerSet* pstHandlerSet);

	int Run();

	int GetIdx() const { return m_iThreadIdx; }

private:

	int InitCodeQueue(const int iThreadIdx);

	//������Ϣ
	int ReceiveMsg(int& riCodeLength);
	
	//������Ϣ
	int ProcessMsg(SHandleResult& stMsgHandleResult);

	//ѹ�뷵����Ϣ
	int EncodeAndSendMsg(SHandleResult& stMsgHandleResult);

public:
	//ѹ�뷵��
	int PushCode(const unsigned char* pMsg, int iCodeLength);

	//������Ϣ
	int PopCode(unsigned char* pMsg, int iMaxLength, int& riLength);

private:
	int CreateThread();

private:
    CProtocolEngine* m_pstProtocolEngine;
    CMsgHandlerSet* m_pstHandlerSet;

	CCodeQueue* m_pInputQueue;	//������Ϣ���У������ڲ��������������̷ַ߳�
	CCodeQueue* m_pOutputQueue; //�����Ϣ����
	int m_iThreadIdx;			//�߳�index

	pthread_t m_hTrd;
	pthread_attr_t m_stAttr;

    GameProtocolMsg m_stGameMsg;
    unsigned char m_szCodeBuf[MAX_MSGBUFFER_SIZE];
};
