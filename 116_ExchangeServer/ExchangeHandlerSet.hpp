#pragma once

#include "MsgHandlerSet.hpp"
#include "DBClientWrapper.hpp"
#include "OnlineExchangeHandler.hpp"
#include "GetCardNoHandler.hpp"

using namespace ServerLib;

// ExchangeӦ���е���Ϣ�����߹�����
class CExchangeHandlerSet : public CMsgHandlerSet
{
public:
	CExchangeHandlerSet();

	virtual ~CExchangeHandlerSet();

	//��ʼ����Ϣ���������
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:

	//���߶һ���Ϣ
	COnlineExchangeHandler m_stOnlineExchangeHandler;

	//��ȡ������Ϣ
	CGetCardNoHandler m_stGetCardNoHandler;

private:

	// ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
	DBClientWrapper m_oDBClient;

	//DB����������
	char* m_pQueryBuff;

	//������̣߳�Ϊ�߳�index
	int m_iThreadIndex;
};
