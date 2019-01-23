#pragma once

#include <vector>

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "Kernel/GameRole.hpp"

//��ҳ�ֵ��¼
struct PayRecordData
{
	int iTime;			//��ֵʱ��
	int iRechargeID;	//��ֵID

	PayRecordData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CRechargeManager
{
public:
	CRechargeManager();
	~CRechargeManager();

public:
	//��ʼ��
	int Initialize();

	void SetOwner(unsigned int uin);
	CGameRoleObj* GetOwner();

	//��ҳ�ֵ
	int UserRecharge(int iRechargeID, int iTime, const std::string& strOrderID);

	//�����ȡ��¼
	void GetPayRecord(Zone_GetPayRecord_Response& stResp);

	//�����ȡ��ֵ���
	int GetPayGift(int iGiftType);

	//���³�ֵ��DB
	void UpdateRechargeToDB(RECHARGEDBINFO& stInfo);

	//��DB���س�ֵ
	void InitRechargeFromDB(const RECHARGEDBINFO& stInfo);
private:

	//������ҳ�ֵ��¼
	void AddRechargeRecord(int iRechargeID, int iTime);

	//����VIP����
	void AddVipExp(int iAddExp);

	void Reset();

private:
	//���uin
	unsigned m_uiUin;

	//����׳����״̬
	int m_iFirstRewardStat;

	//����ѳ�ֵ�����ID
	std::vector<int> m_vRechargeIDs;

	//��ҳ�ֵ��¼
	std::vector<PayRecordData> m_vRecords;
};
