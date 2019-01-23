#pragma once

#include "MsgHandler.hpp"

// ��������LotusServer�� MSGID_REGAUTH_REGACCOUNT_REQUEST ��Ϣ
class CRegisterAccountHandler : public CMsgHandler
{
public:
    CRegisterAccountHandler();

	//�ڶ�����������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int OnRequestRegAccount();

	//�������ʺ�
	void AddAccount(unsigned uiSessionFd, const AccountInfo& stInfo);

    int CheckParam();

private:
	// �ͻ�����
	TNetHead_V2* m_pstNetHead;	

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg;	
	
	// ���������������ڱ�ʶһ��session
	unsigned int m_uiSessionFD;
	unsigned short m_unValue;
};
