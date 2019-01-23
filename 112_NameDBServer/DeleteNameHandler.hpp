#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"

// MSGID_DELETENAME_REQUEST ��Ϣ������
class CDeleteNameHandler : public CMsgHandler
{
public:
	CDeleteNameHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);
	
	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

    //ɾ��Name��Ϣ
    int DeleteName(const std::string& strName, int iType);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//DB����������					
	int m_iThreadIdx;					//�����߳�idx
};
