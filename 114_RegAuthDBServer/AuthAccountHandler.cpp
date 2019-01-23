#include <stdlib.h>
#include <string.h>

#include "GameProtocol.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"
#include "ThreadLogManager.hpp"
#include "UnixTime.hpp"
#include "NowTime.hpp"
#include "TimeUtility.hpp"
#include "StringUtility.hpp"
#include "ConfigManager.hpp"

#include "AuthAccountHandler.h"

using namespace ServerLib;

CAuthAccountHandler::CAuthAccountHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
	m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CAuthAccountHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	if (!pstHandleResult)
	{
		return;
	}

	// �������ʺż�¼������
	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_REGAUTH_AUTHACCOUNT_REQUEST:
	{
		OnRequestAuthAccount(*pstHandleResult);
	}
	break;

	default:
	{

	}
	break;
	}

	return;
}

//��֤����˺�����
int CAuthAccountHandler::OnRequestAuthAccount(SHandleResult& stHandleResult)
{
	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, 0);
	RegAuth_AuthAccount_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_stregauth_authaccount_response();

	//��ȡ����
	const RegAuth_AuthAccount_Request& stReq = m_pstRequestMsg->stbody().m_stregauth_authaccount_request();
	pstResp->mutable_stinfo()->CopyFrom(stReq.stinfo());
	const std::string& strAccount = stReq.stinfo().straccount();
	int iType = stReq.stinfo().itype();

	//��֤�˺�
	bool bIsExist = false;
	int iRet = AuthAccount(stReq.stinfo(), *pstResp, bIsExist);
	if (iRet)
	{
		//��֤ʧ��
		TRACE_THREAD(m_iThreadIdx, "Failed to auth account, ret %d, account %s, type %d\n", iRet, strAccount.c_str(), iType);
		
		pstResp->set_iresult(iRet);
		return -3;
	}

	if (!bIsExist)
	{
		//�˺Ų����ڣ������˺�
		iRet = AddNewAccount(stReq.stinfo(), *pstResp);
		if (iRet)
		{
			//�����˺�ʧ��
			TRACE_THREAD(m_iThreadIdx, "Failed to add new account, ret %d, account %s, type %d\n", iRet, strAccount.c_str(), iType);

			pstResp->set_iresult(iRet);
			return -4;
		}

		//ע���˺�
		pstResp->set_bisregister(true);
	}

	//��֤�ɹ�
	DEBUG_THREAD(m_iThreadIdx, "Success to auth account %s, type %d\n", strAccount.c_str(), iType);

	pstResp->set_iresult(T_SERVER_SUCCESS);
	return 0;
}

//��֤����˺�
//bIsExist�����˺��Ƿ����
int CAuthAccountHandler::AuthAccount(const AccountInfo& stInfo, RegAuth_AuthAccount_Response& stResp, bool& bIsExist)
{
	//������Ϊ����
	bIsExist = true;

	//�������ӵ�DB
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRegAuthDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get regauth db config, index %d\n", m_iThreadIdx);
		return T_REGAUTHDB_SQL_FAILED;
	}

	int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
		return iRet;
	}

	char* pszQueryString = *m_ppQueryBuff;
	int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE - 1, "select uin,worldid,password from %s where account='%s' and platformtype=%d",
		MYSQL_ACCOUNTINFO_TABLE, stInfo.straccount().c_str(), stInfo.itype());

	//ִ�в�ѯ
	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, true);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
		return iRet;
	}

	//�������
	int iRowNum = m_pDatabase->GetNumberRows();
	if (iRowNum == 0)
	{
		//�������˺�
		bIsExist = false;
		return T_SERVER_SUCCESS;
	}
	
	MYSQL_ROW pstResult = NULL;
	unsigned long* pLengths = NULL;
	unsigned int uFields = 0;

	iRet = m_pDatabase->FetchOneRow(pstResult, pLengths, uFields);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to fetch rows, account %s, ret %d\n", stInfo.straccount().c_str(), iRet);
		return iRet;
	}

	//У������,�ֶ�2 password
	std::string strDBPassword(pstResult[2], pLengths[2]);
	if (stInfo.strpassword().compare(strDBPassword) != 0)
	{
		//У������ʧ��
		TRACE_THREAD(m_iThreadIdx, "Failed to auth account, invalid password, account %s, type %d\n", stInfo.straccount().c_str(), stInfo.itype());

		return T_REGAUTHDB_INVALID_PASSWD;
	}

	//�ֶ�0 uin
	stResp.set_uin(strtoul(pstResult[0], NULL, 10));

	//�ֶ�1 worldid
	stResp.set_iworldid(atoi(pstResult[1]));

	return T_SERVER_SUCCESS;
}

