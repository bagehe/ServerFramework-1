#pragma once

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "Kernel/GameRole.hpp"

class CRankInfoManager
{
public:
	static CRankInfoManager* Instance();

	~CRankInfoManager();

	//��ʼ��
	void Init();

	//��ȡ������Ϣ
	int GetRankInfo(unsigned uin, int iType, int iFromRank, int iNum, bool bLastRank, Zone_GetRankInfo_Response& stResp);

	//����������Ϣ
	void UpdateRoleRank(CGameRoleObj& stRoleObj);

	//����������Ϣ
	int SetRankListInfo(const World_GetRankInfo_Response& stResp);

	//��ʱ��
	void OnTick();

private:
	CRankInfoManager();

	//����������Ϣ
	void UpdateRankByType(int iType, const RankData& stData, bool bUpdateRank);

	//��ȡ���а���Ϣ
	void GetRankInfoFromWorld(int iType);

	//���������Ϣ
	void PackRankInfo(unsigned uin, int iFromRank, int iNum, const std::vector<RankData>& vRankDatas, Zone_GetRankInfo_Response& stResp);

	//�Ƿ���������
	bool IsInRank(int iType, const RankData& stData);

	void Reset();

private:

	//�ϴ���������ʱ��
	int m_iLastUpdateTime;

	//���а���Ϣ
	RankList m_astRankLists[RANK_TYPE_MAX];
};
