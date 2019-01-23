#include <string.h>

#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"
#include "ConfigManager.hpp"
#include "ErrorNumDef.hpp"

#include "RoleDBListRoleHandler.hpp"

CRoleDBListRoleHandler::CRoleDBListRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{    
    m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CRoleDBListRoleHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_ACCOUNT_LISTROLE_REQUEST:
	{
		AccountListRole(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}
}

int CRoleDBListRoleHandler::AccountListRole(SHandleResult& stHandleResult)
{
	//��ȡ����
	const Account_ListRole_Request& stReq = m_pstRequestMsg->stbody().m_staccountlistrolerequest();
	unsigned int uiUin = stReq.uin();
	unsigned short usWorldID = stReq.world();
    short usNewWorldID = 0;

	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_ACCOUNT_LISTROLE_RESPONSE, uiUin);
	Account_ListRole_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_staccountlistroleresponse();

    // ����uin��DBRoleInfo���м�����seq���������е�world��Ϣ������ָ��world�ϵ�role�б�
	int iErrnoNum = T_ROLEDB_SQL_EXECUTE_FAILED;
	int iRet = QueryAndParseRole(uiUin, usWorldID, usNewWorldID, *pstResp, iErrnoNum);

	TRACE_THREAD(m_iThreadIdx, "Info of ListRoleResponse: result: %u, uin: %u, world: %u:%u, role exist: %d\n",
		iRet, uiUin, usWorldID, usNewWorldID, pstResp->bexist());

    if (iRet != 0)
    {
		pstResp->set_iresult(iErrnoNum);
        return -1;
    }

	pstResp->set_iresult(T_SERVER_SUCCESS);
	return 0;
}

int CRoleDBListRoleHandler::QueryAndParseRole(const unsigned int uiUin, short nWorldID, short nNewWorldID, 
	Account_ListRole_Response& stResp, int& iErrnoNum)
{
	//��ѯDB���Ƿ��Ѿ���������ʺ�

	//��ȡROLEDB��ص�����
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRoleDBConfigByIndex(m_iThreadIdx);
	if(!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get roledb config, invalid index %d\n", m_iThreadIdx);
		return -2;
	}

	//����Ҫ���ʵ����ݿ���Ϣ
	m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

	//����SQL���
	char* pszQueryString = *m_ppQueryBuff;
	static int iQueryStringLen = MAX_QUERYBUFF_SIZE-1;
	int iLength = 0;

	//1��������������2������Ԥ������,�����������ܻ��������������
	if(nNewWorldID==1 || nNewWorldID==2)
	{
		//��������Ԥ��������������������ݽ���
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, "select uin,seq from %s where uin=%u\n", MYSQL_USERINFO_TABLE, uiUin);
	}
	else
	{
		//������ʽ�������ǺϷ������
		iLength = SAFE_SPRINTF(pszQueryString, iQueryStringLen, 
							   "select uin,seq from %s where uin = %u and floor(seq%%10000/10)=%d\n", 
							   MYSQL_USERINFO_TABLE, uiUin, nWorldID);
	}

	//ִ��
	int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if(iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute select sql query, uin %u\n", uiUin);
		return iRet;
	}

	//�������ؽ��
	int iRowNum = m_pDatabase->GetNumberRows();
	if(iRowNum == 0)
	{
		//��������Ҽ�¼��ֱ�ӷ��سɹ�
		stResp.set_bexist(false);
		return T_SERVER_SUCCESS;
	}
	
	TRACE_THREAD(m_iThreadIdx, "List Role Num %d, uin %u, world id %d\n", iRowNum, uiUin, nWorldID);

	MYSQL_ROW pstResult = NULL;
	unsigned long* pLengths = NULL;
	unsigned int uFields = 0;

	iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, uin %u, ret %d\n", uiUin, iRet);
		return iRet;
	}

	stResp.set_bexist(true);
	stResp.set_world(nWorldID);
	stResp.mutable_stroleid()->set_uin(uiUin);
	stResp.mutable_stroleid()->set_uiseq(atoi(pstResult[1]));

    return T_SERVER_SUCCESS;
}

