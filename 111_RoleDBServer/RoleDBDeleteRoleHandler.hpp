#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// MSGID_ACCOUNT_DELETEROLE_REQUEST��Ϣ������
class CRoleDBDeleteRoleHandler : public CMsgHandler
{
public:
	CRoleDBDeleteRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int DeleteRole(const RoleID& stRoleID);
    int QueryRoleInfo(const RoleID& stRoleID);

private:
	DBClientWrapper* m_pDatabase;
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				//���ݿ������BUFF
	int m_iThreadIdx;
};

