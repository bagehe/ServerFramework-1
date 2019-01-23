
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Reward/RewardUtility.h"
#include "Vip/VipUtility.h"
#include "FishGame/FishUtility.h"
#include "FishGame/FishpondObj.h"

#include "ResourceUtility.h"

static GameProtocolMsg stMsg;

//���������Դ��iAddNumΪ����ʾ����
bool CResourceUtility::AddUserRes(CGameRoleObj& stRoleObj, int iResType, long8 lAddNum)
{
	if (!stRoleObj.AddResource(iResType, lAddNum))
	{
		return false;
	}

	//������Դ�仯��֪ͨ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_RESOURCECHANGE_NOTIFY);

	Zone_ResourceChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_resourcechange_notify();
	UserResourceChange* pstChangeInfo = pstNotify->add_stchangeinfos();
	pstChangeInfo->set_irestype(iResType);
	pstChangeInfo->set_inewresnum(stRoleObj.GetResource(iResType));

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, &stRoleObj);

	if (stRoleObj.GetTableID() != 0 && (iResType == RESOURCE_TYPE_COIN || iResType == RESOURCE_TYPE_TICKET))
	{
		//����ڲ����У����ͽ����Ʊ���¸����������
		CFishpondObj* pstFishpondObj = CFishUtility::GetFishpondByID(stRoleObj.GetTableID());
		if (pstFishpondObj)
		{
			pstFishpondObj->SendFishUserUpdate(stRoleObj.GetUin(), iResType, lAddNum, 0, 0);
		}
	}

	//�Ƿ񴥷��ȼý�
	if (stRoleObj.GetResource(RESOURCE_TYPE_COIN) == 0)
	{
		//�����ȼý�
		CVipUtility::TriggerAlmsUpdate(stRoleObj);
	}

	return true;
}
