#include <string.h>

#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "NowTime.hpp"
#include "UnixTime.hpp"
#include "StringUtility.hpp"
#include "AppDef.hpp"
#include "ConfigManager.hpp"

#include "FetchRegAuthHandler.hpp"

using namespace ServerLib;

CFetchRegAuthHandler::CFetchRegAuthHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
    m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CFetchRegAuthHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
    //��ȡ�ʺ���ϸ��Ϣ������
    m_pstRequestMsg = &stReqMsg;

	//������Ϣ
	GenerateMsgHead(pstHandleResult->stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_REGAUTHDB_FETCH_RESPONSE, 0);
	RegAuthDB_GetAccount_Response* pstResp = pstHandleResult->stResponseMsg.mutable_stbody()->mutable_m_stregauthdb_fetch_response();

	//��ȡ������Ϣ
    const RegAuthDB_GetAccount_Request& stReq = m_pstRequestMsg->stbody().m_stregauthdb_fetch_request();
	pstResp->mutable_stinfo()->CopyFrom(stReq.stinfo());

	const std::string& strAccount = stReq.stinfo().straccount(); 
	int iType = stReq.stinfo().itype();

    int iRet = FetchAccountInfo(stReq.stinfo(), *pstResp);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch account, ret %d, account %s, type:%d\n", iRet, strAccount.c_str(), iType);

		pstResp->set_iresult(iRet);
        return;
    }

    DEBUG_THREAD(m_iThreadIdx, "Success to fetch Account %s, type %d\n", strAccount.c_str(), iType);

	pstResp->set_iresult(T_SERVER_SUCCESS);
    return;
}

//��ȡ�����ʺ���ϸ��Ϣ
int CFetchRegAuthHandler::FetchAccountInfo(const AccountInfo& stInfo, RegAuthDB_GetAccount_Response& stResp)
{
    //�������ӵ�DB
    const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRegAuthDBConfigByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get RegAuth db config, index %d\n", m_iThreadIdx);
        return -1;
    }

    int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
        return iRet;
    }

    char* pszQueryString = *m_ppQueryBuff;
    int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE-1, "select * from %s where accountID='%s' and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, stInfo.straccount().c_str(), stInfo.itype());

    iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
        return iRet;
    }

     //�������
    int iRowNum = m_pDatabase->GetNumberRows();
    if(iRowNum != 1)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, invalid rows %d, account %s\n", iRowNum, stInfo.straccount().c_str());
        return T_REGAUTHDB_INVALID_RECORD;
    }

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, account %s, ret %d\n", stInfo.straccount().c_str(), iRet);
        return iRet;
    }

    //�ж�uFields�Ƿ����
    if(uFields != MYSQL_ACCOUNTINFO_FIELDS)
    {
        TRACE_THREAD(m_iThreadIdx, "Wrong result, real fields %u, needed %u\n", uFields, MYSQL_ACCOUNTINFO_FIELDS);
        return T_REGAUTHDB_INVALID_RECORD;
    }

    //�ֶ�3��password
    std::string strDBPassword(pstResult[3], pLengths[3]);

    //��������Ƿ�һ��
    if(strDBPassword.compare(stInfo.strpassword()) != 0)
    {
        return T_REGAUTHDB_INVALID_RECORD;
    }

    //�ֶ�0��AccountID, �ֶ�1��AccountType, ����

    //�ֶ�2��uin
	stResp.set_uin(strtoul(pstResult[2],NULL,10));

    //�ֶ�4��worldID
	stResp.set_iworldid(atoi(pstResult[4]));

    return T_SERVER_SUCCESS;
}
