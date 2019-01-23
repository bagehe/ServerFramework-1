#pragma once

#include "MsgHandlerSet.hpp"
#include "AddRegAuthHandler.hpp"
#include "DeleteRegAuthHandler.hpp"
#include "FetchRegAuthHandler.hpp"
#include "AuthAccountHandler.h"

// RegAuthDBӦ���е���Ϣ�����߹�����
class CRegAuthDBHandlerSet : public CMsgHandlerSet
{
public:
    CRegAuthDBHandlerSet();
    ~CRegAuthDBHandlerSet();

	//��ʼ����Ϣ���������
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:
	// �ü��Ϲ����������Ϣ������
	CAddRegAuthHandler m_stAddRegAuthHandler;
	CDeleteRegAuthHandler m_stDeleteRegAuthHandler;
	CFetchRegAuthHandler m_stFetchRegAuthHandler;
	CAuthAccountHandler m_stAuthAccountHandler;

private:
	// ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
	DBClientWrapper m_oDBClient;

	//DB����������
	char* m_pQueryBuff;

	int m_iThreadIdx;
};
