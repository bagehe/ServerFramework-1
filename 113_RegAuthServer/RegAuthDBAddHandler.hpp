#pragma once

#include "MsgHandler.hpp"

// ��������RegAuthDB Server�� MSGID_REGAUTHDB_ADDACCOUNT_RESPONSE ��Ϣ
class CRegAuthDBAddHandler : public CMsgHandler
{
public:
    CRegAuthDBAddHandler();

	//�ڶ�����������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int OnResponseRegAuthDBAdd();

private:
	// �ͻ�����
	TNetHead_V2* m_pstNetHead; 

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg; 
	
	// ���������������ڱ�ʶһ��session
	unsigned int m_uiSessionFD;
	unsigned short m_unValue;
};
