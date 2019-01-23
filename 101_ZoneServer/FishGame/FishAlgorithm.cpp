
#include "GameProtocol.hpp"
#include "StringUtility.hpp"
#include "LogAdapter.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "FishpondObj.h"
#include "FishUtility.h"

#include "FishAlgorithm.h"

using namespace ServerLib;

//�㷨����
//�������㷨����
RoomAlgorithmData FishAlgorithm::astData[MAX_ROOM_ALGORITHM_TYPE];

//�㷨��ʼ��
void FishAlgorithm::Initalize()
{
	for (int i = 0; i < MAX_ROOM_ALGORITHM_TYPE; ++i)
	{
		astData[i].lLastUpdateTime = CTimeUtility::GetMSTime();
		astData[i].iX = CConfigManager::Instance()->GetBaseCfgMgr().GetGlobalConfig(GLOBAL_TYPE_INITPUMPINGRATIO);
	}
}

void FishAlgorithm::OnTick()
{
	long lTimeNow = CTimeUtility::GetMSTime();

	for (int i = 0; i < MAX_ROOM_ALGORITHM_TYPE; ++i)
	{
		//���·�����״̬
		UpdateServerStat(lTimeNow, i);

		//���´����״̬
		UpdateBigReturnStat(lTimeNow, i);
	}
}

void FishAlgorithm::GetOneRedPacket(CGameRoleObj& stRoleObj, SeatUserData& stUserInfo, int iAlgorithmType)
{
	long lTimeNow = CTimeUtility::GetMSTime();
	int iRedPacketType = RETURN_TYPE_SILVERFLOW;

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	int iServerUpdateTime = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_UPDATETIME) * 1000;

	RoomAlgorithmData& stData = astData[iAlgorithmType];
	if (stData.uReturnID == stUserInfo.uReturnID)
	{
		//�������Ѿ���ù����
		return;
	}

	stUserInfo.iEffectCountdown = 0;
	stUserInfo.lUnEffectTime = 0;

	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stUserInfo.uiUin);
	if (!pstRoleObj)
	{
		LOGERROR("Failed to get game role obj, uin %u\n", stUserInfo.uiUin);
		return;
	}

	bool bGetReturn = false;
	if (stData.aiReturnNum[RETURN_TYPE_SILVERFLOW]>0 && stUserInfo.lUserFlow[CYCLE_TIME_LAST] >= stData.lLastAverageFlow)
	{
		//���Ի�ø���ˮ���

		//ֻ���ڼ�����˲�����ȡ���
		if (stData.lReturnBulletInterval==0 || stData.lTotalBulletNum==0 || stData.lTotalBulletNum%stData.lReturnBulletInterval != 0)
		{
			return;
		}

		bGetReturn = true;
		stUserInfo.iReturnType = RETURN_TYPE_SILVERFLOW;
		iRedPacketType = RETURN_TYPE_SILVERFLOW;
		--stData.aiReturnNum[RETURN_TYPE_SILVERFLOW];
	}
	else if (stData.aiReturnNum[RETURN_TYPE_ONLINEENVELOPRATIO]>0 && (lTimeNow - pstRoleObj->GetLoginTime()) >= iServerUpdateTime)
	{
		//�ܻ�����ߺ��

		//ֻ���ڼ�����˲�����ȡ���
		if (stData.lReturnBulletInterval == 0 || stData.lTotalBulletNum == 0 || stData.lTotalBulletNum%stData.lReturnBulletInterval != 0)
		{
			return;
		}

		bGetReturn = true;
		stUserInfo.iReturnType = RETURN_TYPE_ONLINEENVELOPRATIO;
		iRedPacketType = RETURN_TYPE_ONLINEENVELOPRATIO;
		--stData.aiReturnNum[RETURN_TYPE_ONLINEENVELOPRATIO];
	}
	else if (stData.aiReturnNum[RETURN_TYPE_LOSSPLAYERRATIO]>0 && stUserInfo.iLossNum>0)
	{
		//�ܻ�ÿ�����

		//ֻ���ڼ�����˲�����ȡ���
		if (stData.lReturnBulletInterval == 0 || stData.lTotalBulletNum == 0 || stData.lTotalBulletNum%stData.lReturnBulletInterval != 0)
		{
			return;
		}

		bGetReturn = true;
		stUserInfo.iReturnType = RETURN_TYPE_LOSSPLAYERRATIO;
		iRedPacketType = RETURN_TYPE_LOSSPLAYERRATIO;
		--stData.aiReturnNum[RETURN_TYPE_LOSSPLAYERRATIO];
	}
	else if (stData.aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO]>0)
	{
		//�ܻ��������

		//ֻ���ڼ�����˲�����ȡ���
		if (stData.lReturnBulletInterval == 0 || stData.lTotalBulletNum == 0 || stData.lTotalBulletNum%stData.lReturnBulletInterval != 0)
		{
			return;
		}

		bGetReturn = true;
		stUserInfo.iReturnType = RETURN_TYPE_RANDOMENVELOPRATIO;
		iRedPacketType = RETURN_TYPE_RANDOMENVELOPRATIO;
		--stData.aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO];
	}

	if (bGetReturn)
	{
		//����˺��
		LOGDEBUG("GetType %d, Return Packet Num: %d|%d|%d|%d|%d\n", iRedPacketType, stData.aiReturnNum[0],
			stData.aiReturnNum[1], stData.aiReturnNum[2], stData.aiReturnNum[3],
			stData.aiReturnNum[4]);

		stUserInfo.uReturnID = stData.uReturnID;

		//��ӡ�����Ӫ��־
		CZoneOssLog::TraceGetRedPacket(stUserInfo.uiUin, stRoleObj.GetNickName(), iRedPacketType);

		//������ͨ�����Ϣ
		int iRandMax = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MAXTIMELIMIT);
		int iRandMin = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MINTIMELIMIT);
		stUserInfo.iEffectCountdown = (rand() % (iRandMax - iRandMin) + iRandMin) * 1000;

		//��ȡ���µĺ��
		stUserInfo.iLossNum = 0;
	}

	return;
}

