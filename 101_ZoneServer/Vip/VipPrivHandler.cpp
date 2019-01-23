
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "Resource/ResourceUtility.h"
#include "VipUtility.h"

#include "VipPrivHandler.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CVipPrivHandler::~CVipPrivHandler()
{

}

int CVipPrivHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_GETUSERALMS_REQUEST:
	{
		//�����ȡ�ȼý�
		OnRequestGetAlms();
	}
	break;

	default:
		break;
	}

	return 0;
}

//�����ȡ�ȼý������
int CVipPrivHandler::OnRequestGetAlms()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GETUSERALMS_RESPONSE);
	Zone_GetUserAlms_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_getuseralms_response();

	//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const VipLevelConfig* pstVipConfig = stBaseCfgMgr.GetVipConfig(m_pRoleObj->GetVIPLevel());
	if (!pstVipConfig)
	{
		LOGERROR("Failed to get vip config, invalid level %d, uin %u\n", m_pRoleObj->GetVIPLevel(), m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_INVALID_CFG);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -3;
	}

	const VipPriv* pstPrivConfig = NULL;
	for (unsigned i = 0; i < pstVipConfig->vPrivs.size(); ++i)
	{
		if (pstVipConfig->vPrivs[i].iPrivType == VIP_PRIV_GETALMS)
		{
			pstPrivConfig = &pstVipConfig->vPrivs[i];
			break;
		}
	}

	if (!pstPrivConfig)
	{
		//��VIP�ȼ�������ȡ
		LOGERROR("Failed to get alms config, vip level %d\n", m_pRoleObj->GetVIPLevel());

		pstResp->set_iresult(T_ZONE_INVALID_CFG);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -4;
	}

	//�Ƿ����ȡ
	if (m_pRoleObj->GetNextAlmsTime() == 0)
	{
		//������ȡ
		LOGERROR("Failed to get vip alms, uin %u, vip level %d, almsnum %d\n", m_pRoleObj->GetUin(), m_pRoleObj->GetVIPLevel(), m_pRoleObj->GetAlmsNum());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -5;
	}

	//��ȡ�ȼý�
	if (!CResourceUtility::AddUserRes(*m_pRoleObj, RESOURCE_TYPE_COIN, pstPrivConfig->aiParams[2]))
	{
		//��ȡʧ��
		LOGERROR("Failed to get vip alms, can not add coins, uin %u\n", m_pRoleObj->GetUin());

		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -6;
	}

	//��ȡ��ҵ�ǰ������� ��Ӫ��־ʹ��
	long8 lNewCoinNum = m_pRoleObj->GetResource(RESOURCE_TYPE_COIN);

	//��ӡ��Ӫ��־  ��ȡ�ȼý�
	CZoneOssLog::TraceAlms(m_pRoleObj->GetUin(), m_pRoleObj->GetChannel(), m_pRoleObj->GetNickName(), m_pRoleObj->GetAlmsNum(), pstPrivConfig->aiParams[2],
		lNewCoinNum - pstPrivConfig->aiParams[2], lNewCoinNum);

	//���¾ȼý���Ϣ
	CVipUtility::UpdateAlmsInfo(*m_pRoleObj, m_pRoleObj->GetAlmsNum()+1, 0);

	//���ͷ���
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}