//�˺Ų����ڣ������˺�
int CAuthAccountHandler::AddNewAccount(const AccountInfo& stInfo, RegAuth_AuthAccount_Response& stResp)
{
	//����Ψһuin
	unsigned uin = 0;
	int iRet = GetAvaliableUin(uin);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get avaliable uin, ret %d\n", iRet);
		return iRet;
	}

	//�����µļ�¼
	iRet = AddNewRecord(stInfo, uin, stResp);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to add new record, account %s, type %d, ret %d\n", stInfo.straccount().c_str(), stInfo.itype(), iRet);
		return iRet;
	}

	return T_SERVER_SUCCESS;
}

//��ȡ����uin
int CAuthAccountHandler::GetAvaliableUin(unsigned int& uin)
{
	//��ȡ�ʺ����ݿ������
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetUniqUinDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get UniqUinDB config!\n");
		return T_REGAUTHDB_SQL_FAILED;
	}

	int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, db name %s\n", pstDBConfig->szDBName);
		return iRet;
	}

	//����SQL���
	char* pszQueryString = *m_ppQueryBuff;
	int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE - 1, "insert into %s set uin=NULL", MYSQL_UNIQUININFO_TABLE);

	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
		return iRet;
	}

	uin = m_pDatabase->GetLastInsertID();
	if (uin == 0)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get uniq uin, should not be zero!\n");
		return T_REGAUTHDB_SQL_FAILED;
	}

	DEBUG_THREAD(m_iThreadIdx, "Success to get new uin: %u\n", uin);

	return T_SERVER_SUCCESS;
}

//�����µļ�¼
int CAuthAccountHandler::AddNewRecord(const AccountInfo& stInfo, unsigned int uin, RegAuth_AuthAccount_Response& stResp)
{
	//�������ӵ�DB
	const OneDBInfo* pstDBConfig = ConfigManager::Instance()->GetRegAuthDBConfigByIndex(m_iThreadIdx);
	if (!pstDBConfig)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to get RegAuth db config, index %d\n", m_iThreadIdx);
		return T_REGAUTHDB_SQL_FAILED;
	}

	int iRet = m_pDatabase->SetMysqlDBInfo(pstDBConfig->szDBHost, pstDBConfig->szUserName, pstDBConfig->szUserPasswd, pstDBConfig->szDBName);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to set mysql db info, ret %d\n", iRet);
		return iRet;
	}

	//�����˺�ʱ��
	char szCreateTime[32] = { 0 };
	CTimeUtility::ConvertUnixTimeToTimeString(CTimeUtility::GetNowTime(), szCreateTime);

	char* pszQueryString = *m_ppQueryBuff;
	int iLength = SAFE_SPRINTF(pszQueryString, MAX_QUERYBUFF_SIZE - 1, \
		"insert into %s(account,platformtype,uin,worldid,createtime,password,thirdparty,deviceid,channelid,clienttype,appid)"
		"values('%s',%d,%u,%u,'%s','%s','%s','%s','%s','%s','%s')", MYSQL_ACCOUNTINFO_TABLE,
		stInfo.straccount().c_str(), stInfo.itype(), uin, stInfo.iworldid(), szCreateTime, stInfo.strpassword().c_str(), 
		stInfo.strthirdparty().c_str(), stInfo.strdeviceid().c_str(), stInfo.strchannel().c_str(), stInfo.strclienttype().c_str(), 
		stInfo.strappid().c_str());

	iRet = m_pDatabase->ExecuteRealQuery(pszQueryString, iLength, false);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIdx, "Failed to execute sql query, ret %d\n", iRet);
		return iRet;
	}

	//���÷���
	stResp.set_uin(uin);
	stResp.set_iworldid(stInfo.iworldid());

	DEBUG_THREAD(m_iThreadIdx, "Success to add new account %s, type %d, uin %u\n", stInfo.straccount().c_str(), stInfo.itype(), uin);

	return T_SERVER_SUCCESS;
}
