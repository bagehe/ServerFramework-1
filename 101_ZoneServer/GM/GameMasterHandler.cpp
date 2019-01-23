
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "GMReposityCommand.hpp"
#include "GMResourceCommand.hpp"
#include "GMHorseLampCommand.hpp"

#include "GameMasterHandler.hpp"

static GameProtocolMsg stMsg;

CGameMasterHandler::~CGameMasterHandler(void)
{

}

CGameMasterHandler::CGameMasterHandler()
{
	//ע��GMCommand
	RegisterGMCommand();
}

int CGameMasterHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_GAMEMASTER_REQUEST:
	{
		//����ԱGM����
		OnRequestGM();
	}
	break;

	case MSGID_WORLD_GAMEMASTER_REQUEST:
	{
		//Worldת��GM
		OnRequestWorldGM();
	}
	break;

	case MSGID_WORLD_GAMEMASTER_RESPONSE:
	{
		//Worldת��GM����
		OnResponseWorldGM();
	}
	break;

	default:
	{
		LOGERROR("Failed to handler request msg, invalid msgid: %u\n", m_pRequestMsg->sthead().uimsgid());
		return -1;
	}
	break;
	}

	return 0;
}

int CGameMasterHandler::OnRequestGM()
{
    //У���������Ч��
    int iRet = SecurityCheck();
    if(iRet)
    {
        LOGERROR("Failed to check request param, uin %u, ret %d\n", m_pRequestMsg->sthead().uin(), iRet);
        return iRet;
    }

	unsigned uin = m_pRoleObj->GetUin();

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GAMEMASTER_RESPONSE);
	GameMaster_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_gamemaster_response();

	//���GMȨ��
	if (!CheckIsGMUser())
	{
		LOGERROR("Failed to run gm command, invalid gmuser %u\n", uin);

		pstResp->set_iresult(T_ZONE_INVALID_GMUSER);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return T_ZONE_INVALID_GMUSER;
	}

	//������
	const GameMaster_Request& stReq = m_pRequestMsg->stbody().m_stzone_gamemaster_request();
	if (stReq.ioperatype() <= GM_OPERA_INVALID || stReq.ioperatype() >= GM_OPERA_MAX)
	{
		LOGERROR("Failed to run gm command, invalid opera type %d, gm user %u\n", stReq.ioperatype(), uin);

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return T_ZONE_PARA_ERROR;
	}

	if (!m_apGMCommand[stReq.ioperatype()])
	{
		LOGERROR("Failed to get gm handler, opera type %d, gm user %u\n", stReq.ioperatype(), uin);
		
		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return T_ZONE_PARA_ERROR;
	}

	if (stReq.ioperatype() == GM_OPERA_HORSELAMP)
	{
		//����Ʋ���Ҫת��World
		iRet = m_apGMCommand[stReq.ioperatype()]->Run(uin, stReq, *pstResp);
		if (iRet)
		{
			LOGERROR("Failed to run gm command, ret %d, opera type %d, gm user %u\n", iRet, stReq.ioperatype(), uin);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}

		pstResp->set_iresult(T_SERVER_SUCCESS);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return T_SERVER_SUCCESS;
	}

	//ת��GM����World
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_GAMEMASTER_REQUEST);
	stMsg.mutable_sthead()->set_uin(uin);
	
	GameMaster_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_gamemaster_request();
	pstReq->CopyFrom(stReq);
	pstReq->set_ufromuin(uin);
	pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());

	//���͵�World
	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

    return T_SERVER_SUCCESS;
}

//Worldת��GM
int CGameMasterHandler::OnRequestWorldGM()
{
	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_GAMEMASTER_RESPONSE);
	stMsg.mutable_sthead()->set_uin(m_pRequestMsg->sthead().uin());
	GameMaster_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stworld_gamemaster_response();

	//������
	const GameMaster_Request& stReq = m_pRequestMsg->stbody().m_stworld_gamemaster_request();
	pstResp->set_ioperatype(stReq.ioperatype());
	pstResp->set_ufromuin(stReq.ufromuin());
	pstResp->set_utouin(stReq.utouin());

	unsigned uGMUin = m_pRequestMsg->sthead().uin();
	if (stReq.ioperatype() <= GM_OPERA_INVALID || stReq.ioperatype() >= GM_OPERA_MAX)
	{
		LOGERROR("Failed to run gm command, invalid opera type %d, gm user %u\n", stReq.ioperatype(), uGMUin);

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return T_ZONE_PARA_ERROR;
	}

	if (!m_apGMCommand[stReq.ioperatype()])
	{
		LOGERROR("Failed to get gm handler, opera type %d, gm user %u\n", stReq.ioperatype(), uGMUin);

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return T_ZONE_PARA_ERROR;
	}

	//�Ը��˵Ĳ���
	int iRet = m_apGMCommand[stReq.ioperatype()]->Run(stReq.utouin(), stReq, *pstResp);
	if (iRet)
	{
		LOGERROR("Failed to run gm command, ret %d, opera type %d, gm user %u\n", iRet, stReq.ioperatype(), uGMUin);

		pstResp->set_iresult(iRet);
		CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
		return iRet;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	return T_SERVER_SUCCESS;
}

//Worldת��GM����
int CGameMasterHandler::OnResponseWorldGM()
{
	//��ȡWorld����
	const GameMaster_Response& stResp = m_pRequestMsg->stbody().m_stworld_gamemaster_response();

	//��ȡGMUser
	m_pRoleObj = CUnitUtility::GetRoleByUin(stResp.ufromuin());
	if (!m_pRoleObj)
	{
		return 0;
	}

	//��ȡ������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GAMEMASTER_RESPONSE);
	GameMaster_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_gamemaster_response();
	pstResp->CopyFrom(stResp);

	//���ͷ���
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return 0;
}

//����Ƿ���GM�û�
int CGameMasterHandler::CheckIsGMUser()
{
    //���ȼ���Ƿ�GM��ҵ�uin
    CGMPrivConfigManager& rstGMPrivConfigMgr = CConfigManager::Instance()->GetGMPrivConfigManager();
    if(rstGMPrivConfigMgr.CheckIsGMUin(m_pRoleObj->GetUin()))
    {
        return true;
    }

    unsigned int uClientIP = inet_network(m_pSession->GetClientIP());
    if(rstGMPrivConfigMgr.CheckIsGMIP(uClientIP))
    {
        return true;
    }

    return false;
}

//ע��GMCommand
void CGameMasterHandler::RegisterGMCommand()
{
	//�����������
	m_apGMCommand[GM_OPERA_ADDITEM] = &m_stReposityHandler;
	m_apGMCommand[GM_OPERA_GETREPINFO] = &m_stReposityHandler;

	//�����Դ���
	m_apGMCommand[GM_OPERA_ADDRES] = &m_stResouceHandler;
	m_apGMCommand[GM_OPERA_RECHARGE] = &m_stResouceHandler;
	m_apGMCommand[GM_OPERA_GETBASEINFO] = &m_stResouceHandler;

	//��������
	m_apGMCommand[GM_OPERA_HORSELAMP] = &m_stHorseLampHandler;

	//GM�ʼ����
	m_apGMCommand[GM_OPERA_SENDMAIL] = &m_stMailHandler;

	return;
}
