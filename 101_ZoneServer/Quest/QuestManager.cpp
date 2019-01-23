
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Random.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/GameEventManager.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "Resource/ResourceUtility.h"
#include "Reward/RewardUtility.h"

#include "QuestManager.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CQuestManager::CQuestManager()
{
	m_uiUin = 0;

	m_vQuestData.clear();

	//��������������ʱ��
	m_iAdventEndTime = 0;

	//������������õĴ���
	m_iAdventNum = 0;

	//������������´�ˢ��ʱ��
	m_iAdventNextUpdateTime = 0;

	//���ÿ�������´�ˢ��ʱ��
	m_iDailyNextUpdateTime = 0;

	//��������׶�����ˮ
	m_iAdventUserCost = 0;

	//��������׶��ܷ���
	m_iAdventShootNum = 0;

	//����ȡ�Ļ�Ծ�ȱ���ID
	m_vGetLivnessRewards.clear();
}

CQuestManager::~CQuestManager()
{
	m_uiUin = 0;

	m_vQuestData.clear();

	//��������������ʱ��
	m_iAdventEndTime = 0;

	//������������õĴ���
	m_iAdventNum = 0;

	//������������´�ˢ��ʱ��
	m_iAdventNextUpdateTime = 0;

	//���ÿ�������´�ˢ��ʱ��
	m_iDailyNextUpdateTime = 0;

	//��������׶�����ˮ
	m_iAdventUserCost = 0;

	//��������׶��ܷ���
	m_iAdventShootNum = 0;

	//����ȡ�Ļ�Ծ�ȱ���ID
	m_vGetLivnessRewards.clear();
}

//��ʼ��
int CQuestManager::Initialize()
{
	return 0;
}

void CQuestManager::SetOwner(unsigned int uin)
{
	m_uiUin = uin;
}

CGameRoleObj* CQuestManager::GetOwner()
{
	return CUnitUtility::GetRoleByUin(m_uiUin);
}

