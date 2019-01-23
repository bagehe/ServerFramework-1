#include <assert.h>
#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/HandlerFactory.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/UpdateRoleInfoHandler.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Login/LoginHandler.hpp"
#include "Kernel/HandlerHelper.hpp"
#include "CommDefine.h"
#include "Login/LogoutHandler.hpp"
#include "Resource/ResourceUtility.h"
#include "RepThings/RepThingsUtility.hpp"

#include "FetchRoleWorldHandler.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler(void)
{
}

int CFetchRoleWorldHandler::OnClientMsg()
{
    int iRet = -1;
    switch (m_pRequestMsg->sthead().uimsgid())
    {
    case MSGID_WORLD_FETCHROLE_RESPONSE:
        {
            iRet = OnFetchRole();
            break;
        }

    default:
        {
            break;
        }
    }

    return iRet;
}

int CFetchRoleWorldHandler::OnFetchRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->stbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // ��ȡ�Ự
    CSessionManager* pSessionManager = CModuleHelper::GetSessionManager();
    m_pSession = pSessionManager->FindSessionByRoleID(rstFetchResp.stroleid());
    if(!m_pSession)
    {
        TRACESVR("Cannot Find Session: Uin = %u\n", uiUin);
        return -2;
    }

    // Session �Ѿ���ʹ��,loginʱ���
    if (rstFetchResp.bislogin() && m_pSession->GetBindingRole() != NULL)
    {
        TRACESVR("Session Already Binding Role: Uin = %u, Session = %d\n", uiUin, m_pSession->GetID());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        return -3;
    }

    // ��ȡ��ɫ��Ϣʧ��
    if (rstFetchResp.iresult() != 0)
    {
        TRACESVR("FetchRole Failed: Uin = %u, ResultID = %d\n", uiUin, rstFetchResp.iresult());
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());
        return -4;
    }

    // ���uin�Ƿ�һ��
    if (uiUin != m_pSession->GetRoleID().uin())
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // ���World����
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("Invalid Session: Uin = %u\n", uiUin);

        return -5;
    }

    TRACESVR("FetchRole OK: Uin = %u\n", uiUin);

    // ��¼��ɫ
    int iRet = LoginRole();
    if (iRet < 0)
    {
        CHandlerHelper::SetErrorCode(T_ZONE_SESSION_EXISTS_ERR);
        CLoginHandler::LoginFailed(m_pSession->GetNetHead());
        pSessionManager->DeleteSession(m_pSession->GetID());

        // ���World����
        CLogoutHandler::NotifyLogoutToWorld(rstFetchResp.stroleid());

        TRACESVR("LoginRole Failed: iRet = %d\n", iRet);

        return -6;
    }

    TRACESVR("LoginRole OK: Uin = %u\n", uiUin);

    return 0;
}

int CFetchRoleWorldHandler::LoginRole()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->stbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    // ������ɫ����
    m_pRoleObj = (CGameRoleObj*)CUnitUtility::CreateUnit(uiUin);
    if(!m_pRoleObj)
    {
        TRACESVR("Create RoleObj Failed.\n");
        return -1;
    }

    // ��ʼ����ɫ����ϵͳ
    int iRet = m_pRoleObj->InitRole(rstFetchResp.stroleid());
    if (iRet < 0)
    {
        TRACESVR("Init Role Failed: iRet = %d\n", iRet);

        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());

        return -2;
    }

    // ���Ự�ͽ�ɫ��
    m_pSession->SetBindingRole(m_pRoleObj);
    m_pRoleObj->SetSessionID(m_pSession->GetID());

    // ��ʼ����ɫ����
    iRet = InitRoleData();
    if (iRet < 0)
    {
        CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());
        TRACESVR("Init RoleData Failed: iRet = %d\n", iRet);

        return -3;
    }

    // ֪ͨ��¼�ɹ�
    CLoginHandler::LoginOK(uiUin);

    // ��½���ʼ��
    iRet = InitRoleAfterLogin(*m_pRoleObj, rstFetchResp);
	if (iRet)
	{
		CUnitUtility::DeleteUnit(&m_pRoleObj->GetUnitInfo());
		TRACESVR("InitRoleAfterLogin Failed: iRet = %d\n", iRet);
		
		return -4;
	}

    return 0;
}

