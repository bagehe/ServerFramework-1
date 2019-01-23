
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/GameEventManager.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Reward/RewardUtility.h"
#include "FishGame/FishpondObj.h"
#include "FishGame/FishUtility.h"
#include "Mail/MailUtility.h"

#include "RepThingsManager.hpp"
#include "RepThingsUtility.hpp"

//��ɾ��Ʒ�Ľӿ�,���iAddNumС��0��ʾɾ��
int CRepThingsUtility::AddItemNum(CGameRoleObj& stRoleObj, int iItemID, int iAddNum, int iItemChannel)
{
	if (iAddNum == 0)
	{
		return 0;
	}

	//֪ͨ��õ���
	const FishItemConfig* pstItemConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetFishItemConfig(iItemID);
	if (!pstItemConfig)
	{
		LOGERROR("Failed to get fish item config, item id %d\n", iItemID);
		return T_ZONE_INVALID_CFG;
	}

    //��ȡ������Ʒ������
    CRepThingsManager& rstThingsManager = stRoleObj.GetRepThingsManager();

	if (iAddNum < 0)
	{
		//����
		return rstThingsManager.DeleteRepItem(iItemID, -iAddNum, iItemChannel);
	}
	else
	{
		//����
		int iRet = rstThingsManager.AddRepItem(iItemID, iAddNum, iItemChannel);
		if (iRet)
		{
			//�������ӣ����Է����ʼ�
			iRet = CMailUtility::SendRepFullMail(stRoleObj, iItemID, iAddNum);
			if (iRet)
			{
				return iRet;
			}
		}

		CGameEventManager::NotifyGetItem(stRoleObj, pstItemConfig->iType, iItemID, iAddNum);
	}

	if (stRoleObj.GetTableID() != 0 && pstItemConfig->iType == FISH_ITEM_WARHEAD)
	{
		//�����е�ͷ�仯
		CFishpondObj* pstFishpondObj = CFishUtility::GetFishpondByID(stRoleObj.GetTableID());
		if (pstFishpondObj)
		{
			pstFishpondObj->SendFishUserUpdate(stRoleObj.GetUin(), 0, 0, iItemID, iAddNum);
		}
	}

	//����������Ϣ
	TROLETALLYINFO& stTallyInfo = stRoleObj.GetTallyInfo();
	if (iAddNum > 0)
	{
		//��õ���
		stTallyInfo.lCashTicketNum += pstItemConfig->iTicketValue;
	}

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	int aiNormalWarheadID[WARHEAD_TYPE_MAX] = { 0 };
	aiNormalWarheadID[WARHEAD_TYPE_BRONZE] = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NORMALBRONZE);
	aiNormalWarheadID[WARHEAD_TYPE_SILVER] = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NORMALSILVER);
	aiNormalWarheadID[WARHEAD_TYPE_GOLD] = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NORMALGOLD);

	for (int i = (WARHEAD_TYPE_INVALID + 1); i < WARHEAD_TYPE_MAX; ++i)
	{
		if (iItemID != aiNormalWarheadID[i])
		{
			continue;
		}

		//�Ƕ�Ӧ��ͷ
		if (iItemChannel == ITEM_CHANNEL_SENDGIFT)
		{
			//����
			stTallyInfo.aiSendGiftNum[i] += -iAddNum;
		}
		else if (iItemChannel == ITEM_CHANNEL_ROLEMAIL)
		{
			//��ȡ�����ʼ�
			stTallyInfo.aiRecvGiftNum[i] += iAddNum;
		}

		break;
	}

    return 0;
}

//������
int CRepThingsUtility::OpenBoxGift(CGameRoleObj& stRoleObj, int iBoxID, int iBoxNum, bool bDeleteItem)
{
	//��������
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const OpenBoxConfig* pstBoxConfig = stBaseCfgMgr.GetOpenBoxConfig(iBoxID);
	if (!pstBoxConfig)
	{
		LOGERROR("Failed to get open box config, item id %d\n", iBoxID);
		return T_ZONE_INVALID_CFG;
	}

	int iRet = T_SERVER_SUCCESS;
	if (bDeleteItem)
	{
		//�۳�����
		iRet = CRepThingsUtility::AddItemNum(stRoleObj, iBoxID, -iBoxNum, ITEM_CHANNEL_OPENBOX);
		if (iRet)
		{
			LOGERROR("Failed to open rep item, uin %u, item id %d, num %d, ret %d\n", stRoleObj.GetUin(), iBoxID, iBoxNum, iRet);
			return iRet;
		}
	}

	iRet = CRewardUtility::GetReward(stRoleObj, iBoxNum, pstBoxConfig->astRewards, sizeof(pstBoxConfig->astRewards) / sizeof(RewardConfig), ITEM_CHANNEL_OPENBOX);
	if (iRet)
	{
		LOGERROR("Failed to add open box item, ret %d, uin %u, box id %d\n", iRet, stRoleObj.GetUin(), iBoxID);
		return iRet;
	}

	//��ӡ��Ӫ��־  ʹ�õ��� һ�δ����б��䣬���������ͷ�����¼
	//��Ӧ����ʹ�ñ�����Ӧ��Ŀ
	CRepThingsManager& stThingsMgr = stRoleObj.GetRepThingsManager();
	long8 lNewBoxNum = stThingsMgr.GetRepItemNum(iBoxID);
	for (int j = 0; j < iBoxNum; ++j)
	{
		for (unsigned i = 0; i < sizeof(pstBoxConfig->astRewards) / sizeof(RewardConfig); ++i)
		{
			const RewardConfig& stConfig = pstBoxConfig->astRewards[i];
			//��ȡ֮ǰ������
			long8 lNewRewardNum = 0;
			switch (stConfig.iType)
			{
			case REWARD_TYPE_RES:
			{
				lNewRewardNum = stRoleObj.GetResource(stConfig.iRewardID);
			}
			break;

			case REWARD_TYPE_ITEM:
			{
				lNewRewardNum = stThingsMgr.GetRepItemNum(stConfig.iRewardID);
			}
			break;

			default:
				break;
			}
			CZoneOssLog::TraceUseItem(stRoleObj.GetUin(), stRoleObj.GetChannel(), stRoleObj.GetNickName(), iBoxID, lNewBoxNum - iBoxNum, lNewBoxNum,
				stConfig.iRewardID, lNewRewardNum - stConfig.iRewardNum, lNewRewardNum);
		}
	}
	

	return T_SERVER_SUCCESS;
}

//����ʧ�ܣ��������߸����
void CRepThingsUtility::OnSendGiftFailed(unsigned uiUin, unsigned uiToUin, int iItemID, int iItemNum, int iErrorNum)
{
	//��������
	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(uiUin);
	if (!pstRoleObj)
	{
		LOGERROR("Failed to return gift item, uin %u, item id:num %d:%d\n", uiUin, iItemID, iItemNum);
		return;
	}

	int iRet = CRepThingsUtility::AddItemNum(*pstRoleObj, iItemID, iItemNum, ITEM_CHANNEL_SENDGIFT);
	if (iRet)
	{
		LOGERROR("Failed to return gift item, uin %u, item id:num %d:%d\n", uiUin, iItemID, iItemNum);
		return;
	}

	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REPOPERA_RESPONSE);
	
	Zone_RepOpera_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_repopera_response();
	pstResp->set_etype(REQ_OPERA_TYPE_SENDGIFT);
	pstResp->set_utouin(uiToUin);
	pstResp->set_iitemid(iItemID);
	pstResp->set_iitemnum(iItemNum);
	pstResp->set_iresult(iErrorNum);

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	return;
}