//�������
int CQuestManager::FinQuest(int iQuestID)
{
	QuestData* pstQuest = GetQuestByID(iQuestID);
	if (!pstQuest || pstQuest->bIsFin)
	{
		//���񲻴��ڻ��������
		LOGERROR("Failed to fin quest, invalid quest id %d, uin %u\n", iQuestID, m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const QuestConfig* pstConfig = stBaseCfgMgr.GetQuestConfig(iQuestID);
	if (!pstConfig)
	{
		//�Ҳ�������
		LOGERROR("Failed to get quest config, invalid quest id %d, uin %u\n", iQuestID, m_uiUin);
		return T_ZONE_INVALID_CFG;
	}

	if (pstQuest->iNum < pstConfig->alParam[3])
	{
		//��δ���
		LOGERROR("Failed to fin quest, need:real %lld:%lld, quest id %d, uin %u\n", pstConfig->alParam[3], pstQuest->iNum, iQuestID, m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_QUESTCHANGE_NOTIFY);
	Zone_QuestChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_questchange_notify();

	//�����������
	int iRet = T_SERVER_SUCCESS;
	switch (pstQuest->iType)
	{
	case QUEST_TYPE_NEW:
	{
		//�����������,ֱ��ɾ��
		DeleteQuest(iQuestID, *pstNotify->add_stchanges());

		//��ȡ����
		iRet = GetQuestReward(iQuestID, pstQuest->iType ,pstConfig->astRewards, sizeof(pstConfig->astRewards)/sizeof(RewardConfig));
		if (iRet)
		{
			LOGERROR("Failed to get quest reward, ret %d, uin %u, quest id %d\n", iRet, m_uiUin, iQuestID);
			return iRet;
		}
	}
	break;

	case QUEST_TYPE_DAILY:
	{
		//���ÿ������
		UpdateQuest(iQuestID, QUEST_CHANGE_FIN, 0, *pstNotify->add_stchanges());

		//��ȡ����
		iRet = GetQuestReward(iQuestID, pstQuest->iType,  pstConfig->astRewards, sizeof(pstConfig->astRewards) / sizeof(RewardConfig));
		if (iRet)
		{
			LOGERROR("Failed to get quest reward, ret %d, uin %u, quest id %d\n", iRet, m_uiUin, iQuestID);
			return iRet;
		}
	}
	break;

	case QUEST_TYPE_ACHIEVE:
	{
		//��ɳɾ�����
		if (pstConfig->iNextQuestID == 0)
		{
			//���һ���ɾ�����
			UpdateQuest(iQuestID, QUEST_CHANGE_FIN, 0, *pstNotify->add_stchanges());
		}
		else
		{
			//�ɾ�������ɶ�Ҫ�̳�
			long8 iAchieveNum = pstQuest->iNum;

			//ɾ���ɾ�����
			DeleteQuest(iQuestID, *pstNotify->add_stchanges());

			//�����һ������
			const QuestConfig* pstNextConfig = stBaseCfgMgr.GetQuestConfig(pstConfig->iNextQuestID);
			if (!pstNextConfig)
			{
				LOGERROR("Failed to get quest config, quest id %d\n", pstConfig->iNextQuestID);
				return T_ZONE_INVALID_CFG;
			}

			AddQuest(pstNextConfig->iID, pstNextConfig->iType, pstNextConfig->iNeedType, *pstNotify->add_stchanges(), iAchieveNum);
		}
		
		//��ȡ����
		iRet = GetQuestReward(iQuestID, pstQuest->iType, pstConfig->astRewards, sizeof(pstConfig->astRewards) / sizeof(RewardConfig));
		if (iRet)
		{
			LOGERROR("Failed to get quest reward, ret %d, uin %u, quest id %d\n", iRet, m_uiUin, iQuestID);
			return iRet;
		}
	}
	break;

	case QUEST_TYPE_ADVENTURE:
	{
		//�����������
		int iGunMultiple = (m_iAdventShootNum == 0) ? 0 : (m_iAdventUserCost/m_iAdventShootNum);

		//ɾ����������
		DeleteQuest(iQuestID, *(pstNotify->add_stchanges()));
		m_iAdventEndTime = 0;
		m_iAdventUserCost = 0;
		m_iAdventShootNum = 0;

		if (pstConfig->iNextQuestID != 0)
		{
			//����һ�׶Σ������µ���������
			const QuestConfig* pstNextConfig = stBaseCfgMgr.GetAdventQuestConfig(pstConfig->iNextQuestID);
			if (!pstNextConfig)
			{
				LOGERROR("Failed to get quest config, quest id %d\n", pstConfig->iNextQuestID);
				return T_ZONE_INVALID_CFG;
			}

			//������������
			AddQuest(pstNextConfig->iID, pstNextConfig->iType, pstNextConfig->iNeedType, *pstNotify->add_stchanges());
			m_iAdventEndTime = CTimeUtility::GetNowTime() + pstNextConfig->iCountdownTime;
		}
		else
		{
			//û����һ�׶�
			++m_iAdventNum;
		}

		//��ȡ����������
		const AdventureRewardConfig* pstRewardConfig = stBaseCfgMgr.GetAdventureRewardConfig(pstConfig->iQuestIndex, iGunMultiple);
		if (!pstRewardConfig)
		{
			LOGERROR("Failed to get adventure reward config, quest index %d, gun multiple %d, uin %u\n", pstConfig->iQuestIndex, iGunMultiple, m_uiUin);
			return T_ZONE_INVALID_CFG;
		}

		//��ȡ����
		iRet = GetQuestReward(iQuestID, pstQuest->iType, pstRewardConfig->astRewards, sizeof(pstRewardConfig->astRewards) / sizeof(RewardConfig));
		if (iRet)
		{
			LOGERROR("Failed to get quest reward, ret %d, uin %u, quest id %d\n", iRet, m_uiUin, iQuestID);
			return iRet;
		}
	}
	break;

	default:
		break;
	}

	pstNotify->set_iadventendtime(m_iAdventEndTime);
	pstNotify->set_iadventnum(m_iAdventNum);

	//��������仯��֪ͨ
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());

	//�������
	CGameEventManager::NotifyFinQuest(*GetOwner(), pstConfig->iType, pstConfig->iID);

	return T_SERVER_SUCCESS;
}

//��ȡ��Ծ�Ƚ���
int CQuestManager::GetLivnessReward(int iRewardID)
{
	//�Ƿ��Ѿ���ȡ��
	if (std::find(m_vGetLivnessRewards.begin(), m_vGetLivnessRewards.end(), iRewardID) != m_vGetLivnessRewards.end())
	{
		//�Ѿ���ȡ��
		LOGERROR("Failed to get livness reward, already get , uin %u, reward id %d\n", m_uiUin, iRewardID);
		return T_ZONE_PARA_ERROR;
	}

	//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const LivnessRewardConfig* pstLivenessConfig = stBaseCfgMgr.GetLivnessRewardConfig(iRewardID);
	if (!pstLivenessConfig)
	{
		LOGERROR("Failed to get liveness config, id %d\n", iRewardID);
		return T_ZONE_INVALID_CFG;
	}

	CGameRoleObj* pstRoleObj = GetOwner();
	if (!pstRoleObj)
	{
		return T_ZONE_PARA_ERROR;
	}

	//��Ծ���Ƿ�����
	int iLivnessNum = pstRoleObj->GetResource(RESOURCE_TYPE_LIVENESS);
	if (iLivnessNum < pstLivenessConfig->iLivnessNum)
	{
		//��Ծ�Ȳ�����
		LOGERROR("Failed to get livness reward, livess num real:need %d:%d, uin %u\n", iLivnessNum, pstLivenessConfig->iLivnessNum, m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	//��ȡ����
	const OpenBoxConfig* pstBoxConfig = stBaseCfgMgr.GetOpenBoxConfig(pstLivenessConfig->iBoxItemID);
	if (!pstBoxConfig)
	{
		LOGERROR("Failed to get open box config, item id %d\n", pstLivenessConfig->iBoxItemID);
		return T_ZONE_INVALID_CFG;
	}

	int iRet = CRewardUtility::GetReward(*pstRoleObj, 1, pstBoxConfig->astRewards, sizeof(pstBoxConfig->astRewards) / sizeof(RewardConfig), ITEM_CHANNEL_QUEST);
	if (iRet)
	{
		LOGERROR("Failed to add open box item, ret %d, uin %u, box id %d\n", iRet, m_uiUin, pstBoxConfig->iID);
		return iRet;
	}

	//��ӡ��Ӫ��־ ��Ծ�ȱ���
	for (unsigned i = 0; i < sizeof(pstBoxConfig->astRewards) / sizeof(RewardConfig); ++i)
	{
		//����Ծ�ȱ���֮ǰ��Ӧ ���������
		long8 lOldNum = 0;

		const RewardConfig& stRewardConfig = pstBoxConfig->astRewards[i];
		switch (stRewardConfig.iType)
		{
		case REWARD_TYPE_RES:
		{
			lOldNum = pstRoleObj->GetResource(stRewardConfig.iType);
		}
		break;

		case REWARD_TYPE_ITEM:
		{
			lOldNum = pstRoleObj->GetRepThingsManager().GetRepItemNum(stRewardConfig.iType);
		}
		break;

		default:
			break;
		}

		CZoneOssLog::TraceLiveness(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), pstBoxConfig->iID, stRewardConfig.iType,
			stRewardConfig.iRewardID, lOldNum, lOldNum + stRewardConfig.iRewardNum);
	}
	

	//���ӵ�������б�
	m_vGetLivnessRewards.push_back(iRewardID);

	return T_SERVER_SUCCESS;
}

//������������
int CQuestManager::OnQuestNeedChange(int iNeedType, int iParam1, int iParam2, int iParam3, int iParam4, int iExtraParam)
{
	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_QUESTCHANGE_NOTIFY);
	Zone_QuestChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_questchange_notify();

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	for (unsigned i = 0; i < m_vQuestData.size(); ++i)
	{
		if (m_vQuestData[i].bIsFin || m_vQuestData[i].iNeedType != iNeedType)
		{
			continue;
		}

		//�Ƕ�Ӧ����
		const QuestConfig* pstQuestConfig = stBaseCfgMgr.GetQuestConfig(m_vQuestData[i].iQuestID);
		if (!pstQuestConfig)
		{
			LOGERROR("Failed to get quest config, invalid quest id %d, uin %u\n", m_vQuestData[i].iQuestID, m_uiUin);
			return T_ZONE_INVALID_CFG;
		}

		//�ж��Ƿ���������
		switch (iNeedType)
		{
		case QUEST_NEED_KILLFISH:
		{
			//��������
			if (pstQuestConfig->alParam[0] != 0 && (pstQuestConfig->alParam[0] & iParam1) == 0)
			{
				//���Ƕ�Ӧ����ģʽ
				continue;
			}

			if (pstQuestConfig->alParam[1] != 0 && pstQuestConfig->alParam[1] != iParam2)
			{
				//���Ƕ�Ӧ����ID
				continue;
			}

			if (pstQuestConfig->alParam[2] > 100 && pstQuestConfig->alParam[2] != iParam3)
			{
				//��ID��ƥ��
				continue;
			}
			else if (pstQuestConfig->alParam[2] <= 100 && pstQuestConfig->alParam[2] > 0 && iExtraParam!=pstQuestConfig->alParam[2])
			{
				//�����Ͳ�ƥ��
				continue;
			}
		}
		break;

		case QUEST_NEED_GETFISHRES:
		case QUEST_NEED_USESKILL:
		case QUEST_NEED_FINQUEST:
		case QUEST_NEED_GETITEM:
		{
			if ((pstQuestConfig->alParam[0] != 0 && (pstQuestConfig->alParam[0] & iParam1) == 0) ||
				(pstQuestConfig->alParam[1] != 0 && pstQuestConfig->alParam[1] != iParam2) ||
				(pstQuestConfig->alParam[2] != 0 && pstQuestConfig->alParam[2] != iParam3))
			{
				//���Ƕ�Ӧ����ģʽ
				continue;
			}
		}
		break;

		case QUEST_NEED_CHANGEOPERA:
		case QUEST_NEED_LOTTERY:
		case QUEST_NEED_LOGINDAY:
		case QUEST_NEED_ONLINETIME:
		{
			if ((pstQuestConfig->alParam[0] != 0 && pstQuestConfig->alParam[0] != iParam1) ||
				(pstQuestConfig->alParam[1] != 0 && pstQuestConfig->alParam[1] != iParam2) ||
				(pstQuestConfig->alParam[2] != 0 && pstQuestConfig->alParam[2] != iParam3))
			{
				//���Ƕ�Ӧ����ģʽ
				continue;
			}
		}

		default:
			break;
		}

		//��������
		UpdateQuest(m_vQuestData[i].iQuestID, QUEST_CHANGE_UPDATE, iParam4, *pstNotify->add_stchanges());
	}

	if (pstNotify->stchanges_size() != 0)
	{
		pstNotify->set_iadventendtime(m_iAdventEndTime);
		pstNotify->set_iadventnum(m_iAdventNum);

		CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());
	}

	return T_SERVER_SUCCESS;
}

//�����ӵ�֪ͨ
void CQuestManager::OnShootBullet(int iGunID, int iCost)
{
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	int iDayAdventureNum = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ADVENTURENUM);
	if (m_iAdventNum >= iDayAdventureNum)
	{
		//�������������������
		return;
	}

	//���Ӽ���
	m_iAdventShootNum += 1;
	m_iAdventUserCost += iCost;

	if (m_iAdventEndTime != 0)
	{
		//��ǰ����������
		return;
	}

	//������ȡ����������
	if (m_iAdventNum == 0)
	{
		int iBulletMin = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ADVENTBULLETMIN);
		int iBulletMax = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ADVENTBULLETMAX);

		if (m_iAdventShootNum < iBulletMin || m_iAdventShootNum >= CRandomCalculator::GetRandomNumber(iBulletMin, iBulletMax))
		{
			return;
		}
	}
	else
	{
		//�ڶ�����������
		if (CRandomCalculator::GetRandomInRangeTenThousand() >= stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_SECONDADVENTRATE))
		{
			return;
		}
	}

	//��ȡ��������
	const QuestConfig* pstNextConfig = stBaseCfgMgr.GetAdventQuestConfig(START_ADVENTURE_QUEST);
	if (!pstNextConfig)
	{
		LOGERROR("Failed to get adventure quest config, adventure quest id %d\n", START_ADVENTURE_QUEST);
		return;
	}

	//������������
	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_QUESTCHANGE_NOTIFY);
	Zone_QuestChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_questchange_notify();
	AddQuest(pstNextConfig->iID, pstNextConfig->iType, pstNextConfig->iNeedType, *pstNotify->add_stchanges());
	m_iAdventEndTime = CTimeUtility::GetNowTime() + pstNextConfig->iCountdownTime;

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());

	return;
}

