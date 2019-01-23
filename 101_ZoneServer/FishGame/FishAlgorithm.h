#pragma once

#include <string.h>
#include "CommDefine.h"

//�����㷨����

//�������
enum ReturnType
{
	RETURN_TYPE_SILVERFLOW = 0,			//������ˮ�ϸߵ���
	RETURN_TYPE_ONLINEENVELOPRATIO = 1,	//�ϸ�ʱ��γ������ߵ���
	RETURN_TYPE_LOSSPLAYERRATIO = 2,	//�ϸ�ʱ��ο������
	RETURN_TYPE_RANDOMENVELOPRATIO = 3,	//�����ȡ����
	RETURN_TYPE_BIGBOSS = 5,			//BOSSս����
	RETURN_TYPE_MAX = 6,				//�������
};

struct RoomAlgorithmData
{
	//�ϴν���ʱ��
	long lLastUpdateTime;

	//�ϴδ��������ʱ��
	long lBigReturnUpdateTime;

	//��ǰ��������ID
	unsigned uCycleID;

	//�������ܽ������
	long8 lTotalCostSliver;

	//�������ܽ������
	long8 lTotalRewardSilver;

	//�����������ֺ��ʹ����
	long8 lTotalNewRedUseNum;

	//�������������
	long8 lCostSilver;

	//�������������
	long8 lRewardSilver;

	//���㷨���͵�ǰ�����
	long8 lPlayingNum;

	//�������ϸ�����ƽ����ˮ
	long8 lLastAverageFlow;

	//��������ǰ��̨�ܱ���
	long8 lTotalGunMultiple;

	//��������ǰ�����ӵ�����Ŀ
	long8 lTotalBulletNum;

	//��������ӵ����
	long8 lReturnBulletInterval;

	//�ϸ��������ڵ�X
	int iX;

	//�ϸ��������ڷ�������״̬
	int iServerX;

	//������ӯ�����
	long8 lServerWinInventory;

	//��������̬���
	long8 lDynamicInventory;

	//�ܵĺ�����Ž��
	long8 lTotalReturnSilver;

	//���ں��ʹ�ý��
	long8 lUsedReturnSilver;

	//���������ս������
	long8 lTodayCostSliver;

	//���������ս������
	long8 lTodayRewardSilver;

	//�����������ѷ��Ŵ����
	long8 lTodayBigReturnSilver;

	//��ǰ���ID
	unsigned uReturnID;

	//ÿ�����ͺ������
	int aiReturnNum[RETURN_TYPE_MAX];

	//�������ۼƿ���ֵ
	long8 lTotalServerLossNum;

	//���ڵ�ȯ�����������
	long8 lTicketFishRewardSilver;

	//���ڵ�ȯ�������Ʊ����
	long8 lTicketFishDropNum;

	//���ڶ౶�㷢����ˮ
	long8 lMultiFishFlow;

	//���ڶ౶��������ˮ
	long8 lMultiFishHitFlow;

	//���ڶ౶������
	long8 lMultiFishLossNum;

	RoomAlgorithmData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�����㷨
struct SeatUserData;
class FishAlgorithm
{
public:

	//�㷨��ʼ��
	static void Initalize();

	//�㷨��ʱ��
	static void OnTick();

	static void GetOneRedPacket(CGameRoleObj& stRoleObj, SeatUserData& stUserInfo, int iAlgorithmType);

private:

	//���·�����״̬
	static void UpdateServerStat(long lTimeNow, int iAlgorithmType);

	//���´����״̬
	static void UpdateBigReturnStat(long lTimeNow, int iAlgorithmType);

public:

	//�������㷨����
	static RoomAlgorithmData astData[MAX_ROOM_ALGORITHM_TYPE];
};