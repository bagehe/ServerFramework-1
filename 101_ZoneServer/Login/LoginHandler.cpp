#include <string.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "StringUtility.hpp"
#include "SessionKeyUtility.hpp"
#include "LogAdapter.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/HandlerFactory.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/AppLoop.hpp"
#include "Kernel/HandlerHelper.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Kernel/UnitUtility.hpp"

#include "LoginHandler.hpp"

CLoginHandler::~CLoginHandler()
{
}

int CLoginHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_LOGINSERVER_REQUEST:
		{
			OnRequestLoginServer();
			break;
		}

	default:
		{
			break;
		}
	}

	return 0;
}

int CLoginHandler::OnRequestLoginServer()
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    int iRet = SecurityCheck(*m_pNetHead);
    if (iRet)
    {
        LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
        LoginFailed(*m_pNetHead);
        return -1;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    const Zone_LoginServer_Request& rstRequest = m_pRequestMsg->stbody().m_stzone_loginserver_request();
    iRet = LoginRole(rstRequest.stroleid(), m_pNetHead, rstRequest);
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_LOGINSERVER_FAILED);
        LoginFailed(*m_pNetHead);
    }

    return iRet;	
}

//֪ͨ��ɫ��¼
int CLoginHandler::NotifyRoleLogin(CGameRoleObj* pstRoleObj)
{
    return 0;
}

int CLoginHandler::SecurityCheck(const TNetHead_V2& rstNetHead)
{
    unsigned int uiUin = m_pRequestMsg->sthead().uin();
    if (uiUin == 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_PARA_ERROR);
		return -1;
    }
	
	Zone_LoginServer_Request* pstRequest = m_pRequestMsg->mutable_stbody()->mutable_m_stzone_loginserver_request();

	//���uin
	if (uiUin != pstRequest->stroleid().uin())
	{
		CHandlerHelper::SetErrorCode(T_ZONE_PARA_ERROR);
		return -2;
	}

	//�Ƿ����ְ���������
	if (CConfigManager::Instance()->GetBaseCfgMgr().IsContainMaskWord(pstRequest->strnickname()))
	{
		//����������
		CHandlerHelper::SetErrorCode(T_ZONE_PARA_ERROR);
		return -5;
	}

	//���ͻ��˰汾��
	//pstRequest->uclientversion();
	
	int iRealNameStat = REAL_STAT_NONAME;

	CGMPrivConfigManager& stGMPrivMgr = CConfigManager::Instance()->GetGMPrivConfigManager();
	if (CConfigManager::Instance()->IsSSKeyCheckEnabled() && !stGMPrivMgr.CheckIsGMIP(rstNetHead.m_uiSrcIP))
	{
		//��GM��ң����SessionKey
		int iRet = CheckSessionKey(pstRequest->strsessionkey(), uiUin, CModuleHelper::GetWorldID(), iRealNameStat);
		if (iRet)
		{
			CHandlerHelper::SetErrorCode(iRet);
			return -3;
		}
	}

	pstRequest->set_irealnamestat(iRealNameStat);

    //todo jasonxiong ����������Ҫ���Ӻڰ�����
    /*
    CWhiteListConfig& rWhiteListConfig = 
        WhiteListCfgMgr();
    if (rWhiteListConfig.IsInBlackList(uiUin))
    {
        TRACESVR("uin: %u is in black list, can not login\n", uiUin);
        CHandlerHelper::SetErrorCode(EQEC_UinInBlackList);
        return -2;
    }
    */
	
    return 0;
}

