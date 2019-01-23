#pragma once

#include "MsgHandlerSet.hpp"
#include "RegisterAccountHandler.hpp"
#include "AuthAccountHandler.hpp"
#include "RegAuthDBAddHandler.hpp"
#include "RegAuthDBFetchHandler.hpp"
#include "ClientClosedHandler.hpp"
#include "PlatformAuthHandler.hpp"

using namespace ServerLib;

// RegAuthӦ���е���Ϣ�����߹�����
class CRegAuthHandlerSet : public CMsgHandlerSet
{
public:
	CRegAuthHandlerSet();

	virtual ~CRegAuthHandlerSet();

	//��ʼ����Ϣ���������
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:

	//���߳���Ϣ
	CRegisterAccountHandler m_stRegisterAccountHandler;
	CRegAuthDBAddHandler m_stRegAuthDBAddHandler;
	CRegAuthDBFetchHandler m_stRegAuthDBFetchHandler;
	CAuthAccountHandler m_stAuthAccountHandler;
	CClientClosedHandler m_stClientClosedHandler;

	//ƽ̨��֤�߳���Ϣ
	CPlatformAuthHandler m_stPlatformAuthHandler;

private:

	//������̣߳�Ϊ�߳�index
	int m_iThreadIndex;
};
