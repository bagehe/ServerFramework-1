#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"

//������ɫ��Ϣ
class CRoleDBCreateRoleHandler : public CMsgHandler
{
public:
	CRoleDBCreateRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:
    int InsertNewRoleRecord(const World_CreateRole_Request& stReq, unsigned int uiSeq);

    //��ȡ��ҽ�ɫ������ʱ��
    void GetRoleCreateTime(unsigned short& usCreateTime);

    //���ɲ����SQL Query���
    int GenerateQueryString(const World_CreateRole_Request& stReq, unsigned int uiSeq, char* pszBuff, int iBuffLen, int& iLength);

private:
	DBClientWrapper* m_pDatabase;
	GameProtocolMsg* m_pstRequestMsg;	// ���������Ϣ
	char** m_ppQueryBuff;				//���ݿ������BUFF
	int m_iThreadIdx;
};