//У��SessionKey
int CLoginHandler::CheckSessionKey(const std::string& strSessionKey, unsigned uiUin, int nWorldID, int& iRealNameStat)
{
	//session key��ʽ��	 uin|time|WorldID|realname_stat|pic_url
	char szOriginKey[256] = { 0 };
	int iOriginKeyLen = sizeof(szOriginKey);
	int iRet = CSessionKeyUtility::DecryptSessionKey(strSessionKey.c_str(), strSessionKey.length(), szOriginKey, iOriginKeyLen);
	if (iRet)
	{
		LOGERROR("Failed to decrypt session key, uin %u, ret %d\n", uiUin, iRet);
		return T_ZONE_PARA_ERROR;;
	}

	unsigned uKeyUin = 0;
	unsigned uKeyTime = 0;
	int iKeyWorldID = 0;
	char szPicID[128] = { 0 };

	//session key : uin|time|WorldID|realname_stat|pic_url
	sscanf(szOriginKey, "%u|%u|%d|%d|%s", &uKeyUin, &uKeyTime, &iKeyWorldID, &iRealNameStat, szPicID);

	//�������ͷ��
	m_strPicID = szPicID;

	//������,session key 1���Ӻ�ʧЧ
	//todo jasonxiong �ȵ���30���ӣ��������
	if (uKeyUin != uiUin || iKeyWorldID != nWorldID || (CTimeUtility::GetNowTime() - (int)uKeyTime) > 60*30)
	{
		LOGERROR("Failed to check session key, uin %u, world %d.\n", uiUin, nWorldID);
		return T_ZONE_PARA_ERROR;
	}

	return T_SERVER_SUCCESS;
}

int CLoginHandler::LoginRole(const RoleID& stRoleID, TNetHead_V2* pNetHead, const Zone_LoginServer_Request& rstRequest)
{
    ASSERT_AND_LOG_RTN_INT(pNetHead);

	int iSessionID = (int)ntohl(pNetHead->m_uiSocketFD);

	LOGDETAIL("Login Req: Uin = %u, Session = %d\n", stRoleID.uin(), iSessionID);

	// ��Session�Ѿ�����
	CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
	CGameSessionObj* pSessionObj = pSessionManager->FindSessionByID(iSessionID);
	if (pSessionObj)
	{
		LOGERROR("Session Already Exist: ID = %d\n", iSessionID);
		return -2;
	}

	// ��RoleID��Session�Ѿ����ڣ��Ҵ���δ��¼״̬
	pSessionObj = pSessionManager->FindSessionByRoleID(stRoleID);
	if (pSessionObj && pSessionObj->GetBindingRole() == NULL)
	{
		LOGERROR("Session Alreay Binding Role: ID = %d, Uin = %u\n", iSessionID, stRoleID.uin());
		return -3;
	}

    // ����һ���µĻỰ
    pSessionObj = pSessionManager->CreateSession(pNetHead, stRoleID);
    if(!pSessionObj)
    {
        LOGERROR("Cannot Create Session: Uin = %u, SessionID = %d\n", stRoleID.uin(), iSessionID);  
        return -2;
    }

    // ����ͻ��˰汾��
    pSessionObj->SetClientVersion(rstRequest.uclientversion());

	//�����¼ԭ��
	pSessionObj->SetLoginReason(rstRequest.uloginreason());

	//�������ͷ��ID
	pSessionObj->SetPictureID(m_strPicID);

	if (rstRequest.strchannel().size() != 0)
	{
		//�ϱ���ҵ�¼��Ϣ
		CZoneOssLog::ReportUserLogin(stRoleID.uin(), rstRequest.straccount().c_str(), rstRequest.strdeviceid().c_str(), rstRequest.strchannel().c_str(),
			rstRequest.bisnew(), pSessionObj->GetClientIP());
	}

    // ��World������������,��ͬuin�Ķ�����ȥ
    int iRet = KickRoleFromWorldServer(stRoleID, iSessionID, rstRequest);
    if (iRet < 0)
    {
        CModuleHelper::GetSessionManager()->DeleteSession(iSessionID);
    }

    return iRet;
}

int CLoginHandler::SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead)
{
	static GameProtocolMsg stMsg;

    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);

    Zone_LoginServer_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_loginserver_response();
    pstResp->Clear();

    pstResp->set_iresult(uiResultID);

    CZoneMsgHelper::SendZoneMsgToClient(stMsg, rstNetHead);

    return 0;
}