// ��ʼ����ɫ����
int CFetchRoleWorldHandler::InitRoleData()
{
    const World_FetchRole_Response& rstFetchResp = m_pRequestMsg->stbody().m_stworld_fetchrole_response();
    unsigned int uiUin = rstFetchResp.stroleid().uin();

    CGameSessionObj *pSession = m_pRoleObj->GetSession();
    if (!pSession)
    {
        TRACESVR("No session: %u\n", uiUin);
        return -20;
    }

    //1.��ʼ����ҵĻ�����Ϣ
    BASEDBINFO stBaseInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strbaseinfo(), stBaseInfo))
    {
        TRACESVR("Failed to decode base proto data, uin %u\n", uiUin);
        return -21;
    }
    m_pRoleObj->InitBaseInfoFromDB(stBaseInfo, rstFetchResp.stkicker());

    //2.��ʼ����ҵ�������Ϣ
    QUESTDBINFO stQuestInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strquestinfo(), stQuestInfo))
    {
        TRACESVR("Failed to decode quest proto data, uin %u\n", uiUin);
        return -22;
    }
	m_pRoleObj->InitQuestFromDB(stQuestInfo);
    
    //3.��ʼ����ҵ���Ʒ��Ϣ
    ITEMDBINFO stItemInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().striteminfo(), stItemInfo))
    {
        TRACESVR("Failed to decode item proto data, uin %u\n", uiUin);
        return -23;
    }
    m_pRoleObj->InitRepThingsFromDB(stItemInfo);

    //4.��ʼ����ҵĺ�����Ϣ
    FRIENDDBINFO stFriendInfo;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strfriendinfo(), stFriendInfo))
    {
        TRACESVR("Failed to decode friend proto data, uin %u\n", uiUin);
        return -26;
    }

	//5.��ʼ����ҵ��ʼ���Ϣ
	MAILDBINFO stMailInfo;
	if (!DecodeProtoData(rstFetchResp.stuserinfo().strmailinfo(), stMailInfo))
	{
		TRACESVR("Failed to decode mail proto data, uin %u\n", uiUin);
		return -27;
	}
	m_pRoleObj->InitMailFromDB(stMailInfo);

    //6.��ҵ�Reserved1�ֶ�,�������ݵ����߼�����

    //7.��ʼ����ҵ�Reserved2�ֶ�
    RESERVED2DBINFO stReserved2Info;
    if(!DecodeProtoData(rstFetchResp.stuserinfo().strreserved2(), stReserved2Info))
    {
        TRACESVR("Failed to decode reserved2 proto data, uin %u\n", uiUin);
        return -29;
    }

	//��Ҷһ���Ϣ
	m_pRoleObj->InitExchangeFromDB(stReserved2Info.stexchangeinfo());

	//��ҳ�ֵ��Ϣ
	m_pRoleObj->InitRechargeFromDB(stReserved2Info.strechargeinfo());

    // GM��־
    //todo jasonxiong ��ע�͵�����GM��صĹ��ܣ�������Ҫʱ�ٽ��п���
    /*
    m_pRoleObj->GetRoleInfo().m_stBaseProfile.m_cGMType = rstDBRoleInfo.fGM;
    CWhiteListConfig& rWhiteListConfig = WhiteListCfgMgr();
    bool bGM = rWhiteListConfig.IsInGMList(m_pRoleObj->GetRoleID().m_uiUin);
    if (bGM)
    {
        CUnitUtility::SetUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    }
    else
    {
        CUnitUtility::ClearUnitStatus(&(m_pRoleObj->GetRoleInfo().m_stUnitInfo), EUS_ISGM);
    } 
    */ 

    // ����ʱ��
    m_pRoleObj->SetLoginCount(stBaseInfo.ilogincount()+1);

    m_pRoleObj->SetOnline();      

	//������ҽ�����Ϣ
	m_pRoleObj->UpdateTallyInfo();

	if (m_pRoleObj->GetLoginCount() == 1)
	{
		//���˺ŵ�һ�ε�¼,����ʵʱ��Ӯ
		m_pRoleObj->GetTallyInfo().lUserWinNum = m_pRoleObj->GetUserWinNum();
		m_pRoleObj->SetUserWinNum(0);
	}

    return 0;
}

