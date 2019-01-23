#pragma once

#include <vector>

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "Kernel/GameRole.hpp"

//�����ע��Ϣ
struct UserBetData
{
	unsigned uin;	//���uin
	std::vector<LasvegasBetData> vBets;

	UserBetData()
	{
		Reset();
	}

	bool operator== (const UserBetData& stData)
	{
		return (this->uin == stData.uin);
	}

	void Reset()
	{
		uin = 0;
		vBets.clear();
	}
};

class CLasvegasManager
{
public:
	static CLasvegasManager* Instance();

	~CLasvegasManager();

	//��ʼ��
	void Init();

	//��ҽ���ת��
	void EnterLasvegas(CGameRoleObj& stRoleObj, bool bIsEnter);

	//�����ע
	int BetLasvegas(CGameRoleObj& stRoleObj, int iNumber, int iBetCoins);

	//����ת����Ϣ
	void UpdateLasvegasInfo(const World_UpdateLasvegas_Notify& stNotify);

	//��ȡת���н���Ϣ
	int GetRewardInfo(CGameRoleObj& stRoleObj, Zone_GetRewardInfo_Response& stResp, int iFrom, int iNum);

	//��ʱ��
	void OnTick();

private:
	CLasvegasManager();

	//���Ž���
	void SendLasvegasReward();

	//����ת����Ϣ
	void SendLasvegasInfoNotify(CGameRoleObj* pstRoleObj);

	//����World��ע��Ϣ
	void SendUpdateBetInfoToWorld(int iNumber, int iBetCoins);

	//���ת����Ϣ,�������н���¼
	void PackLasvegasInfo(LasvegasInfo& stInfo);

	void Reset();

private:

	//��ǰ�׶�
	int m_iStepType;

	//��ǰ�׶ν���ʱ��
	int m_iStepEndTime;

	//�Ƿ����͸��¸����
	bool m_bSendUserNotify;

	//������͸��¸���ҵ�ʱ��
	int m_iLastSendNotifyTime;

	//�Ƿ���Ҫ���Ž���
	bool m_bNeedSendReward;

	//�������Ž���ʱ��
	int m_iLotteryRewardTime;

	//���ο�����Ϣ
	int m_iLotteryID;

	//���������Ϣ
	std::vector<int> m_vLotteryIDs;

	//����н���¼
	std::vector<LotteryPrizeData> m_vPrizeDatas;

	//��ǰ��ע��Ϣ
	std::vector<LasvegasBetData> m_vBetDatas;

	//ת������б�
	std::vector<UserBetData> m_vUserList;
};