int CLoginHandler::SendSuccessfulResponse(CGameRoleObj* pLoginRoleObj)
{
	static GameProtocolMsg stMsg;

    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_LOGINSERVER_RESPONSE);
    
    Zone_LoginServer_Response* pstLoginResp = stMsg.mutable_stbody()->mutable_m_stzone_loginserver_response();
    pstLoginResp->set_iresult(T_SERVER_SUCCESS);
	pstLoginResp->set_uin(pLoginRoleObj->GetUin());
    pstLoginResp->set_iworldid(CModuleHelper::GetWorldID());
    pstLoginResp->set_izoneid(CModuleHelper::GetZoneID());

    //����������ϵĻ�����Ϣ
    BASEDBINFO* pstBaseInfo = pstLoginResp->mutable_stlogininfo()->mutable_stbaseinfo();
    pLoginRoleObj->UpdateBaseInfoToDB(*pstBaseInfo);

    //������ҵı�����Ϣ
    ITEMDBINFO* pstItemInfo = pstLoginResp->mutable_stlogininfo()->mutable_stiteminfo();
    pLoginRoleObj->UpdateRepThingsToDB(*pstItemInfo);

	//������ҵ�������Ϣ
	QUESTDBINFO* pstQuestInfo = pstLoginResp->mutable_stlogininfo()->mutable_stquestinfo();
	pLoginRoleObj->UpdateQuestToDB(*pstQuestInfo);

	//������ҵĺ�����Ϣ

	//������ҵ��ʼ���Ϣ
	MAILDBINFO* pstMailInfo = pstLoginResp->mutable_stlogininfo()->mutable_stmailinfo();
	pLoginRoleObj->UpdateMailToDB(*pstMailInfo);

	//RESERVED2
	RESERVED2DBINFO* pstReserved2Info = pstLoginResp->mutable_stlogininfo()->mutable_streserved2();
	
	//��Ҷһ���Ϣ
	EXCHANGEDBINFO* pstExchangeInfo = pstReserved2Info->mutable_stexchangeinfo();
	pLoginRoleObj->UpdateExchangeToDB(*pstExchangeInfo);

	//��ҳ�ֵ��Ϣ
	RECHARGEDBINFO* pstRechargeInfo = pstReserved2Info->mutable_strechargeinfo();
	pLoginRoleObj->UpdateRechargeToDB(*pstRechargeInfo);

    CZoneMsgHelper::SendZoneMsgToRole(stMsg, pLoginRoleObj);

    return 0;
}

// ��¼�ɹ�����
int CLoginHandler::LoginOK(unsigned int uiUin, bool bNeedResponse)
{
    CGameRoleObj* pLoginRoleObj =  GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
    ASSERT_AND_LOG_RTN_INT(pLoginRoleObj);

    //��¼�¼�
    CGameEventManager::NotifyLogin(*pLoginRoleObj);

	if (bNeedResponse)
	{
		// ��¼�ɹ�
		SendSuccessfulResponse(pLoginRoleObj);
	}

    // ֪ͨ�����û�
    NotifyRoleLogin(pLoginRoleObj);

    LOGDEBUG("Uin: %u\n", uiUin);

    // ��¼��ˮ
    CZoneOssLog::TraceLogin(*pLoginRoleObj);

    return 0;
}

// ��¼ʧ�ܴ���
int CLoginHandler::LoginFailed(const TNetHead_V2& rstNetHead)
{ 
	unsigned int uiResultID = CHandlerHelper::GetErrorCode();
	SendFailedResponse(uiResultID, rstNetHead);

    return 0;
}

//ͨ��World����ͬuin���Ѿ���¼�ĺ�������
int CLoginHandler::KickRoleFromWorldServer(const RoleID& stRoleID, int iFromSessionID, const Zone_LoginServer_Request& rstRequest)
{
	static GameProtocolMsg stMsg;

    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_KICKROLE_REQUEST);

    World_KickRole_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_kickrole_request();
	pstReq->set_bislogin(iFromSessionID>0);
	pstReq->set_ukickeduin(stRoleID.uin());

	KickerInfo* pstKicker = pstReq->mutable_stkicker();
	pstKicker->set_ifromworldid(CModuleHelper::GetWorldID());
	pstKicker->set_ifromzoneid(CModuleHelper::GetZoneID());
	pstKicker->set_isessionid(iFromSessionID);
	pstKicker->mutable_stroleid()->CopyFrom(stRoleID);
	pstKicker->set_strnickname(rstRequest.strnickname());
	pstKicker->set_strchannel(rstRequest.strchannel());
	pstKicker->set_irealnamestat(rstRequest.irealnamestat());
	pstKicker->set_straccount(rstRequest.straccount());
	pstKicker->set_strdeviceid(rstRequest.strdeviceid());

    LOGDEBUG("Send KickRole Request: Uin = %u\n", stRoleID.uin());

    int iRet = CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

    return iRet;
}
