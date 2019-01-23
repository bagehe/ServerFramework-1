#include <string.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "ThreadLogManager.hpp"
#include "StringUtility.hpp"
#include "ConfigManager.hpp"

#include "DeleteRegAuthHandler.hpp"

using namespace ServerLib;

CDeleteRegAuthHandler::CDeleteRegAuthHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
    m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CDeleteRegAuthHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
    //Delete RegAuth ��������Ϣ
    m_pstRequestMsg = &stReqMsg;

	//������Ϣ
	GenerateMsgHead(pstHandleResult->stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_REGAUTHDB_DELETE_RESPONSE, 0);
	RegAuthDB_DelAccount_Response* pstResp = pstHandleResult->stResponseMsg.mutable_stbody()->mutable_m_stregauthdb_delete_response();

	//������Ϣ
    const RegAuthDB_DelAccount_Request& stReq = m_pstRequestMsg->stbody().m_stregauthdb_delete_request();
	const std::string& strAccount = stReq.stinfo().straccount();
	int iType = stReq.stinfo().itype();

    DEBUG_THREAD(m_iThreadIdx, "Handling DeleteRegAuthRequest, account %s, type %d\n", strAccount.c_str(), iType);

    //����AccountID��AccountTypeɾ����¼
    int iRet = DeleteAccount(strAccount, iType);
    if (iRet)
    {
		TRACE_THREAD(m_iThreadIdx, "Failed to delete account, ret %d, account %s, type %d\n", iRet, strAccount.c_str(), iType);

		pstResp->set_iresult(iRet);
        return;
    }

	pstResp->set_iresult(T_SERVER_SUCCESS);
    return;
}

//ɾ��RegAuth�ʺ�
int CDeleteRegAuthHandler::DeleteAccount(const std::string& strAccount, int iType)
{
    //��ȡRegAuthDB���ݿ��������
    const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRegAuthDBConfigByIndex(m_iThreadIdx);
    if(!pstDBConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to get regauth db config, thread index %d\n", m_iThreadIdx);
        return -1;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);

    char* pszQueryString = *m_ppQueryBuff;

    //����ɾ����SQL���
    int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE-1, "delete from %s where accountID= '%s' and accountType=%d", 
                 MYSQL_ACCOUNTINFO_TABLE, strAccount.c_str(), iType);

    int iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to execute delete sql query, account %s, ret %d\n", strAccount.c_str(), iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());

    return 0;
}