//���·�����״̬
void FishAlgorithm::UpdateServerStat(long lTimeNow, int iAlgorithmType)
{
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	int iServerUpdateTime = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_UPDATETIME) * 1000;

	if (lTimeNow < (astData[iAlgorithmType].lLastUpdateTime + iServerUpdateTime))
	{
		//δ��ʱ��
		return;
	}

	//���½���ʱ��
	astData[iAlgorithmType].lLastUpdateTime = lTimeNow;
	astData[iAlgorithmType].uReturnID = CFishUtility::GetReturnUniqID();
	astData[iAlgorithmType].uCycleID = CFishUtility::GetCycleUniqID();

	//��ȡ���õ�X1��X2,Y
	//int iX1ConfigValue = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MINPUMPINGRATIO);	//���ַ���
	int iX1ConfigValue = stBaseCfgMgr.GetRoomX1Config(iAlgorithmType);	//�ַ���
	int iX2ConfigValue = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_INITPUMPINGRATIO);
	int iYConfigValue = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_CHOUSHUIMODIFY);

	//�ȼ��������ӯ��
	long8 lServerWinNum = astData[iAlgorithmType].lRewardSilver - (astData[iAlgorithmType].lCostSilver - astData[iAlgorithmType].lUsedReturnSilver);
	
	//��ӡ������ ����������� ��Ӫ��־
	//�����ˮ
	int iPump = 0;
	if (astData[iAlgorithmType].lRewardSilver != 0)
	{
		iPump = lServerWinNum / astData[iAlgorithmType].lRewardSilver * 1000;
	}

	//����ƽ����̨����
	int iAverageGunMultiple = 0;
	if (astData[iAlgorithmType].lTotalBulletNum != 0)
	{
		iAverageGunMultiple = astData[iAlgorithmType].lRewardSilver / astData[iAlgorithmType].lTotalBulletNum;
	}
	
	if (lServerWinNum < 0)
	{
		//�������������
		astData[iAlgorithmType].lTotalServerLossNum -= lServerWinNum;

		//�������
	}
	else
	{
		if (astData[iAlgorithmType].iX == iX2ConfigValue)
		{
			//If X=X2���������=G*��X2-X1��/X2��K=0

			//������0
			astData[iAlgorithmType].lTotalServerLossNum = 0;

			//������ �������=G*��X2-X1��/X2
			astData[iAlgorithmType].lTotalReturnSilver += lServerWinNum*(iX2ConfigValue - iX1ConfigValue) / iX2ConfigValue;
		}
		else
		{
			//�������=G*��X2-X1��/��X2+10%����K=K-G*10%/��X2+10%��

			astData[iAlgorithmType].lTotalServerLossNum -= lServerWinNum*iYConfigValue / (iX2ConfigValue + iYConfigValue);
			astData[iAlgorithmType].lTotalServerLossNum = astData[iAlgorithmType].lTotalServerLossNum > 0 ? astData[iAlgorithmType].lTotalServerLossNum : 0;
			astData[iAlgorithmType].lTotalReturnSilver += lServerWinNum*(iX2ConfigValue - iX1ConfigValue) / (iX2ConfigValue + iYConfigValue);
		}
	}

	//�����ȯ����ˮ����
	const int iTicketFishReturnRate = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_TICKETRETURNRATE);
	long8 lTicketFishReturnNum = 0;
	if (astData[iAlgorithmType].lTicketFishRewardSilver > (astData[iAlgorithmType].lTicketFishDropNum*iTicketFishReturnRate))
	{
		lTicketFishReturnNum = (astData[iAlgorithmType].lTicketFishRewardSilver - (astData[iAlgorithmType].lTicketFishDropNum*iTicketFishReturnRate));
		astData[iAlgorithmType].lTotalReturnSilver += lTicketFishReturnNum;
	}

	//If K>0ʱ����ˮ����X = X2 + 10 % ; If K = 0 ��ˮ����X = X2
	if (astData[iAlgorithmType].lTotalServerLossNum > 0)
	{
		astData[iAlgorithmType].iX = iX2ConfigValue + iYConfigValue;
	}
	else
	{
		astData[iAlgorithmType].iX = iX2ConfigValue;
	}

	//3.���Ų����ĺ��

	//�����������������
	int iReturnRatio = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ENVELOPERATIO);

	//���ź��������
	long8 lReturnPlayerNum = astData[iAlgorithmType].lPlayingNum;

	//�����������
	if (lReturnPlayerNum > 0 && astData[iAlgorithmType].lTotalReturnSilver > 0)
	{
		//������������
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_SILVERFLOW] = lReturnPlayerNum * iReturnRatio * stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_GUNMULTIPLE) / (1000 * 1000);
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_ONLINEENVELOPRATIO] = lReturnPlayerNum * iReturnRatio * stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ONLINEENVELOPRATIO) / (1000 * 1000);
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_LOSSPLAYERRATIO] = lReturnPlayerNum * iReturnRatio * stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LOSSPLAYERRATIO) / (1000 * 1000);
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO] = lReturnPlayerNum * iReturnRatio * stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_RANDOMENVELOPRATIO) / (1000 * 1000);
	}

	//���ٷ�2��
	if (astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO] <= 1)
	{
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO] = 2;
	}

	//4.���������״̬

	//��������������
	astData[iAlgorithmType].iServerX = 0;
	astData[iAlgorithmType].lServerWinInventory = (astData[iAlgorithmType].lTotalRewardSilver - astData[iAlgorithmType].lTotalCostSliver + astData[iAlgorithmType].lTotalNewRedUseNum);
	long8 lTotalUserCost = 0;
	if (astData[iAlgorithmType].lPlayingNum > 0)
	{
		lTotalUserCost = astData[iAlgorithmType].lTotalBulletNum * astData[iAlgorithmType].lTotalGunMultiple * 
			stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_SILVERPARAM) / (astData[iAlgorithmType].lPlayingNum * 1000);
	}
	
	long lInitWinSilver = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_INITSILVER);
	astData[iAlgorithmType].lDynamicInventory = (lTotalUserCost >= lInitWinSilver) ? lTotalUserCost : lInitWinSilver;
	long8 lServerRatio = 0;
	if (astData[iAlgorithmType].lDynamicInventory > 0)
	{
		lServerRatio = (astData[iAlgorithmType].lServerWinInventory + astData[iAlgorithmType].lDynamicInventory) * 1000 / astData[iAlgorithmType].lDynamicInventory;
	}

	const ServerStatConfig* pstStatConfig = stBaseCfgMgr.GetServerStatConfig(lServerRatio);
	if (pstStatConfig)
	{
		astData[iAlgorithmType].iServerX = pstStatConfig->iDecrease;
	}

	//�����ϸ����ڵķ�����ƽ����ˮ
	astData[iAlgorithmType].lLastAverageFlow = astData[iAlgorithmType].lPlayingNum>0 ? (astData[iAlgorithmType].lRewardSilver / astData[iAlgorithmType].lPlayingNum) : 0;

	//�����������ӵ����
	astData[iAlgorithmType].lReturnBulletInterval = astData[iAlgorithmType].lTotalBulletNum*stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_RETURNALLTIME) /
		(stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_UPDATETIME) * 1000);

	//���¶౶������
	astData[iAlgorithmType].lMultiFishLossNum += (astData[iAlgorithmType].lMultiFishFlow - astData[iAlgorithmType].lMultiFishHitFlow);
	if (astData[iAlgorithmType].lMultiFishLossNum > 0)
	{
		astData[iAlgorithmType].lMultiFishLossNum = 0;
	}

	//��ӡ��Ӫ��־
	CZoneOssLog::TraceServerCycleProfit(iAlgorithmType, astData[iAlgorithmType].lRewardSilver,
		astData[iAlgorithmType].lCostSilver, lServerWinNum, astData[iAlgorithmType].lTicketFishDropNum,
		iPump, astData[iAlgorithmType].lPlayingNum, iAverageGunMultiple,
		astData[iAlgorithmType].lTotalReturnSilver, lTicketFishReturnNum, astData[iAlgorithmType].lLastUpdateTime+ iServerUpdateTime,
		astData[iAlgorithmType].aiReturnNum[RETURN_TYPE_RANDOMENVELOPRATIO], astData[iAlgorithmType].lUsedReturnSilver);

	astData[iAlgorithmType].lCostSilver = 0;
	astData[iAlgorithmType].lRewardSilver = 0;
	astData[iAlgorithmType].lTotalBulletNum = 0;
	astData[iAlgorithmType].lUsedReturnSilver = 0;
	astData[iAlgorithmType].lTicketFishRewardSilver = 0;
	astData[iAlgorithmType].lTicketFishDropNum = 0;
	astData[iAlgorithmType].lMultiFishFlow = 0;
	astData[iAlgorithmType].lMultiFishHitFlow = 0;

	return;
}