//��ʱ��
void CQuestManager::OnTick(int iTimeNow)
{
	if (m_iAdventEndTime > iTimeNow && m_iAdventNextUpdateTime > iTimeNow && m_iDailyNextUpdateTime > iTimeNow)
	{
		//����Ҫ����
		return;
	}

	ResetQuest(false);

	return;
}

//��������DB
void CQuestManager::UpdateQuestToDB(QUESTDBINFO& stQuestDBInfo)
{
	stQuestDBInfo.Clear();

	//��������
	for (unsigned i = 0; i < m_vQuestData.size(); ++i)
	{
		OneQuest* pstQuestInfo = stQuestDBInfo.add_stquestinfos();

		pstQuestInfo->set_iquestid(m_vQuestData[i].iQuestID);
		pstQuestInfo->set_iquesttype(m_vQuestData[i].iType);
		pstQuestInfo->set_ineedtype(m_vQuestData[i].iNeedType);
		pstQuestInfo->set_inum(m_vQuestData[i].iNum);
		pstQuestInfo->set_bisfin(m_vQuestData[i].bIsFin);
	}

	//��������ˢ������
	stQuestDBInfo.set_iadventureendtime(m_iAdventEndTime);
	stQuestDBInfo.set_iadventurenum(m_iAdventNum);
	stQuestDBInfo.set_iadventnextupdatetime(m_iAdventNextUpdateTime);
	stQuestDBInfo.set_iadventusercost(m_iAdventUserCost);
	stQuestDBInfo.set_iadventshootnum(m_iAdventShootNum);

	//ÿ������ˢ������
	stQuestDBInfo.set_idailynextupdatetime(m_iDailyNextUpdateTime);

	//����ȡ��Ծ�Ƚ���ID
	for (unsigned i = 0; i < m_vGetLivnessRewards.size(); ++i)
	{
		stQuestDBInfo.add_igetliverewardids(m_vGetLivnessRewards[i]);
	}

	return;
}

