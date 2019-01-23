
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "TimeUtility.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"

#include "RankInfoManager.h"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRankInfoManager* CRankInfoManager::Instance()
{
	static CRankInfoManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CRankInfoManager;
	}

	return pInstance;
}

CRankInfoManager::~CRankInfoManager()
{

}

CRankInfoManager::CRankInfoManager()
{
	Reset();
}

//��ʼ��
void CRankInfoManager::Init()
{
	//��ȡ������Ϣ
	m_iLastUpdateTime = CTimeUtility::GetNowTime();

	for (int i = RANK_TYPE_INVALID + 1; i < RANK_TYPE_MAX; ++i)
	{
		GetRankInfoFromWorld(i);
	}

	return;
}

//��ȡ������Ϣ
int CRankInfoManager::GetRankInfo(unsigned uin, int iType, int iFromRank, int iNum, bool bLastRank, Zone_GetRankInfo_Response& stResp)
{
	if (iType <= RANK_TYPE_INVALID || iType >= RANK_TYPE_MAX)
	{
		LOGERROR("Failed to get rank info, invalid type %d\n", iType);
		return T_ZONE_PARA_ERROR;
	}

	if (iFromRank <= 0 || iFromRank > MAX_RANK_INFO_NUM || iNum <= 0)
	{
		LOGERROR("Failed to get rank info, from %d, num %d\n", iFromRank, iNum);
		return T_ZONE_PARA_ERROR;
	}

	RankList& stOneList = m_astRankLists[iType];
	if (bLastRank)
	{
		PackRankInfo(uin, iFromRank, iNum, stOneList.vLastRankInfos, stResp);
	}
	else
	{
		PackRankInfo(uin, iFromRank, iNum, stOneList.vRankInfos, stResp);
	}

	return T_SERVER_SUCCESS;
}

//����������Ϣ
void CRankInfoManager::UpdateRoleRank(CGameRoleObj& stRoleObj)
{
	RankData stData;
	stData.uiUin = stRoleObj.GetUin();
	stData.strNickName = stRoleObj.GetNickName();
	stData.strPicID = stRoleObj.GetPicID();
	stData.iVIPLevel = stRoleObj.GetVIPLevel();

	//ˢ�¸�����ܻ���
	stRoleObj.AddFishScore(0);

	//���½������
	stData.iNum = stRoleObj.GetResource(RESOURCE_TYPE_COIN);
	UpdateRankByType(RANK_TYPE_MONEY, stData, stRoleObj.GetUpdateRank());

	//���»ƽ�ͷ����
	stData.iNum = stRoleObj.GetRepThingsManager().GetRepItemNum(GOLD_WARHEAD_ID);
	if (IsInRank(RANK_TYPE_WARHEAD, stData) || stData.iNum >= WARHEAD_INRANK_NUM)
	{
		UpdateRankByType(RANK_TYPE_WARHEAD, stData, stRoleObj.GetUpdateRank());
	}

	//�����ջ�������
	if (stRoleObj.GetScoreInfo().lDayScore > 0)
	{
		stData.iNum = stRoleObj.GetScoreInfo().lDayScore;
		UpdateRankByType(RANK_TYPE_DAYSCORE, stData, stRoleObj.GetUpdateRank());
	}

	//�����ܻ�������
	if (stRoleObj.GetScoreInfo().lWeekScore > 0)
	{
		stData.iNum = stRoleObj.GetScoreInfo().lWeekScore;
		UpdateRankByType(RANK_TYPE_WEEKSCORE, stData, stRoleObj.GetUpdateRank());
	}

	//����ǿ�Ƹ���Ϊfalse
	stRoleObj.SetUpdateRank(false);

	return;
}

//����������Ϣ
int CRankInfoManager::SetRankListInfo(const World_GetRankInfo_Response& stResp)
{
	if (stResp.itype() <= RANK_TYPE_INVALID || stResp.itype() >= RANK_TYPE_MAX)
	{
		return T_ZONE_PARA_ERROR;
	}

	RankList& stOneList = m_astRankLists[stResp.itype()];

	if (stResp.iversionid() <= stOneList.uVersionID)
	{
		//�Ѿ�����������
		return T_SERVER_SUCCESS;
	}

	//������������
	stOneList.uVersionID = stResp.iversionid();
	stOneList.iLastUpdateTime = stResp.ilastupdate();
	
	RankData stOneData;

	//����������
	stOneList.vLastRankInfos.clear();
	for (int i = 0; i < stResp.stlastranks_size(); ++i)
	{
		stOneData.uiUin = stResp.stlastranks(i).uin();
		stOneData.strNickName = stResp.stlastranks(i).sznickname();
		stOneData.strPicID = stResp.stlastranks(i).strpicid();
		stOneData.iVIPLevel = stResp.stlastranks(i).iviplevel();
		stOneData.iNum = stResp.stlastranks(i).inum();
		stOneData.strSign = stResp.stlastranks(i).strsign();

		stOneList.vLastRankInfos.push_back(stOneData);
	}

	//����������
	stOneList.vRankInfos.clear();
	for (int i = 0; i < stResp.stranks_size(); ++i)
	{
		stOneData.uiUin = stResp.stranks(i).uin();
		stOneData.strNickName = stResp.stranks(i).sznickname();
		stOneData.strPicID = stResp.stranks(i).strpicid();
		stOneData.iVIPLevel = stResp.stranks(i).iviplevel();
		stOneData.iNum = stResp.stranks(i).inum();
		stOneData.strSign = stResp.stranks(i).strsign();

		stOneList.vRankInfos.push_back(stOneData);
	}

	return T_SERVER_SUCCESS;
}

