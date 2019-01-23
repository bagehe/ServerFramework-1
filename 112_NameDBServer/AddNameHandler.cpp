#include <stdlib.h>
#include <string.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "ThreadLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "StringUtility.hpp"
#include "NameDBApp.hpp"
#include "ConfigManager.hpp"

#include "AddNameHandler.hpp"

using namespace ServerLib;


CAddNameHandler::CAddNameHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
	m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CAddNameHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	// �������ּ�¼������
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_ADDNEWNAME_REQUEST:
	{
		OnAddNameRequest(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}

	return;
}

//���б�Ҫ�Ĳ������
int CAddNameHandler::CheckParams(const AddNewName_Request& stReq)
{
	if (stReq.itype() <= EN_NAME_TYPE_INVALID || stReq.itype() >= EN_NAME_TYPE_MAX)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to add new name , invalid type %d\n", stReq.itype());
		return T_NAMEDB_PARA_ERROR;
	}

	if (stReq.strname().size() == 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to add new name, invalid param!\n");
		return T_NAMEDB_PARA_ERROR;
	}

	return T_SERVER_SUCCESS;
}

void CAddNameHandler::OnAddNameRequest(SHandleResult& stHandleResult)
{
	//��ȡ����
	const AddNewName_Request& stReq = m_pstRequestMsg->stbody().m_staddnewname_request();

	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_ADDNEWNAME_RESPONSE, 0);
	AddNewName_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_staddnewname_response();
	pstResp->set_itype(stReq.itype());
	pstResp->set_name_id(stReq.name_id());
	pstResp->set_strname(stReq.strname());

	int iRet = CheckParams(stReq);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to add new name request, param check failed, ret %d\n", iRet);

		pstResp->set_iresult(iRet);
		return;
	}

	//����ʺ��Ƿ����
	bool bIsExist = false;
	iRet = CheckNameExist(stReq.strname(), stReq.itype(), bIsExist);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to check name exist, ret 0x%0x\n", iRet);

		pstResp->set_iresult(iRet);
		return;
	}

	if (bIsExist)
	{
		//�ʺ��Ѿ�����
		TRACE_THREAD(m_iThreadIdx, "Failed to add new name, already exist, ret 0x%0x\n", T_NAMEDB_NAME_EXISTS);

		pstResp->set_iresult(T_NAMEDB_NAME_EXISTS);
		return;
	}

	//�����µļ�¼
	iRet = AddNewRecord(stReq.strname(), stReq.itype(), stReq.name_id());
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to add new name record, name %s, type %d, ret %d\n", stReq.strname().c_str(), stReq.itype(), iRet);

		pstResp->set_iresult(iRet);
		return;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
	return;
}

//����ʺ��Ƿ����
int CAddNameHandler::CheckNameExist(const std::string& strName, int iType, bool& bIsExist)
{
	//�������ӵ�DB
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetNameDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, index %d\n", m_iThreadIdx);
		return -1;
	}

	int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
		return iRet;
	}

	char* pszQueryString = *m_ppQueryBuff;
	int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE - 1, "select nameid from %s where nickname='%s' and nametype=%d",
		MYSQL_NAMEINFO_TABLE, strName.c_str(), iType);

	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
		return iRet;
	}

	if (m_pDatabase->GetNumberRows() != 0)
	{
		//�Ѿ����ڸ�����
		bIsExist = true;
	}
	else
	{
		bIsExist = false;
	}

	return T_SERVER_SUCCESS;
}

//�����µļ�¼
int CAddNameHandler::AddNewRecord(const std::string& strName, int iNameType, unsigned uNameID)
{
	//�������ӵ�DB
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetNameDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get name db config, index %d\n", m_iThreadIdx);
		return -1;
	}

	int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
		return iRet;
	}

	char* pszQueryString = *m_ppQueryBuff;
	int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE - 1, \
		"insert into %s(nickname,nametype,nameid) "
		"values('%s',%d,%u)",
		MYSQL_NAMEINFO_TABLE,
		strName.c_str(), iNameType, uNameID);

	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
		return iRet;
	}

	TRACE_THREAD(m_iThreadIdx, "Success to add new name: %s, type %d\n", strName.c_str(), iNameType);

	return T_SERVER_SUCCESS;
}