//��DB��ʼ������
void CQuestManager::InitQuestFromDB(const QUESTDBINFO& stQuestDBInfo)
{
	//��������
	m_vQuestData.clear();

	QuestData stQuest;
	for (int i = 0; i < stQuestDBInfo.stquestinfos().size(); ++i)
	{
		stQuest.Reset();

		stQuest.iQuestID = stQuestDBInfo.stquestinfos(i).iquestid();
		stQuest.iType = stQuestDBInfo.stquestinfos(i).iquesttype();
		stQuest.iNeedType = stQuestDBInfo.stquestinfos(i).ineedtype();
		stQuest.iNum = stQuestDBInfo.stquestinfos(i).inum();
		stQuest.bIsFin = stQuestDBInfo.stquestinfos(i).bisfin();

		m_vQuestData.push_back(stQuest);
	}

	//��������ˢ������
	m_iAdventEndTime = stQuestDBInfo.iadventureendtime();
	m_iAdventNum = stQuestDBInfo.iadventurenum();
	m_iAdventNextUpdateTime = stQuestDBInfo.iadventnextupdatetime();
	m_iAdventUserCost = stQuestDBInfo.iadventusercost();
	m_iAdventShootNum = stQuestDBInfo.iadventshootnum();

	//ÿ������ˢ������
	m_iDailyNextUpdateTime = stQuestDBInfo.idailynextupdatetime();

	//����ȡ��Ծ�Ƚ�������
	m_vGetLivnessRewards.clear();
	for (int i = 0; i < stQuestDBInfo.igetliverewardids_size(); ++i)
	{
		m_vGetLivnessRewards.push_back(stQuestDBInfo.igetliverewardids(i));
	}

	ResetQuest(true);

	return;
}

