#pragma once

#include <vector>
#include "GameProtocol.hpp"
#include "CommDefine.h"

class CWorldRankInfoManager
{
public:
	static CWorldRankInfoManager* Instance();

	~CWorldRankInfoManager();

	//��ʼ��RankInfo
	void Init();

	//��ȡ������Ϣ
	int GetWorldRank(int iRankType, unsigned uVersionID, World_GetRankInfo_Response& stResp);

	//����������Ϣ
	int UpdateWorldRank(int iRankType, const RankInfo& stRankInfo);

	//��ʱ��
	void OnTick(int iTimeNow);

private:

	CWorldRankInfoManager();

	//����RankData
	void SetRankData(RankData& stData, const RankInfo& stInfo);

	//�����ܰ���
	void SendRankReward(int iRankType, int iTimeNow);

	//����Rank��Ϣ
	void LoadRankInfo();

	//����Rank��Ϣ
	void SaveRankInfo();

	//����
	void Reset();

private:

	//�����б�
	RankList m_astRankLists[RANK_TYPE_MAX];
};
