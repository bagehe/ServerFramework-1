#pragma once

#include "MsgHandler.hpp"
#include "PlatformProxy.h"

//���̴߳���Handler

class CPlatformAuthHandler : public CMsgHandler
{
public:
	CPlatformAuthHandler();

	//��������������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

	//�����߳�index
	void SetThreadIdx(int iThreadIndex);

private:

	//ƽ̨��֤
	int OnRequestPlatformAuth(SHandleResult& stHandleResult);

	//��ȡƽ̨��Ϣ
	CPlatformProxy* GetPlatformProxy(int iType);

private:

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg;

	//��֤ƽ̨��Ϣ
	CPlatformProxy* m_apPlatformProxy[LOGIN_PLATFORM_MAX];

	//�߳�idnex
	int m_iThreadIndex;
};
