
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "HashUtility.hpp"

#include "AppThreadManager.hpp"

using namespace ServerLib;

CAppThreadManager* CAppThreadManager::Instance()
{
	static CAppThreadManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CAppThreadManager;
	}

	return pInstance;
}

CAppThreadManager::CAppThreadManager()
{
	m_iThreadNum = 0;
}

CAppThreadManager::~CAppThreadManager()
{
	m_iThreadNum = 0;
}

//��ʼ���߳�,pstProtocolEngine��pstHandlerSet�̶߳������ܶ���̹߳���
int CAppThreadManager::InitOneThread(CProtocolEngine* pstProtocolEngine, CMsgHandlerSet* pstHandlerSet)
{
	if (m_iThreadNum < 0 || m_iThreadNum >= MAX_APP_THREAD_NUM)
	{
		return -1;
	}

	//��ʼ���߳�
	int iRet = m_aThreads[m_iThreadNum].Initialize(m_iThreadNum, pstProtocolEngine, pstHandlerSet);
	if (iRet)
	{
		return -2;
	}

	++m_iThreadNum;

	return 0;
}

CAppThread* CAppThreadManager::GetThread(int iThreadIdx)
{
	if (iThreadIdx < 0 || iThreadIdx >= m_iThreadNum)
	{
		return NULL;
	}

	return &m_aThreads[iThreadIdx];
}

//��ȡʵ���߳�����
int CAppThreadManager::GetThreadNum()
{
	return m_iThreadNum;
}

int CAppThreadManager::PushCode(unsigned int uHashValue, const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg || m_iThreadNum<=0)
	{
		return -1;
	}

	int iThreadIdx = uHashValue % m_iThreadNum;

	return m_aThreads[iThreadIdx].PushCode(pMsg, iCodeLength);
}

//��ѯд�߳��������
int CAppThreadManager::PollingPushCode(const unsigned char* pMsg, int iCodeLength)
{
	if (!pMsg || m_iThreadNum <= 0)
	{
		return -1;
	}

	static unsigned uPollingIndex = 0;
	int iThreadIndex = uPollingIndex++%m_iThreadNum;

	return m_aThreads[iThreadIndex].PushCode(pMsg, iCodeLength);
}

int CAppThreadManager::PopCode(const int iThreadIdx, unsigned char* pMsg, int iMaxLength, int& riLength)
{
	if (!pMsg)
	{
		return -1;
	}

	if (iThreadIdx < 0 || iThreadIdx >= m_iThreadNum)
	{
		return -2;
	}

	return m_aThreads[iThreadIdx].PopCode(pMsg, iMaxLength, riLength);
}