//�������
void CQuestManager::AddQuest(int iQuestID, int iType, int iNeedType, QuestChange& stChangeInfo, long8 iNum)
{
	QuestData stData;
	stData.iQuestID = iQuestID;
	stData.iType = iType;
	stData.iNeedType = iNeedType;
	stData.iNum = iNum;
	stData.bIsFin = false;

	m_vQuestData.push_back(stData);

	stChangeInfo.set_iquestid(iQuestID);
	stChangeInfo.set_ichangetype(QUEST_CHANGE_ADD);
	stChangeInfo.set_inum(iNum);

	return;
}

//��������
void CQuestManager::UpdateQuest(int iQuestID, int iChangeType, int iAddNum, QuestChange& stChangeInfo)
{
	for (unsigned i = 0; i < m_vQuestData.size(); ++i)
	{
		if (m_vQuestData[i].iQuestID != iQuestID)
		{
			continue;
		}

		m_vQuestData[i].iNum += iAddNum;
		if (iChangeType == QUEST_CHANGE_FIN)
		{
			m_vQuestData[i].bIsFin = true;
		}

		stChangeInfo.set_iquestid(iQuestID);
		stChangeInfo.set_ichangetype(iChangeType);
		stChangeInfo.set_inum(m_vQuestData[i].iNum);

		break;
	}

	return;
}

