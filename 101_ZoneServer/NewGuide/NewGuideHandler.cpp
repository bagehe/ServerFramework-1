
#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/ZoneOssLog.hpp"

#include "NewGuideHandler.h"

static GameProtocolMsg stMsg;

CNewGuideHandler::~CNewGuideHandler(void)
{

}

CNewGuideHandler::CNewGuideHandler()
{

}

int CNewGuideHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_FINGUIDE_REQUEST:
	{
		//���������������
		OnRequestFinGuide();
	}
	break;

	case MSGID_ZONE_UPDATENAME_REQUEST:
	{
		//����޸���������
		OnRequestUpdateName();
	}

	default:
	{
		LOGERROR("Failed to handler request msg, invalid msgid: %u\n", m_pRequestMsg->sthead().uimsgid());
		return -1;
	}
	break;
	}

	return 0;
}

//���������������
int CNewGuideHandler::OnRequestFinGuide()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��ȡ����
	const Zone_FinGuide_Request& stReq = m_pRequestMsg->stbody().m_stzone_finguide_request();

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_FINGUIDE_RESPONSE);
	Zone_FinGuide_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_finguide_response();
	pstResp->set_iguideid(stReq.iguideid());

	//������
	if (stReq.iguideid() <= 0 || stReq.iguideid() > 32)
	{
		LOGERROR("Failed to fin new guide, invalid guide id %d, uin %u\n", stReq.iguideid(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//����Ϊ���
	m_pRoleObj->SetFinGuide(stReq.iguideid());

	//��ӡ��Ӫ��־�������������
	CZoneOssLog::TraceFinNewGuide(m_pRoleObj->GetUin(), m_pRoleObj->GetChannel(), m_pRoleObj->GetNickName(), stReq.iguideid());

	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//����޸���������
int CNewGuideHandler::OnRequestUpdateName()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//��ȡ����
	const Zone_UpdateName_Request& stReq = m_pRequestMsg->stbody().m_stzone_updatename_request();

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UPDATENAME_RESPONSE);
	Zone_UpdateName_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_updatename_response();
	pstResp->set_strnewname(stReq.strnewname());
	pstResp->set_bissign(stReq.bissign());

	//������
	if (stReq.strnewname().size() <= 0 || stReq.strnewname().size() >= MAX_NICK_NAME_LENGTH)
	{
		//���ֳ��Ȳ���
		LOGERROR("Failed to change name, invalid new name %s, uin %u\n", stReq.strnewname().c_str(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	//�Ƿ����ְ���������
	if (CConfigManager::Instance()->GetBaseCfgMgr().IsContainMaskWord(stReq.strnewname()))
	{
		//����������
		LOGERROR("Failed to renew name, contains mask word, new name %s, uin %u\n", stReq.strnewname().c_str(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -5;
	}

	if (stReq.bissign())
	{
		//�޸�ǩ��
		m_pRoleObj->SetSign(stReq.strnewname().c_str());
	}
	else
	{
		//�޸�����

		//�Ƿ��Ѿ��޸Ĺ�����
		if (m_pRoleObj->GetNameChanged())
		{
			//�Ѿ��޸Ĺ�
			LOGERROR("Failed to change nick name, already changed, uin %u\n", m_pRoleObj->GetUin());

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -3;
		}

		//�޸�����
		m_pRoleObj->SetNickName(stReq.strnewname().c_str());
		m_pRoleObj->SetNameChanged(true);
	}

	//ǿ�Ƹ������а�
	m_pRoleObj->SetUpdateRank(true);

	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}
