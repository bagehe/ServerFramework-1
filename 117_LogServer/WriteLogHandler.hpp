#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"
#include "LogTargetProxy.hpp"

//д�����־������
// MSGID_WORLD_WRITELOG_REQUEST ��Ϣ������
class CWriteLogHandler : public CMsgHandler
{
public:
	CWriteLogHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

	//�����־
    void OnWriteLogRequest(SHandleResult& stHandleResult);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//DB����������					
	int m_iThreadIdx;					//�����߳�idx
	LogTargetProxy* m_apstLogProxy[LOG_TARGET_MAX];	//д��־Ŀ��ƽ̨����
};