//ɾ������
void CQuestManager::DeleteQuest(int iQuestID, QuestChange& stChangeInfo)
{
	for (unsigned i = 0; i < m_vQuestData.size(); ++i)
	{
		if (m_vQuestData[i].iQuestID == iQuestID)
		{
			m_vQuestData.erase(m_vQuestData.begin()+i);
			break;
		}
	}

	stChangeInfo.set_iquestid(iQuestID);
	stChangeInfo.set_ichangetype(QUEST_CHANGE_DELETE);

	return;
}

//��ȡ����
QuestData* CQuestManager::GetQuestByID(int iQuestID)
{
	for (unsigned i = 0; i < m_vQuestData.size(); ++i)
	{
		if (m_vQuestData[i].iQuestID == iQuestID)
		{
			return &m_vQuestData[i];
		}
	}

	return NULL;
}

//��ȡ������
int CQuestManager::GetQuestReward(int iQuestID, int iQuestType, const RewardConfig* pstRewardConfig, int iNum)
{
	CGameRoleObj* pstRoleObj = GetOwner();

	if (!pstRewardConfig || !pstRoleObj)
	{
		return T_ZONE_PARA_ERROR;
	}

	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_GETREWARD_NOTIFY);
	Zone_GetReward_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_getreward_notify();
	pstNotify->set_iquestid(iQuestID);

	int iRet = T_SERVER_SUCCESS;
	for (int i = 0; i < iNum; ++i)
	{
		switch (pstRewardConfig[i].iType)
		{
		case REWARD_TYPE_ITEM:
		{
			//����
			iRet = CRepThingsUtility::AddItemNum(*pstRoleObj, pstRewardConfig[i].iRewardID, pstRewardConfig[i].iRewardNum, ITEM_CHANNEL_QUEST);
			if (iRet)
			{
				LOGERROR("Failed to get reward, ret %d, uin %u, quest id %d, reward id %d\n", iRet, m_uiUin, iQuestID, pstRewardConfig[i].iRewardID);
				return iRet;
			}

			//��ӡ��Ӫ��־
			long8 lNewNum = pstRoleObj->GetRepThingsManager().GetRepItemNum(pstRewardConfig[i].iRewardID);
			CZoneOssLog::TraceQuest(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), iQuestID, iQuestType, pstRewardConfig[i].iType, pstRewardConfig[i].iRewardID,
				lNewNum - pstRewardConfig[i].iRewardNum, lNewNum);

			RewardInfo* pstReward = pstNotify->add_strewards();
			pstReward->set_itype(pstRewardConfig[i].iType);
			pstReward->set_iid(pstRewardConfig[i].iRewardID);
			pstReward->set_inum(pstRewardConfig[i].iRewardNum);
		}
		break;

		case REWARD_TYPE_RES:
		{
			//��Դ
			if(!CResourceUtility::AddUserRes(*pstRoleObj, pstRewardConfig[i].iRewardID, pstRewardConfig[i].iRewardNum))
			{
				LOGERROR("Failed to get reward, ret %d, uin %u, quest id %d, reward id %d\n", iRet, m_uiUin, iQuestID, pstRewardConfig[i].iRewardID);
				return T_ZONE_INVALID_CFG;
			}

			//��ӡ��Ӫ��־
			long8 lNewResNum = pstRoleObj->GetResource(pstRewardConfig[i].iRewardID);
			CZoneOssLog::TraceQuest(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), iQuestID, iQuestType,  pstRewardConfig[i].iType, pstRewardConfig[i].iRewardID,
				lNewResNum - pstRewardConfig[i].iRewardNum, lNewResNum);

			RewardInfo* pstReward = pstNotify->add_strewards();
			pstReward->set_itype(pstRewardConfig[i].iType);
			pstReward->set_iid(pstRewardConfig[i].iRewardID);
			pstReward->set_inum(pstRewardConfig[i].iRewardNum);
		}
		break;

		default:
			break;
		}
	}

	if (pstNotify->strewards_size() != 0)
	{
		//����֪ͨ
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());
	}

	return T_SERVER_SUCCESS;
}

