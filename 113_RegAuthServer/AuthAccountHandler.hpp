#pragma once

#include "MsgHandler.hpp"

// ��������LotusServer��MSGID_AUTHACCOUNT_REQUEST��Ϣ
class CAuthAccountHandler : public CMsgHandler
{
public:
    CAuthAccountHandler();
    
	//�ڶ�����������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

	//��֤�ʺŵ�����
    int OnRequestAuthAccount();

	//ƽ̨��֤����
	int OnResponsePlatformAuth();

	//RegAuthDB��֤����
	int OnResponseRegDBAuth();

	//����ƽ̨��֤
	int PlatformAuthAccount(const AccountInfo& stInfo);

	//������֤����RegAuthDB
	int SendAuthToRegAuthDB(const AccountInfo& stInfo);

private:
	// �ͻ�����
	TNetHead_V2* m_pstNetHead;  
	
	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg; 
	
	// ���������������ڱ�ʶһ��session
	unsigned int m_uiSessionFD;
	unsigned short m_unValue;
};
