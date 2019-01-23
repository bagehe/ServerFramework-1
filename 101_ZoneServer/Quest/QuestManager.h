#pragma once

//������������
#include <string.h>
#include <vector>

#include "GameProtocol.hpp"

//��ʼ��������׶�
const static int START_ADVENTURE_QUEST = 1;

//��������
struct QuestData
{
	int iQuestID;	//����ID
	int iType;		//��������
	int iNeedType;	//���������������
	long8 iNum;		//����ǰ��������
	bool bIsFin;	//�����Ƿ������

	QuestData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

class CGameRoleObj;
class RewardConfig;
class CQuestManager
{
public:
	CQuestManager();
	~CQuestManager();

public:

	//��ʼ��
	int Initialize();

	void SetOwner(unsigned int uin);
	CGameRoleObj* GetOwner();

	//�������
	int FinQuest(int iQuestID);

	//��ȡ��Ծ�Ƚ���
	int GetLivnessReward(int iRewardID);

	//������������
	int OnQuestNeedChange(int iNeedType, int iParam1, int iParam2, int iParam3, int iParam4, int iExtraParam=0);

	//�����ӵ�֪ͨ
	void OnShootBullet(int iGunID, int iCost);

	//��ʱ��
	void OnTick(int iTimeNow);

	//��������DB
	void UpdateQuestToDB(QUESTDBINFO& stQuestDBInfo);

	//��DB��ʼ������
	void InitQuestFromDB(const QUESTDBINFO& stQuestDBInfo);

private:

	//�������
	void AddQuest(int iQuestID, int iType, int iNeedType, QuestChange& stChangeInfo, long8 iNum=0);

	//��������
	void UpdateQuest(int iQuestID, int iChangeType, int iAddNum, QuestChange& stChangeInfo);

	//ɾ������
	void DeleteQuest(int iQuestID, QuestChange& stChangeInfo);

	//��ȡ����
	QuestData* GetQuestByID(int iQuestID);

	//��ȡ������
	int GetQuestReward(int iQuestID, int iQuestType, const RewardConfig* pstRewardConfig, int iNum);

	//��������
	void ResetQuest(bool bIsInit);

private:

	//���uin
	unsigned m_uiUin;

	//���ӵ�е�����
	std::vector<QuestData> m_vQuestData;

	//��������������ʱ��
	int m_iAdventEndTime;

	//������������õĴ���
	int m_iAdventNum;

	//������������´�ˢ��ʱ��
	int m_iAdventNextUpdateTime;

	//���ÿ�������´�ˢ��ʱ��
	int m_iDailyNextUpdateTime;

	//��������׶�����ˮ
	int m_iAdventUserCost;

	//��������׶��ܷ���
	int m_iAdventShootNum;

	//����ȡ�Ļ�Ծ�ȱ���ID
	std::vector<int> m_vGetLivnessRewards;
};
