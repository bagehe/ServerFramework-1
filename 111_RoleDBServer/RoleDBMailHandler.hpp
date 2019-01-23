#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// MSGID_WORLD_SENDMAIL_REQUEST ��Ϣ������
class CRoleDBMailHandler : public CMsgHandler
{
public:
	CRoleDBMailHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx) { m_iThreadIdx = iThreadIdx; }

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

	//���������ʼ�
	int OfflineMail(SHandleResult& stHandleResult);

	//��ȡ��������
	int GetOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo);

	//���������ʼ���Ϣ
	void AddOfflineMailInfo(const OneMailInfo& stMailInfo, RESERVED1DBINFO& stOfflineInfo);

	//������������
	int UpdateOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// ���ݿ�����Ļ�����			
	int m_iThreadIdx;					//�����߳�idx
};
