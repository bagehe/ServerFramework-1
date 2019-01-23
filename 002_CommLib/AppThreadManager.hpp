#pragma once

#include "CommDefine.h"
#include "AppThread.hpp"

class CAppThreadManager
{
public:
	static CAppThreadManager* Instance();
	~CAppThreadManager();

	//��ʼ���߳�,pstProtocolEngine��pstHandlerSet�̶߳������ܶ���̹߳���
	int InitOneThread(CProtocolEngine* pstProtocolEngine, CMsgHandlerSet* pstHandlerSet);

	//д�߳��������
	int PushCode(unsigned int uHashValue, const unsigned char* pMsg, int iCodeLength);

	//��ѯд�߳��������
	int PollingPushCode(const unsigned char* pMsg, int iCodeLength);

	//���߳��������
	int PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength);

	CAppThread* GetThread(int iThreadIdx);

	//��ȡʵ���߳�����
	int GetThreadNum();

private:
	CAppThreadManager();

private:

	//ʵ���߳���
	int m_iThreadNum;

	CAppThread m_aThreads[MAX_APP_THREAD_NUM];
};
