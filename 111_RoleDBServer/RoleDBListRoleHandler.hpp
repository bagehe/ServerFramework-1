#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// LISTROLE_ACCOUNT_REQUEST��Ϣ������
class CRoleDBListRoleHandler : public CMsgHandler
{
public:

	CRoleDBListRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int QueryAndParseRole(const unsigned int uiUin, short nWorldID, short nNewWorldID, Account_ListRole_Response& stResp, int& iErrnoNum);
	int AccountListRole(SHandleResult& stHandleResult);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// ���ݿ�����Ļ�����					
	int m_iThreadIdx;					//�����߳�idx
};