// ��¼���ʼ��
int CFetchRoleWorldHandler::InitRoleAfterLogin(CGameRoleObj& stRoleObj, const World_FetchRole_Response& stResp)
{
	unsigned uiUin = stResp.stroleid().uin();

	//���ͷ���ʱ��ͬ��
	CUnitUtility::SendSyncTimeNotify(&stRoleObj, CTimeUtility::GetMSTime());

	//������������
	RESERVED1DBINFO stReserved1Info;
	if (!DecodeProtoData(stResp.stuserinfo().strreserved1(), stReserved1Info))
	{
		TRACESVR("Failed to decode reserved1 proto data, uin %u\n", uiUin);
		return -1;
	}

	int iRet = T_SERVER_SUCCESS;

	//�������߳�ֵ
	const PAYOFFLINEDBINFO& stPayInfo = stReserved1Info.stpayinfo();
	CRechargeManager& stRechargeMgr = stRoleObj.GetRechargeManager();
	for (int i = 0; i < stPayInfo.strecords_size(); ++i)
	{
		const RechargeRecord& stOneRecharge = stPayInfo.strecords(i);
		iRet = stRechargeMgr.UserRecharge(stOneRecharge.irechargeid(), stOneRecharge.itime(), stOneRecharge.strorderid());
		if (iRet)
		{
			//ֻ��ӡ������־
			LOGERROR("Failed to do offline recharge, recharge id %d, time %d, uin %u\n", stOneRecharge.irechargeid(), stOneRecharge.itime(), uiUin);
		}
	}

	//��������������Դ
	const RESOFFLINEDBINFO& stResInfo = stReserved1Info.stresinfo();
	for (int i = 0; i < stResInfo.staddres_size(); ++i)
	{
		const AddResInfo& stOneAddRes = stResInfo.staddres(i);
		if (!CResourceUtility::AddUserRes(stRoleObj, stOneAddRes.irestype(), stOneAddRes.iaddnum()))
		{
			LOGERROR("Failed to offline add res type %d, num %d, uin %u\n", stOneAddRes.irestype(), stOneAddRes.iaddnum(), uiUin);
		}
	}

	//�������ߵ���
	const ItemSlotInfo& stItemInfo = stReserved1Info.stiteminfo();
	for (int i = 0; i < stItemInfo.stslots_size(); ++i)
	{
		const OneSlotInfo& stOneItem = stItemInfo.stslots(i);
		iRet = CRepThingsUtility::AddItemNum(stRoleObj, stOneItem.iitemid(), stOneItem.iitemnum(), ITEM_CHANNEL_GMADD);
		if (iRet)
		{
			LOGERROR("Failed to offline add item, id %d, num %d, uin %u\n", stOneItem.iitemid(), stOneItem.iitemnum(), uiUin);
		}
	}

	//���������ʼ�
	const MAILOFFLINEDBINFO& stMailInfo = stReserved1Info.stmailinfo();
	for (int i = 0; i < stMailInfo.stmails_size(); ++i)
	{
		//�����ʼ�
		stRoleObj.GetMailManager().AddNewMail(stMailInfo.stmails(i));
	}

    return 0;
}