//���´����״̬
void FishAlgorithm::UpdateBigReturnStat(long lTimeNow, int iAlgorithmType)
{
	//���ж��Ƿ����
	if (!CFishUtility::IsSameDay(lTimeNow, astData[iAlgorithmType].lBigReturnUpdateTime) && astData[iAlgorithmType].lBigReturnUpdateTime != 0)
	{
		astData[iAlgorithmType].lTodayCostSliver = 0;
		astData[iAlgorithmType].lTodayRewardSilver = 0;
		astData[iAlgorithmType].lTodayBigReturnSilver = 0;

		return;
	}
	
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	int iBigReturnUpdateTime = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_UPDATETIME) * 1000;
	if (lTimeNow < (astData[iAlgorithmType].lBigReturnUpdateTime + iBigReturnUpdateTime))
	{
		//δ��ʱ��
		return;
	}

	astData[iAlgorithmType].lBigReturnUpdateTime = lTimeNow;
	
	//int iX1ConfigValue = stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_MINPUMPINGRATIO);	//���ַ���
	int iX1ConfigValue = stBaseCfgMgr.GetRoomX1Config(iAlgorithmType);	//�ַ���

	//�������ӯ��
	long8 lExtraWinSilver = astData[iAlgorithmType].lTodayRewardSilver*(1000 - iX1ConfigValue) / 1000 - astData[iAlgorithmType].lTodayCostSliver
		- astData[iAlgorithmType].lTodayBigReturnSilver - astData[iAlgorithmType].lTotalReturnSilver;
	if (lExtraWinSilver <= 0)
	{
		//�����㷢��Ҫ��
		return;
	}

	astData[iAlgorithmType].lTodayBigReturnSilver += lExtraWinSilver;
	astData[iAlgorithmType].lTotalReturnSilver += lExtraWinSilver;

	return;
}
