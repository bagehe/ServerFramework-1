#pragma once

#include <string>
#include <vector>
#include "GameProtocol.hpp"
#include "CommDefine.h"

class CWorldLasvegasManager
{
public:
	static CWorldLasvegasManager* Instance();

	~CWorldLasvegasManager();

	//��ʼ��Lasvegas
	void Init();

	//����������ע��Ϣ
	void UpdateBetNumber(int iNumber, int iBetCoins);

	//�����н���Ϣ
	void UpdatePrizeInfo(const World_UpdatePrizeInfo_Request& stReq);

	//��ʱ��
	void OnTick(int iTimeNow);

private:

	CWorldLasvegasManager();

	//����Lasvegas��Ϣ
	void LoadLasvegasInfo();

	//����Lasvegas��Ϣ
	void SaveLasvegasInfo();

	//���ʹ�ת����Ϣ
	void SendUpdateLasvegasNotify(int iTimeNow);

	//����
	void Reset();

private:

	//��ǰ�׶�
	int m_iStepType;

	//��ǰ�׶ν���ʱ��
	int m_iStepEndTime;

	//�Ƿ���Ҫ���͸���
	bool m_bSendUpdate;

	//�ϴθ�������ʱ��
	int m_iLastUpdateTime;

	//���������Ϣ
	std::vector<int> m_vLotteryIDs;

	//����н���¼
	std::vector<LotteryPrizeData> m_vPrizeDatas;

	//��ǰ��ע��Ϣ
	std::vector<LasvegasBetData> m_vBetDatas;
};
