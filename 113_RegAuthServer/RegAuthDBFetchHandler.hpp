#pragma once

#include "MsgHandler.hpp"

// ��������RegAuthDB Server�� MSGID_REGAUTHDB_FETCH_REQUEST ��Ϣ
class CRegAuthDBFetchHandler : public CMsgHandler
{
public:
    CRegAuthDBFetchHandler();

	//�ڶ�����������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int OnResponseRegAuthDBFetch();

    //������֤�ɹ��ظ���LotusServer
    void SendAuthSuccessResponseToLotus(unsigned int uiSessionFd, unsigned int uin, int iWorldID);

private:
	// �ͻ�����
	TNetHead_V2* m_pstNetHead;  

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg; 
	
	// ���������������ڱ�ʶһ��session
	unsigned int m_uiSessionFD;
	unsigned short m_unValue;
};
