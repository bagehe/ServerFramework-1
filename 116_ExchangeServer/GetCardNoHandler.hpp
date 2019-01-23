#pragma once

#include <string>

#include "MsgHandler.hpp"

//���̴߳���Handler
class CGetCardNoHandler : public CMsgHandler
{
public:
	CGetCardNoHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	//��������������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

	//�����߳�index
	void SetThreadIdx(int iThreadIndex);

private:

	//��ȡ����
	int OnRequestGetCardNo(SHandleResult& stHandleResult);

private:

	//�������ݿ��ָ��
	DBClientWrapper* m_pDatabase;		

	//DB����������
	char** m_ppQueryBuff;				

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg;

	//�߳�idnex
	int m_iThreadIndex;
};
