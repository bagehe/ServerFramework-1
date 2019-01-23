#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"

//��֤�˺ŵ�����

// MSGID_REGAUTH_AUTHACCOUNT_REQUEST ��Ϣ������
class CAuthAccountHandler : public CMsgHandler
{
public:
	CAuthAccountHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx) { m_iThreadIdx = iThreadIdx; }

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

	//��֤����˺�����
	int OnRequestAuthAccount(SHandleResult& stHandleResult);

	//��֤����˺�
	//bIsExist�����˺��Ƿ����
	int AuthAccount(const AccountInfo& stInfo, RegAuth_AuthAccount_Response& stResp, bool& bIsExist);

	//�˺Ų����ڣ������˺�
	int AddNewAccount(const AccountInfo& stInfo, RegAuth_AuthAccount_Response& stResp);

	//��ȡ����uin
	int GetAvaliableUin(unsigned int& uin);

	//�����µļ�¼
	int AddNewRecord(const AccountInfo& stInfo, unsigned int uin, RegAuth_AuthAccount_Response& stResp);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//DB����������					
	int m_iThreadIdx;					//�����߳�idx
};