//��������
void CQuestManager::ResetQuest(bool bIsInit)
{
	int iTimeNow = CTimeUtility::GetNowTime();
	int iNextDayNowTime = iTimeNow + 24 * 60 * 60;

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_QUESTCHANGE_NOTIFY);
	Zone_QuestChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_questchange_notify();

	bool bSendNotify = false;

	//�ճ�����
	if (m_iDailyNextUpdateTime <= iTimeNow)
	{
		//�����ճ�����
		bSendNotify = true;
		m_iDailyNextUpdateTime = CTimeUtility::GetTodayTime(iNextDayNowTime, stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_DAILYRESETTIME));

		//ˢ���ճ�����
		for (unsigned i = 0; i < m_vQuestData.size(); ++i)
		{
			if (m_vQuestData[i].iType != QUEST_TYPE_DAILY)
			{
				continue;
			}

			m_vQuestData[i].bIsFin = false;
			m_vQuestData[i].iNum = 0;

			QuestChange* pstOneChange = pstNotify->add_stchanges();
			pstOneChange->set_iquestid(m_vQuestData[i].iQuestID);
			pstOneChange->set_inum(m_vQuestData[i].iNum);
			pstOneChange->set_ichangetype(QUEST_CHANGE_UPDATE);
		}

		//ˢ�»�Ծ�ȱ���
		m_vGetLivnessRewards.clear();

		//���û�Ծ��
		CResourceUtility::AddUserRes(*GetOwner(), RESOURCE_TYPE_LIVENESS, -GetOwner()->GetResource(RESOURCE_TYPE_LIVENESS));
	}

	//��������
	if (m_iAdventNextUpdateTime <= iTimeNow)
	{
		//���������������
		bSendNotify = true;
		m_iAdventNextUpdateTime = CTimeUtility::GetTodayTime(iNextDayNowTime, stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ADVENTRESETTIME));

		//ˢ�������������
		m_iAdventNum = 0;
	}

	if (m_iAdventEndTime!=0 && m_iAdventEndTime<=iTimeNow)
	{
		//������������
		bSendNotify = true;
		m_iAdventEndTime = 0;
		m_iAdventUserCost = 0;
		m_iAdventShootNum = 0;

		for (unsigned i = 0; i < m_vQuestData.size(); ++i)
		{
			if (m_vQuestData[i].iType != QUEST_TYPE_ADVENTURE)
			{
				continue;
			}

			//ɾ����������
			DeleteQuest(m_vQuestData[i].iQuestID, *pstNotify->add_stchanges());
			break;
		}
	}

	if (!bIsInit && bSendNotify)
	{
		//��Ҫ����֪ͨ
		pstNotify->set_iadventnum(m_iAdventNum);
		pstNotify->set_iadventendtime(m_iAdventEndTime);

		CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());
	}

	return;
}
