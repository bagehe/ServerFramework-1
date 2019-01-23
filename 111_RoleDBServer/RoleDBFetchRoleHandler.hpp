#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "SeqConverter.hpp"

// MSGID_ROLEDB_FETCHROLE_REQUEST��Ϣ������
class CRoleDBFetchRoleHandler : public CMsgHandler
{
public:
	CRoleDBFetchRoleHandler(DBClientWrapper* pDatabase, char** pQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    void OnFetchRoleRequest(SHandleResult& pstHandleResult);

    int QueryRole(const RoleID& stRoleID, World_FetchRole_Response& stResp);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//���ݿ������BUFF			
	int m_iThreadIdx;					//�����߳�idx
};
