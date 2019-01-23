
#include "GameProtocol.hpp"
#include "TimeUtility.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "Kernel/ZoneMsgHelper.hpp"

#include "VipUtility.h"

static GameProtocolMsg stMsg;

//���¾ȼý���Ϣ
void CVipUtility::UpdateAlmsInfo(CGameRoleObj& stRoleObj, int iAlmsNum, int iNextAlmsTime)
{
	//������Ϣ
	stRoleObj.SetAlmsNum(iAlmsNum);
	stRoleObj.SetNextAlmsTime(iNextAlmsTime);
	stRoleObj.SetLastAlmsUpdateTime(CTimeUtility::GetNowTime());

	//����֪ͨ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UPDATEALMS_NOTIFY);
	Zone_UpdateAlms_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_updatealms_notify();
	pstNotify->set_igetnum(iAlmsNum);
	pstNotify->set_inextgettime(iNextAlmsTime);

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, &stRoleObj);

	return;
}

//�����ȼý����
int CVipUtility::TriggerAlmsUpdate(CGameRoleObj& stRoleObj)
{
	int iTimeNow = CTimeUtility::GetNowTime();

	//�Ƿ����
	if (!CTimeUtility::IsInSameDay(iTimeNow, stRoleObj.GetLastAlmsUpdateTime()))
	{
		//����
		UpdateAlmsInfo(stRoleObj, 0, 0);
	}

	//�Ƿ�ɴ���
	if (stRoleObj.GetResource(RESOURCE_TYPE_COIN) != 0 || stRoleObj.GetNextAlmsTime()!=0)
	{
		//�����㴥������
		return T_SERVER_SUCCESS;
	}

	//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const VipLevelConfig* pstVipConfig = stBaseCfgMgr.GetVipConfig(stRoleObj.GetVIPLevel());
	if (!pstVipConfig)
	{
		LOGERROR("Failed to get vip config, invalid level %d, uin %u\n", stRoleObj.GetVIPLevel(), stRoleObj.GetUin());
		return T_ZONE_INVALID_CFG;
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
		//��VIP�ȼ�û�оȼý�
		return T_SERVER_SUCCESS;
	}

	if (stRoleObj.GetAlmsNum() >= pstPrivConfig->aiParams[1])
	{
		//�Ѿ�û�д���
		return T_SERVER_SUCCESS;
	}

	if (stRoleObj.GetAlmsNum() == 0)
	{
		//�״δ���
		UpdateAlmsInfo(stRoleObj, stRoleObj.GetAlmsNum(), iTimeNow);
	}
	else
	{
		//���״δ���,��ʼ����ʱ
		UpdateAlmsInfo(stRoleObj, stRoleObj.GetAlmsNum(), iTimeNow+ pstPrivConfig->aiParams[0]);
	}

	return T_SERVER_SUCCESS;
}
