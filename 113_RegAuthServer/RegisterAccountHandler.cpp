#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "StringUtility.hpp"
#include "HashUtility.hpp"
#include "TimeUtility.hpp"
#include "ConfigManager.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "ErrorNumDef.hpp"
#include "SessionManager.hpp"
#include "RegAuthMsgProcessor.hpp"
#include "RegAuthDBAddHandler.hpp"

#include "RegisterAccountHandler.hpp"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRegisterAccountHandler::CRegisterAccountHandler()
{

}

int CRegisterAccountHandler::CheckParam()
{
	return 0;
}

void CRegisterAccountHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	ASSERT_AND_LOG_RTN_VOID(pstNetHead);

	//�ݲ�֧�ָ�Э�飬ע�͵�
	return;

	m_pstNetHead = pstNetHead;
	m_pstRequestMsg = &stReqMsg;

	m_uiSessionFD = m_pstRequestMsg->sthead().uisessionfd();

	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_REGAUTH_REGACCOUNT_REQUEST:
	{
		//ע��ƽ̨�ʺŵ�����
		OnRequestRegAccount();
		return;
	}
	break;

	default:
	{

	}
	break;
	}

	return;
}

int CRegisterAccountHandler::OnRequestRegAccount()
{
	//������Ϣ
	GenerateMsgHead(stMsg, m_uiSessionFD, MSGID_REGAUTH_REGACCOUNT_RESPONSE, 0);
	RegAuth_RegAccount_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stregauth_regaccount_response();

	// ���������Ϣ���Ƿ���ڷǷ��ֶ�
	if (CheckParam() != 0)
	{
		TRACESVR("Invalid parameter found in the request\n");

		pstResp->set_iresult(T_REGAUTH_PARA_ERROR);
		CRegAuthMsgProcessor::Instance()->SendRegAuthToLotus(m_uiSessionFD, stMsg);
		return -1;
	}

	//��ȡ����
	const RegAuth_RegAccount_Request& stReq = m_pstRequestMsg->stbody().m_stregauth_regaccount_request();
	
	TRACESVR("Handling RegAccountRequest from lotus server, account: %s, type: %d\n",
		stReq.stinfo().straccount().c_str(), stReq.stinfo().itype());

	// ���session�Ƿ��Ѿ����ڣ�session�����Ƿ񵽴�����
	int iRet = CSessionManager::Instance()->CheckSession(*m_pstNetHead);
	if (iRet)
	{
		pstResp->set_iresult(iRet);
		CRegAuthMsgProcessor::Instance()->SendRegAuthToLotus(m_uiSessionFD, stMsg);
		return -4;
	}
	
	// ����NetHead����session
	CSessionObj* pSessionObj = CSessionManager::Instance()->CreateSession(*m_pstNetHead);
	ASSERT_AND_LOG_RTN_INT(pSessionObj);
	pSessionObj->SetCreatedTime(CTimeUtility::GetNowTime());

	m_uiSessionFD = pSessionObj->GetSessionFD();
	m_unValue = pSessionObj->GetValue();
	TRACESVR("sockfd: %u, value: %d\n", m_uiSessionFD, m_unValue);

	unsigned int uiSessionFd = m_uiSessionFD;

	//ֱ��ת��ע�������RegAuthDB
	AddAccount(uiSessionFd, stReq.stinfo());

	return T_SERVER_SUCCESS;
}

//�������ʺ�
void CRegisterAccountHandler::AddAccount(unsigned uiSessionFd, const AccountInfo& stInfo)
{
	unsigned uiHashNum = CHashUtility::BKDRHash(stInfo.straccount().c_str(), stInfo.straccount().size());

	//������Ϣͷ
	GenerateMsgHead(stMsg, uiSessionFd, MSGID_REGAUTHDB_ADDACCOUNT_REQUEST, uiHashNum);

	//RegAuthDB�������ʺŵ�����
	RegAuthDB_AddAccount_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stregauthdb_addaccount_request();
	pstReq->mutable_stinfo()->CopyFrom(stInfo);

	pstReq->set_iworldid(1);

	//��������
	char szEncryptPasswd[256] = { 0 };
	int iEncryptBuffLen = sizeof(szEncryptPasswd);

	int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(stInfo.strpassword().c_str(), stInfo.strpassword().size(), szEncryptPasswd, iEncryptBuffLen);
	if (iRet)
	{
		TRACESVR("Failed to encrypt account password, account: %s, password: %s\n", stInfo.straccount().c_str(), stInfo.strpassword().c_str());
		return;
	}

	//��������Ϊ���ܺ������
	pstReq->mutable_stinfo()->set_strpassword(szEncryptPasswd, strlen(szEncryptPasswd));

	//ת����Ϣ��RegAuthDBServer
	iRet = CRegAuthMsgProcessor::Instance()->SendRegAuthToDB(stMsg);
	{
		TRACESVR("Failed to send add account request to Account DB server\n");
		return;
	}

	LOGDEBUG("Send add account request to Account DB server\n");

	return;
}
