#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "HashUtility.hpp"
#include "ConfigManager.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "SessionKeyUtility.hpp"
#include "ErrorNumDef.hpp"
#include "SessionManager.hpp"
#include "RegAuthMsgProcessor.hpp"
#include "ThreadLogManager.hpp"
#include "MainPlatformProxy.h"
#include "YmnPlatformProxy.h"

#include "PlatformAuthHandler.hpp"

using namespace ServerLib;

CPlatformAuthHandler::CPlatformAuthHandler()
{
	memset(m_apPlatformProxy, 0, sizeof(m_apPlatformProxy));

	//�Խ��˺�
	m_apPlatformProxy[LOGIN_PLATFORM_MAIN] = new CMainPlatformProxy;

	//��è��
	m_apPlatformProxy[LOGIN_PLATFORM_YMN] = new CYmnPlatformProxy;
}

//��������������
void CPlatformAuthHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
	ASSERT_AND_LOG_RTN_VOID(pstHandleResult);

	m_pstRequestMsg = &stReqMsg;
	switch (m_pstRequestMsg->sthead().uimsgid())
	{
	case MSGID_REGAUTH_PLATFORMAUTH_REQUEST:
	{
		//ƽ̨��֤
		OnRequestPlatformAuth(*pstHandleResult);
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

//�����߳�index
void CPlatformAuthHandler::SetThreadIdx(int iThreadIndex)
{
	m_iThreadIndex = iThreadIndex;
}

//ƽ̨��֤
int CPlatformAuthHandler::OnRequestPlatformAuth(SHandleResult& stHandleResult)
{
	//��ȡ����
	const RegAuth_PlatformAuth_Request& stReq = m_pstRequestMsg->stbody().m_stregauth_platformauth_request();
	const std::string& strAccount = stReq.stinfo().straccount();
	int iType = stReq.stinfo().itype();

	//������Ϣ
	GenerateMsgHead(stHandleResult.stResponseMsg, m_pstRequestMsg->sthead().uisessionfd(), MSGID_REGAUTH_PLATFORMAUTH_RESPONSE, 0);
	RegAuth_PlatformAuth_Response* pstResp = stHandleResult.stResponseMsg.mutable_stbody()->mutable_m_stregauth_platformauth_response();
	pstResp->mutable_stinfo()->CopyFrom(stReq.stinfo());
	
	//todo jasonxiong ��ʱǿ������Ϊ1
	pstResp->mutable_stinfo()->set_iworldid(1);
	
	//��ȡƽ̨��Ϣ
	CPlatformProxy* pstProxy = GetPlatformProxy(iType);
	if (!pstProxy)
	{
		TRACE_THREAD(m_iThreadIndex, "Failed to auth account, invalid type %d, account %s\n", iType, strAccount.c_str());

		pstResp->set_iresult(T_REGAUTH_PARA_ERROR);
		return -1;
	}

	//����ƽ̨��֤
	int iRet = pstProxy->PlatformAuth(stReq.stinfo(), *pstResp, m_iThreadIndex);
	if (iRet)
	{
		TRACE_THREAD(m_iThreadIndex, "Failed to auth platform, ret %d, account %s, type %d\n", iRet, strAccount.c_str(), iType);

		pstResp->set_iresult(iRet);
		return -2;
	}

	DEBUG_THREAD(m_iThreadIndex, "Success to auth platform, account %s, type %d\n", strAccount.c_str(), iType);

	pstResp->set_iresult(T_SERVER_SUCCESS);

	return T_SERVER_SUCCESS;
}

//��ȡƽ̨��Ϣ
CPlatformProxy* CPlatformAuthHandler::GetPlatformProxy(int iType)
{
	if (iType <= LOGIN_PLATFORM_INVALID || iType >= LOGIN_PLATFORM_MAX)
	{
		return NULL;
	}

	return m_apPlatformProxy[iType];
}
