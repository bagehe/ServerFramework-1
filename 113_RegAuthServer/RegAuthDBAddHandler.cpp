#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionObj.hpp"
#include "StringUtility.hpp"
#include "HashUtility.hpp"
#include "ConfigManager.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "SessionKeyUtility.hpp"
#include "ErrorNumDef.hpp"
#include "RegAuthMsgProcessor.hpp"

#include "RegAuthDBAddHandler.hpp"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRegAuthDBAddHandler::CRegAuthDBAddHandler()
{

}

//�ڶ�����������
void CRegAuthDBAddHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	ASSERT_AND_LOG_RTN_VOID(pstNetHead);

	//�ݲ�֧�ָ�Э��
	return;

	m_pstNetHead = pstNetHead;
	m_pstRequestMsg = &stReqMsg;

	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_REGAUTHDB_ADDACCOUNT_RESPONSE:
	{
		//�������ʺŵķ���
		OnResponseRegAuthDBAdd();
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

int CRegAuthDBAddHandler::OnResponseRegAuthDBAdd()
{
	//��ȡ����
	const RegAuthDB_AddAccount_Response& stResp = m_pstRequestMsg->stbody().m_stregauthdb_addaccount_response();

	//������Ϣ
	GenerateMsgHead(stMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_REGAUTH_REGACCOUNT_RESPONSE, 0);
	RegAuth_RegAccount_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stregauth_regaccount_response();
	pstResp->set_iresult(stResp.iresult());

	CRegAuthMsgProcessor::Instance()->SendRegAuthToLotus(m_pstRequestMsg->sthead().uisessionfd(), stMsg);

	return T_SERVER_SUCCESS;
}
