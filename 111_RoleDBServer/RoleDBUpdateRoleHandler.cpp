#include <string.h>

#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"
#include "ConfigManager.hpp"
#include "ErrorNumDef.hpp"

#include "RoleDBUpdateRoleHandler.hpp"

using namespace ServerLib;

CRoleDBUpdateRoleHandler::CRoleDBUpdateRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
	m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CRoleDBUpdateRoleHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_UPDATEROLE_REQUEST:
	{
		OnUpdateRoleRequest(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}
}

void CRoleDBUpdateRoleHandler::OnUpdateRoleRequest(SHandleResult& stHandleResult)
{
	//��ȡ����
	const World_UpdateRole_Request& stReq = m_pstRequestMsg->stbody().m_stworld_updaterole_request();

	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, 0, MSGID_WORLD_UPDATEROLE_RESPONSE, stReq.stroleid().uin());
	World_UpdateRole_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_stworld_updaterole_response();
	pstResp->mutable_stroleid()->CopyFrom(stReq.stroleid());											 

	int iRet = UpdateRole(stReq);
	if (iRet != 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute db query, ret %d\n", iRet);

		pstResp->set_iresult(iRet);
		return;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
}

int CRoleDBUpdateRoleHandler::UpdateRole(const World_UpdateRole_Request& stReq)
{
	//��ȡ���ӵ����ݿ���ص�����
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRoleDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid thread index %d\n", m_iThreadIdx);
		return -2;
	}

	//����Ҫ���������ݿ������Ϣ
	m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

	//��ʼ��SQL���
	int iLength = 0;
	char* pszQueryString = *m_ppQueryBuff;
	int iRet = GenerateQueryString(stReq, pszQueryString, MAX_QUERYBUFF_SIZE - 1, iLength);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to generate update query sql string, ret %d\n", iRet);
		return iRet;
	}

	//ִ��
	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, ret %d, uin %u\n", iRet, stReq.stroleid().uin());
		return iRet;
	}

	DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

	return 0;
}

int CRoleDBUpdateRoleHandler::GenerateQueryString(const World_UpdateRole_Request& stReq, char* pszBuff, int iBuffLen, int& iLength)
{
	if (!pszBuff)
	{
		TRACE_THREAD(m_iThreadIdx, "Fail to generate update query string, invlaid buff pointer!\n");
		return T_ROLEDB_PARA_ERR;
	}

	memset(pszBuff, 0, iBuffLen);

	SAFE_SPRINTF(pszBuff, iBuffLen - 1, "update %s set ", MYSQL_USERINFO_TABLE);
	char* pEnd = pszBuff + strlen(pszBuff);

	MYSQL& stDBConn = m_pDatabase->GetCurMysqlConn();

	const GameUserInfo& rstUserInfo = stReq.stuserinfo();

	//1.��һ�����Ϣ
	if (rstUserInfo.strbaseinfo().size() != 0)
	{
		SAFE_SPRINTF(pEnd, iBuffLen - 1, "base_info=");
		pEnd += strlen("base_info=");

		//��һ�����Ϣ base_info
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strbaseinfo().c_str(), rstUserInfo.strbaseinfo().size());
		*pEnd++ = '\'';
	}

	//2.���������Ϣ
	if (rstUserInfo.strquestinfo().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "quest_info=");

		pEnd += strlen("quest_info=");

		//��ҵ�������Ϣ�ֶ�
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strquestinfo().c_str(), rstUserInfo.strquestinfo().size());
		*pEnd++ = '\'';
	}

	//3.��ҵ���Ʒ��Ϣ
	if (rstUserInfo.striteminfo().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "item_info=");

		pEnd += strlen("item_info=");

		//��ҵ���Ʒ��Ϣ item_info
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.striteminfo().c_str(), rstUserInfo.striteminfo().size());
		*pEnd++ = '\'';
	}

	//4.��ҵĺ�����Ϣ
	if (rstUserInfo.strfriendinfo().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "friend_info=");

		pEnd += strlen("friend_info=");

		//��ҵĺ�����Ϣ
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfriendinfo().c_str(), rstUserInfo.strfriendinfo().size());
		*pEnd++ = '\'';
	}

	//5.��ҵ��ʼ���Ϣ
	if (rstUserInfo.strmailinfo().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "mail_info=");

		pEnd += strlen("mail_info=");

		//��ҵĺ�����Ϣ
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strmailinfo().c_str(), rstUserInfo.strmailinfo().size());
		*pEnd++ = '\'';
	}

	//6.��ҵı����ֶ�1
	if (rstUserInfo.strreserved1().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "reserved1=");

		pEnd += strlen("reserved1=");

		//�����ֶ�1�ĸ���
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved1().c_str(), rstUserInfo.strreserved1().size());
		*pEnd++ = '\'';
	}

	//7.��ҵı����ֶ�2
	if (rstUserInfo.strreserved2().size() != 0)
	{
		*pEnd++ = ',';

		SAFE_SPRINTF(pEnd, iBuffLen - 1, "reserved2=");

		pEnd += strlen("reserved2=");

		//�����ֶ�2�ĸ���
		*pEnd++ = '\'';
		pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved2().c_str(), rstUserInfo.strreserved2().size());
		*pEnd++ = '\'';
	}

	pEnd += SAFE_SPRINTF(pEnd, iBuffLen - 1, " where uin = %u and seq = %u\n", stReq.stroleid().uin(), stReq.stroleid().uiseq());

	iLength = pEnd - pszBuff;

	return T_SERVER_SUCCESS;
}