//��ʱ��
void CRankInfoManager::OnTick()
{
	int iTimeNow = CTimeUtility::GetNowTime();

	//20s����һ��
	if ((iTimeNow - m_iLastUpdateTime) < 20)
	{
		return;
	}

	m_iLastUpdateTime = iTimeNow;

	for (int i = RANK_TYPE_INVALID + 1; i < RANK_TYPE_MAX; ++i)
	{
		GetRankInfoFromWorld(i);
	}

	return;
}

//����������Ϣ
void CRankInfoManager::UpdateRankByType(int iType, const RankData& stData, bool bUpdateRank)
{
	//У�����
	if (iType <= RANK_TYPE_INVALID || iType >= RANK_TYPE_MAX)
	{
		return;
	}

	RankList& stOneList = m_astRankLists[iType];

	//�Ƿ���������
	bool bIsInRank = false;
	std::vector<RankData>::iterator it = std::find(stOneList.vRankInfos.begin(), stOneList.vRankInfos.end(), stData);
	if (it != stOneList.vRankInfos.end())
	{
		bIsInRank = true;

		if (it->iNum == stData.iNum && !bUpdateRank)
		{
			//�������в��Ҳ���Ҫǿ�Ƹ���
			return;
		}
	}

	//���������в��Ҳ��ܽ�������
	if(!bIsInRank && stOneList.vRankInfos.size() >= (int)MAX_RANK_INFO_NUM && 
		stOneList.vRankInfos[MAX_RANK_INFO_NUM - 1].iNum >= stData.iNum && 
		CTimeUtility::IsInSameDay(stOneList.iLastUpdateTime, CTimeUtility::GetNowTime()))
	{
		return;
	}

	//����World��������
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_UPDATERANK_REQUEST);
	World_UpdateRank_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_updaterank_request();
	pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());

	UpdateRankInfo* pstRankInfo = pstReq->add_stupdateranks();
	pstRankInfo->set_iranktype(iType);
	pstRankInfo->mutable_strank()->set_uin(stData.uiUin);
	pstRankInfo->mutable_strank()->set_sznickname(stData.strNickName);
	pstRankInfo->mutable_strank()->set_strpicid(stData.strPicID);
	pstRankInfo->mutable_strank()->set_iviplevel(stData.iVIPLevel);
	pstRankInfo->mutable_strank()->set_inum(stData.iNum);
	pstRankInfo->mutable_strank()->set_strsign(stData.strSign);

	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);

	return;
}

//��ȡ���а���Ϣ
void CRankInfoManager::GetRankInfoFromWorld(int iType)
{
	if (iType <= RANK_TYPE_INVALID || iType >= RANK_TYPE_MAX)
	{
		return;
	}

	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_WORLD_GETRANKINFO_REQUEST);
	World_GetRankInfo_Request* pstReq = stMsg.mutable_stbody()->mutable_m_stworld_getrankinfo_request();
	pstReq->set_ifromzoneid(CModuleHelper::GetZoneID());
	pstReq->set_itype(iType);
	pstReq->set_iversionid(m_astRankLists[iType].uVersionID);

	CZoneMsgHelper::SendZoneMsgToWorld(stMsg);
}

//���������Ϣ
void CRankInfoManager::PackRankInfo(unsigned uin, int iFromRank, int iNum, const std::vector<RankData>& vRankDatas, Zone_GetRankInfo_Response& stResp)
{
	if (vRankDatas.size() == 0)
	{
		//û������
		return;
	}

	for (int i = 0; i < iNum; ++i)
	{
		if ((iFromRank + i) >(int)vRankDatas.size())
		{
			break;
		}

		//��װ����
		RankInfo* pstOneInfo = stResp.add_stranks();
		pstOneInfo->set_uin(vRankDatas[iFromRank + i - 1].uiUin);
		pstOneInfo->set_sznickname(vRankDatas[iFromRank + i - 1].strNickName);
		pstOneInfo->set_strpicid(vRankDatas[iFromRank + i - 1].strPicID);
		pstOneInfo->set_iviplevel(vRankDatas[iFromRank + i - 1].iVIPLevel);
		pstOneInfo->set_inum(vRankDatas[iFromRank + i - 1].iNum);
		pstOneInfo->set_strsign(vRankDatas[iFromRank + i - 1].strSign);
		pstOneInfo->set_iranknum(iFromRank + i);
	}

	//��ȡ�Լ�������
	for (unsigned i = 0; i < vRankDatas.size(); ++i)
	{
		if (vRankDatas[i].uiUin == uin)
		{
			//��������
			stResp.set_imyrank(i+1);
			break;
		}
	}

	return;
}

//�Ƿ���������
bool CRankInfoManager::IsInRank(int iType, const RankData& stData)
{
	if (iType <= RANK_TYPE_INVALID || iType >= RANK_TYPE_MAX)
	{
		return false;
	}

	for (unsigned i = 0; i < m_astRankLists[iType].vRankInfos.size(); ++i)
	{
		if (m_astRankLists[iType].vRankInfos[i].uiUin == stData.uiUin)
		{
			//�����а���
			return true;
		}
	}

	return false;
}

void CRankInfoManager::Reset()
{
	m_iLastUpdateTime = 0;

	for (int i = RANK_TYPE_INVALID; i < RANK_TYPE_MAX; ++i)
	{
		m_astRankLists[i].uVersionID = 0;
		m_astRankLists[i].iLastUpdateTime = 0;
		m_astRankLists[i].vRankInfos.clear();
		m_astRankLists[i].vLastRankInfos.clear();
	}
}
