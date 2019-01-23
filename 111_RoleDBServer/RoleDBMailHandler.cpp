#include <string.h>

#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"
#include "ConfigManager.hpp"
#include "ErrorNumDef.hpp"
#include "ProtoDataUtility.hpp"

#include "RoleDBMailHandler.hpp"

using namespace ServerLib;

CRoleDBMailHandler::CRoleDBMailHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
	m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CRoleDBMailHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_SENDMAIL_REQUEST:
	{
		OfflineMail(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}
}

int CRoleDBMailHandler::OfflineMail(SHandleResult& stHandleResult)
{
	//��ȡ����
	const World_SendMail_Request& stReq = m_pstRequestMsg->mutable_stbody()->m_stworld_sendmail_request();

	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, 0, MSGID_WORLD_SENDMAIL_RESPONSE, stReq.uifromuin());
	World_SendMail_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_stworld_sendmail_response();
	pstResp->set_uifromuin(stReq.uifromuin());
	pstResp->set_uitouin(stReq.uitouin());
	pstResp->set_itrytimes(stReq.itrytimes()+1);
	pstResp->mutable_stmailinfo()->CopyFrom(stReq.stmailinfo());

	//��ȡROLEDB��ص�����
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRoleDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid index %d\n", m_iThreadIdx);

		pstResp->set_iresult(T_ROLEDB_PARA_ERR);
		return T_ROLEDB_PARA_ERR;
	}

	//����Ҫ���ʵ����ݿ���Ϣ
	m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

	//��ȡ��������
	RESERVED1DBINFO stOfflineInfo;
	int iRet = GetOfflineInfo(stReq.uitouin(), stOfflineInfo);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get offline info, uin %u, ret %d\n", stReq.uitouin(), iRet);

		pstResp->set_iresult(iRet);
		return -2;
	}

	//���������ʼ���Ϣ
	AddOfflineMailInfo(stReq.stmailinfo(), stOfflineInfo);

	//������������
	iRet = UpdateOfflineInfo(stReq.uitouin(), stOfflineInfo);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to update offline info, uin %u, ret %d\n", stReq.uitouin(), iRet);

		pstResp->set_iresult(iRet);
		return -3;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
	return 0;
}

//��ȡ��������
int CRoleDBMailHandler::GetOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo)
{
	if (!m_pDatabase)
	{
		return -1;
	}

	//����SQL���
	char* pszQueryString = *m_ppQueryBuff;
	static int iQueryStringLen = MAX_QUERYBUFF_SIZE - 1;

	//��ȡ��������
	int iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, "select reserved1 from %s where uin=%u\n", MYSQL_USERINFO_TABLE, uin);

	//ִ��
	int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, uin %u\n", uin);
		return iRet;
	}

	//�������ؽ��
	int iRowNum = m_pDatabase->GetNumberRows();
	if (iRowNum == 0)
	{
		//�˺Ų�����
		return T_ROLEDB_INVALID_RECORD;
	}

	MYSQL_ROW pstResult = NULL;
	unsigned long* pLengths = NULL;
	unsigned int uFields = 0;

	iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %u, ret %d\n", uin, iRet);
		return iRet;
	}

	std::string strOffline(pstResult[0], pLengths[0]);
	if (!DecodeProtoData(strOffline, stOfflineInfo))
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to decode reserved1 data, uin %u\n", uin);
		return T_ROLEDB_INVALID_RECORD;
	}

	return T_SERVER_SUCCESS;
}

//���������ʼ���Ϣ
void CRoleDBMailHandler::AddOfflineMailInfo(const OneMailInfo& stMailInfo, RESERVED1DBINFO& stOfflineInfo)
{
	MAILOFFLINEDBINFO* pstMailInfo = stOfflineInfo.mutable_stmailinfo();
	pstMailInfo->add_stmails()->CopyFrom(stMailInfo);;

	return;
}

//������������
int CRoleDBMailHandler::UpdateOfflineInfo(unsigned uin, RESERVED1DBINFO& stOfflineInfo)
{
	if (!m_pDatabase)
	{
		return -1;
	}

	//����SQL���
	char* pszQueryString = *m_ppQueryBuff;
	static int iQueryStringLen = MAX_QUERYBUFF_SIZE - 1;

	//��ȡ��������
	SAFE_SPRINTF(pszQueryString, iQueryStringLen, "update %s set ", MYSQL_USERINFO_TABLE);
	char* pEnd = pszQueryString + strlen(pszQueryString);

	//�����������
	std::string strOffline;
	if (!EncodeProtoData(stOfflineInfo, strOffline))
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to encode proto data, uin %u\n", uin);
		return T_ROLEDB_INVALID_RECORD;
	}

	if (strOffline.size() == 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to update offline info, invalid length, uin %u\n", uin);
		return T_ROLEDB_PARA_ERR;
	}

	pEnd += SAFE_SPRINTF(pEnd, iQueryStringLen, "reserved1=");

	//�����������
	*pEnd++ = '\'';
	pEnd += mysql_real_escape_string(&m_pDatabase->GetCurMysqlConn(), pEnd, strOffline.c_str(), strOffline.size());
	*pEnd++ = '\'';

	pEnd += SAFE_SPRINTF(pEnd, iQueryStringLen, " where uin = %u\n", uin);

	int iLength = pEnd - pszQueryString;

	//ִ��
	int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, uin %u\n", uin);
		return iRet;
	}

	DEBUG_THREAD(m_iThreadIdx, "Success to update offline info, uin %u\n", uin);

	return T_SERVER_SUCCESS;
}
