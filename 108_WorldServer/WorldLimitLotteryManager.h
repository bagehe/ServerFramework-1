#pragma once

#include <vector>
#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "BaseConfigManager.hpp"

//�����齱��Ϣ
struct LimitLotteryInfo
{
	int iLotteryID;			//�齱ID
	int iWeight;			//Ȩ��
	int iLimitType;			//��������
	int iConfigDayLimit;	//���õ�������
	int iDailyLimit;		//����ʣ������
	int iTotalLimit;		//��ʣ������

	LimitLotteryInfo()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//��ֵ�齱��¼
struct RechargeLotteryRecord
{
	std::string strNickName;	//�齱����ǳ�
	int iLotteryID;				//���е�ID

	RechargeLotteryRecord()
	{
		Reset();
	}

	void Reset()
	{
		strNickName.clear();
		iLotteryID = 0;
	}
};

class CWorldLimitLotteryManager
{
public:
	static CWorldLimitLotteryManager* Instance();

	~CWorldLimitLotteryManager();

	//��ʼ��
	void Init();

	//�����齱
	void LimitLottery(int iLotteryType, const std::string& strNickName, bool bIsTenTimes, std::vector<int>& vLotteryIDs);

	//��ȡ��ֵ��¼
	void GetLotteryRecord(int iFrom, int iNum, Zone_PayLotteryRecord_Response& stResp);

private:

	CWorldLimitLotteryManager();

	//���س齱��¼
	void LoadRechargeLotteryRecord();

	//����齱��¼
	void SaveRechargeLotterRecord();

	//�������ó齱��Ϣ
	void ResetLotteryInfo(bool bIsInit);

	//һ�γ齱�����س齱���
	int LotteryOneTime(int iLimitType);

	//���ӳ齱��¼
	void AddLotteryRecord(const std::string& strNickName, const std::vector<int>& vLotteryIDs);

	//����
	void Reset();

private:

	//��ֵ�齱��Ϣ�������ʱ��
	int m_iLastUpdateTime;

	//��ֵ�齱������Ϣ
	std::vector<LimitLotteryInfo> m_avLotteryInfo[LIMIT_LOTTERY_MAX];

	//��ֵ�齱��¼
	std::vector<RechargeLotteryRecord> m_vLotteryRecord;
};
