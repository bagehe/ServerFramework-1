#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

// MSGID_WORLD_GAMEMASTER_REQUEST ��Ϣ������
class CRoleDBGMHandler : public CMsgHandler
{
public:
	CRoleDBGMHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);
	
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

	//��������GM
	int GMOperaOffline(SHandleResult& stHandleResult);

	//�޸��������
	int GMModifyRoleData(const GameMaster_Request& stReq);

	//�����������
	int GMUpdateRoleData(unsigned uiUin, const GameLoginInfo& stUserInfo, int iDataType);

	//��ȡ�������
	int GMGetRoleData(unsigned uiUin, GameLoginInfo& stUserInfo, int iDataType);

private:
	DBClientWrapper* m_pDatabase;		// �������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				// ���ݿ�����Ļ�����	
	int m_iThreadIdx;					//�����߳�idx
};
