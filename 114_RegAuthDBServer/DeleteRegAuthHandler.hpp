#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"

// MSGID_REGAUTHDB_DELETE_REQUEST ��Ϣ������
class CDeleteRegAuthHandler : public CMsgHandler
{
public:
	CDeleteRegAuthHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

    //ɾ��RegAuth�ʺ�
    int DeleteAccount(const std::string& strAccount, int iType);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// DB����������
	int m_iThreadIdx;
};
