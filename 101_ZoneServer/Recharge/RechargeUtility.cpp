
#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneMsgHelper.hpp"

#include "RechargeUtility.h"

static GameProtocolMsg stMsg;

//�����¿�ʱ��, iAddTime������
void CRechargeUtility::AddMonthEndTime(CGameRoleObj& stRoleObj, int iAddTime)
{
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_UPDATEMONTHTIME_NOTIFY);
	Zone_UpdateMonthTime_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_updatemonthtime_notify();

	//�����¿�ʱ��
	int iTimeNow = CTimeUtility::GetNowTime();
	int iMonthEndTime = 0;
	if (iTimeNow <= stRoleObj.GetMonthEndTime())
	{
		//ֱ�Ӽ���ȥ
		iMonthEndTime = stRoleObj.GetMonthEndTime() + iAddTime * 24 * 60 * 60;
	}
	else
	{
		//���¼���ʱ��
		iMonthEndTime = CTimeUtility::GetTodayTime(iTimeNow) + iAddTime * 24 * 60 * 60;
	}

	stRoleObj.SetMonthEndTime(iMonthEndTime);
	stRoleObj.SetLastMonthTime(iTimeNow-24*60*60);	//�����ϴ������ȡʱ��Ϊ����

	pstNotify->set_imonthendtime(stRoleObj.GetMonthEndTime());

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, &stRoleObj);

	return;
}
