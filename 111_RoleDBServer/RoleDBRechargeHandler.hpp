#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// MSGID_WORLD_RECHARGE_REQUEST ��Ϣ������
class CRoleDBRechargeHandler : public CMsgHandler
{
public:
	CRoleDBRechargeHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    
	//���߳�ֵ
	int OfflineRecharge(SHandleResult& stHandleResult);
	
	//��ȡ��ֵ�����Ϣ
	int OnGetUserInfoRequest(SHandleResult& stHandleResult);

	//��ȡ��������
	int GetOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo);

	//�������߳�ֵ��Ϣ
	void AddOfflineRechargeInfo(int iRechargeID, int iTime, const std::string& strOrderID, RESERVED1DBINFO& stOfflineInfo);

	//������������
	int UpdateOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo);

	//��ȡ��������
	int GetBaseInfo(unsigned uin, BASEDBINFO& stBaseInfo);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// ���ݿ�����Ļ�����			
	int m_iThreadIdx;					//�����߳�idx
};
