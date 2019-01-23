#include <stdlib.h>
#include <string.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "ThreadLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "LogServerApp.hpp"
#include "ConfigManager.hpp"
#include "MofangLogProxy.hpp"
#include "TapDBLogProxy.hpp"

#include "WriteLogHandler.hpp"

using namespace ServerLib;


CWriteLogHandler::CWriteLogHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
	m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;

	//��ʼ��д��־����
	memset(m_apstLogProxy, 0, sizeof(m_apstLogProxy));

	m_apstLogProxy[LOG_TARGET_TAPDB] = new CTapDBLogProxy;
	m_apstLogProxy[LOG_TARGET_MOFANG] = new CMofangLogProxy;
}

void CWriteLogHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	// д��־������
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_WRITELOG_REQUEST:
	{
		OnWriteLogRequest(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}

	return;
}

//�����־
void CWriteLogHandler::OnWriteLogRequest(SHandleResult& stHandleResult)
{
	//����Ϊ����Ҫ�ظ�
	stHandleResult.iNeedResponse = 0;

	//��ȡ����
	const World_WriteLog_Request& stReq = m_pstRequestMsg->stbody().m_stworld_writelog_request();

	//������
	if (stReq.ilogtargettype() <= LOG_TARGET_INVALID || stReq.ilogtargettype() >= LOG_TARGET_MAX)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to write log, invalid target %d, log %s\n", stReq.ilogtargettype(), stReq.strlogdata().c_str());
		return;
	}

	//����Ŀ�����������־
	int iRet = m_apstLogProxy[stReq.ilogtargettype()]->WriteLog(m_pDatabase, *m_ppQueryBuff, m_iThreadIdx, stReq.strlogdata());
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to write log, ret %d, target %d, log %s\n", iRet, stReq.ilogtargettype(), stReq.strlogdata().c_str());
		return;
	}

	return;
}
