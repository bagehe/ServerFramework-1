#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"

//MSGID_REGAUTHDB_FETCH_REQUEST ��Ϣ������
class CFetchRegAuthHandler : public CMsgHandler
{
public:
	CFetchRegAuthHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

    //��ȡ�����ʺ���ϸ��Ϣ
    int FetchAccountInfo(const AccountInfo& stInfo, RegAuthDB_GetAccount_Response& stResp);

private:
	DBClientWrapper* m_pDatabase;
	GameProtocolMsg* m_pstRequestMsg; // ���������Ϣ
	char** m_ppQueryBuff;
	int m_iThreadIdx;
};
