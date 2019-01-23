#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// MSGID_ROLEDB_UPDATEROLE_REQUEST��Ϣ������
class CRoleDBUpdateRoleHandler : public CMsgHandler
{
public:
	CRoleDBUpdateRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);
	
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    void OnUpdateRoleRequest(SHandleResult& stHandleResult);

private:
    int UpdateRole(const World_UpdateRole_Request& stReq);
    int GenerateQueryString(const World_UpdateRole_Request& iRet, char* pszBuff, int iBuffLen, int& iLength);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// ���ݿ����������			
	int m_iThreadIdx;					//�����߳�idx
};
