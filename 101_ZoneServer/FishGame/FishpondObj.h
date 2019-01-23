#pragma once

#include <vector>
#include "ObjAllocator.hpp"
#include "Kernel/ConfigManager.hpp"

using namespace ServerLib;

const unsigned int MAX_PLAYER = 4;				//�����������������
const unsigned int MAX_BULLET_PER_SECOND = 4;	//1s����ҷ����ӵ�����
const unsigned int MAX_PLAYER_BULLET_NUM = 20;	//��������ӵ��������
const unsigned int MAX_FORM_FISH_NUM = 200;		//��������켣����������
const unsigned int MAX_FISH_NUM = 30;			//��������֧�ֵ�������
const unsigned int MAX_FORM_FISHKILL_NUM = 200;	//�������켣���ɲ�����

enum CycleTimeType
{
	CYCLE_TIME_LAST = 0,		//��һ����������
	CYCLE_TIME_NOW	= 1,		//��ǰ��������
	CYCLE_TIME_MAX	= 2,		//�����������
};

//�������
enum FishType
{
	FISH_TYPE_SMALLFISH		= 0,	//С��
	FISH_TYPE_MIDDLEFISH	= 1,	//������
	FISH_TYPE_BIGFISH		= 2,	//������
	FISH_TYPE_BIGBOSS		= 3,	//��BOSS��
	FISH_TYPE_PHONETICKET	= 4,	//��Ʊ��
	FISH_TYPE_WARHEAD		= 5,	//��ͷ��
	FISH_TYPE_MULTIPLE		= 6,	//�౶��
	FISH_TYPE_SMALLGROUP	= 9,	//С����
	FISH_TYPE_FLASHFISH		= 10,	//����
	FISH_TYPE_BOOMFISH		= 11,	//ը����
};

//��λ���������
struct SeatUserData
{
	int iSeat;					//��λ��
	unsigned uiUin;				//���uin
	int iActiveTime;			//�����Ծʱ��

	long lAimEndTime;			//��׼����ʱ��
	unsigned uAimFishUniqID;	//��׼����ΨһID
	int iAimFishIndex;			//�����׼С���飬С���index
	bool bAutoShoot;			//�Ƿ�ʹ���Զ�����
	long lWildEndTime;			//�񱩽���ʱ��
	int iWildNum;				//�񱩴���

	//��ҷ����ӵ���Ϣ�������������й�
	int iIndex;
	long alShootTime[MAX_PLAYER_BULLET_NUM];	//�����ӵ���ʱ��
	int iBulletNum;				//��ǰ�������ҵ��ӵ���Ŀ

	//��ҵ�ǰ�����֧��
	long8 lUserSilverCost;			//��ҵ���֧��
	long8 lUserTicketSilverCost;	//��ҵ�ȯ��֧��
	long8 lUserSilverReward;		//��ҵ�������
	long8 lUserFlow[CYCLE_TIME_MAX];//��ǰ������ҷ�����ˮ
	int iLossNum;					//��ǰ������ҿ��𣬸���ʾ��������ʾӯ��

	//��ҵĽ�������ID
	unsigned uCycleID;		//��ҽ�������ID
	int iCycleWinNum;		//��ҽ�����������Ӯ
	long8 lCycleNewUsedNum;	//��ҽ����������ֺ��ʹ�ý��

	//�����Ϣ
	unsigned uReturnID;		//��ȡ�����ID
	int iReturnType;		//��ȡ���������
	int iEffectCountdown;	//��Ч����ʱ,���ʱ��, ms
	long lUnEffectTime;		//���ʧЧʱ�䣬����ʱ��, ms

	SeatUserData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�������
struct OutFishRule
{
	int iFishSeqID;  //��������ID
	long lOutTime;   //�����ʱ��,��λms
	int iDeltaTime;	//����ʱ��͵�ǰʱ���ֵ����λms
	int iFishID;    //�����ID
	int iPower;		//����Ȩ��
	int iLimitType;	//����������������
	int iType;		//���������

	OutFishRule()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�ӵ����ݽṹ
struct BulletData
{
	unsigned uUniqueID;		//�ӵ���ΨһID
	int iGunID;				//�����ӵ�����̨ID
	int iSeat;				//�����ӵ���λ��
	FISHPOS stTargetPos;	//�����ӵ���Ŀ��λ��
	long lShootTime;		//�ӵ�����ʱ��,��λ ms
	unsigned uFishUniqID;	//��׼Ŀ����ΨһID��Ϊ0��ʾ����׼�ӵ�
	bool bIsAimFormFish;	//�������׼���Ƿ���׼�����е���
	int iFishIndex;			//�����׼С�����е��㣬ΪС���index��1-5
	int iWildNum;			//���ʹ���˿񱩣��񱩴���
	int iCost;				//������ӵ�������

	BulletData()
	{
		memset(this, 0, sizeof(*this));
	}

	BulletData& operator=(const BulletData& stBullet)
	{
		if (this == &stBullet)
		{
			return *this;
		}

		this->uUniqueID = stBullet.uUniqueID;
		this->iGunID = stBullet.iGunID;
		this->iSeat = stBullet.iSeat;
		this->stTargetPos = stBullet.stTargetPos;
		this->lShootTime = stBullet.lShootTime;
		this->uFishUniqID = stBullet.uFishUniqID;
		this->bIsAimFormFish = stBullet.bIsAimFormFish;
		this->iFishIndex = stBullet.iFishIndex;
		this->iWildNum = stBullet.iWildNum;
		this->iCost = stBullet.iCost;

		return *this;
	}
};

//�������Ϣ
struct FreezeData
{
	unsigned uUniqID;		//��������ΨһID
	long lFreezeBeginTime;	//���⿪ʼʱ��
	int iFreezeContTime;	//�������ʱ��
	int iTotalFreezeTime;	//�����ܹ�����ʱ��
	bool bIsFormFish;		//�Ƿ������е���

	FreezeData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//���������Ϣ
struct FormFishOutData
{
	int iOutID;			//����ΨһID
	int iFishID;		//�����ID
	int iFishSeqID;		//����������ID
	int iTraceID;		//��Ӧ�켣ID
	long lBeginTime;	//��ʼ����ʱ��, ms
	int iRemainNum;		//��������
	int iInterval;		//����ʱ����, ms
	int iMultiple;		//�������ı���
	int iType;			//���������

	int aiCostSilver[MAX_PLAYER][MAX_FORM_FISH_NUM];	//��Ҵ��������������

	//���Ƿ���,���index��Ӧλ��ʾ���Ƿ����
	ulong8 szAliveFish[4];

	FormFishOutData()
	{
		memset(this, 0, sizeof(*this));
	}

	bool IsFishAlive(int iIndex)
	{
		return (szAliveFish[iIndex / 64] >> (iIndex % 64)) & (ulong8)0x01;
	}

	void SetFishAlive(int iIndex, bool bAlive)
	{
		if (bAlive)
		{
			szAliveFish[iIndex / 64] |= ((ulong8)0x01 << (iIndex % 64));
		}
		else
		{
			szAliveFish[iIndex / 64] &= ~((ulong8)0x01 << (iIndex % 64));
		}
	}

	int GetLiveFishNum()
	{
		int iFishNum = 0;
		for (unsigned i = 0; i < sizeof(szAliveFish) / sizeof(ulong8); ++i)
		{
			ulong8 n = szAliveFish[i];
			while (n != 0) {
				n = n & (n - 1);
				++iFishNum;
			}
		}

		return iFishNum;
	}
};

//������Ϣ����
struct FishFormData
{
	long lNextUpdateTime;	//�´γ��������ʱ��
	int iFormTypeID;		//���������ID
	long lFormEndTime;		//������ʧ��ʱ��
	bool bIsCleared;		//�Ƿ��Ѿ��峡
	bool bIsInForm;			//�Ƿ���������

	std::vector<FormFishOutData> vFishOutData;		//����������ϸ��Ϣ

	std::vector<FreezeData> vFormFreezeInfo;	//�����б����������Ϣ

	FishFormData()
	{
		Reset();
	}

	void Reset()
	{
		lNextUpdateTime = 0;
		iFormTypeID = 0;
		lFormEndTime = 0;
		bIsInForm = false;
		bIsCleared = false;

		vFishOutData.clear();
		vFormFreezeInfo.clear();
	}
};

//������ݽṹ
struct FishData
{
	unsigned uUniqueID;				//���ΨһID
	int iFishID;					//���ID
	int iFishSeqID;					//���������ID
	int iTraceID;					//��Ĺ켣ID
	long lBornTime;					//����ʱ��,��λ:ms
	long lDeadTime;					//����ʱ�䣬�ﵽ����ʧ,��λms
	int aiCostSilver[MAX_PLAYER];	//����ڸ����ϵ�����
	long lFreezeBeginTime;			//������ʼʱ��
	int iFreezeContTime;			//��������ʱ��
	int iTotalFreezeTime;			//�ܹ���������ʱ��
	int iType;						//�������
	int iMultiple;					//��ı���
	USHORT cIndex;					//�����С���飬С�����������Ϣ
	int iLimitType;					//������������

	FishData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}

	FishData& operator=(const FishData& stFish)
	{
		if (this == &stFish)
		{
			return *this;
		}

		this->uUniqueID = stFish.uUniqueID;
		this->iFishID = stFish.iFishID;
		this->iFishSeqID = stFish.iFishSeqID;
		this->iTraceID = stFish.iTraceID;
		this->lBornTime = stFish.lBornTime;
		this->lDeadTime = stFish.lDeadTime;
		this->lFreezeBeginTime = stFish.lFreezeBeginTime;
		this->iFreezeContTime = stFish.iFreezeContTime;
		this->iTotalFreezeTime = stFish.iTotalFreezeTime;
		this->iType = stFish.iType;
		this->iMultiple = stFish.iMultiple;
		this->cIndex = stFish.cIndex;
		this->iLimitType = stFish.iLimitType;

		for (unsigned i = 0; i < MAX_PLAYER; ++i)
		{
			this->aiCostSilver[i] = stFish.aiCostSilver[i];
		}

		return *this;
	}
};

//����������Ϣ
struct FishLimitData
{
	int iLimitMaxNum;	//�ܳ����������
	int iRemainNum;		//��ǰʣ���ܳ�������

	FishLimitData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//������ض���
class CGameRoleObj;
class CFishpondObj : public CObj
{
public:
	CFishpondObj();
	virtual ~CFishpondObj();
	virtual int Initialize();
	virtual int Resume();
	DECLARE_DYN

public:

	//���������Ϣ
	void SetTableInfo(unsigned uTableUniqID, int iFishRoomID, const FishRoomConfig& stConfig);

	//��ҽ������
	int EnterFishpond(CGameRoleObj& stRoleObj);

	//����˳����
	void ExitFishpond(CGameRoleObj& stRoleObj, bool bForceExit);

	//����л���̨
	int ChangeGun(CGameRoleObj& stRoleObj, int iNewGunID, bool bIsStyle);

	//��ҷ����ӵ�
	int ShootBullet(CGameRoleObj& stRoleObj, long lShootTime, int iPosX, int iPosY, bool bAutoShoot);

	//���������
	int HitFish(CGameRoleObj& stRoleObj, long lHitTime, unsigned uBulletUniqID, unsigned uFishUniqID, int iFishIndex);

	//���������е���
	int HitFormFish(CGameRoleObj& stRoleObj, long lHitTime, unsigned uBulletUniqID, int iFishOutID, int iFishIndex);

	//���ʹ�ü���
	int UseSkill(CGameRoleObj& stRoleObj, const Zone_UseSkill_Request& stReq);

	//���ѡ����׼��
	int ChooseAimFish(CGameRoleObj& stRoleObj, unsigned uFishUniqID, int iFishIndex);

public:
	//��ȡ����ID
	unsigned GetTableID();

	//��ȡ���ӷ���ID
	int GetFishRoomID();

	//��ȡ�������
	int GetPlayerNum();

	//��ȡ�������
	const FishRoomConfig* GetRoomConfig();

	//��ʱ��
	int OnTick(CGameRoleObj& stRoleObj);

	//���Ͳ��������Ϣ����
	void SendFishUserUpdate(unsigned uiUin, int iResType, long8 lAddNum, int iItemID, int iItemNum);

	//����Fishpond
	void ResetFishpond();

private:
	//��ʼ������Ϣ
	int InitFishInfo();

	//��ʼ��������Ϣ
	int InitOutFishRule();

	//��ʼ��������Ϣ
	int InitFishFormRule(bool bIsFormEnd);

	//���µ���������Ϣ
	void UpdateOneOutFishRule(int iFishSeqID, OutFishRule& stFishRule, int iAddTime);

	//���������Ϣ
	void UpdateSeatUserInfo(CGameRoleObj& stRoleObj, long lTimeNow);

	//���³�����Ϣ
	int UpdateOutFishRule(long lTimeNow);

	//��������������Ϣ
	int UpdateFishInfo(long lTimeNow);

	//����������Ϣ
	int UpdateFishFormInfo(long lTimeNow);

	//���������Ϣ
	int UpdateFishUserInfo(long lTimeNow);

	//����м���
	int AddNewFish(int iFishSeqID, int iFishID, long lOutTime, int iTraceID = 0);

	long GetLastTraceBornTime(int iTraceID);

	//�Ƿ��Ӧ���͵ķ���
	bool IsRoomType(int iRoomType);

	//�Ƿ��Ӧ����ģʽ
	bool IsRoomPattern(int iRoomPattern);

	//�������
	int PlayerSitDown(CGameRoleObj& stRoleObj);

	//ɾ����������ӵ�
	void ClearUserBullets(int iSeat);

	//��ȡ�����λ��Ϣ
	SeatUserData* GetSeatUserByUin(unsigned uiUin);

	//��ȡ����ӵ�
	BulletData* GetBulletData(unsigned uBulletUniqID);

	//��ȡ�������
	FishData* GetFishData(unsigned uUniqID);

	//��ȡ�����������Ϣ
	FormFishOutData* GetFormFishInfo(int iFishOutID, int iFishIndex, FishData& stFishInfo);

	//ɾ����
	void DeleteFishData(unsigned uUniqID);

	//ɾ���ӵ�
	void DeleteBulletData(unsigned uUniqID);

	//����Ƿ���Ч����
	bool CheckIsValidHit(long lHitTime, BulletData& stBulletInfo, FishData& stFishInfo, int iFishIndex = 0);

	//����߼��Ƿ�����
	bool CheckIsLogicHit(CGameRoleObj& stRoleObj, SeatUserData& stUserData, const GunConfig& stGunConfig, FishData& stFishInfo, int iAdjust, bool bIsForm);

	//��ȡ��Ҹ���״̬
	int GetUserStatus(SeatUserData& stUserData);

	//��ȡ���������
	TEXPLINEINFO* GetRoleExpLineInfo(unsigned uiUin, const ExpLineConfig*& pstConfig);

	//���������ը�����߼�
	void BoomFish(SeatUserData& stUserData, FishData*& pstFishInfo, bool bIsFormFish);

	//���Ƿ�����Ļ��
	bool IsFishInScreen(FishData& stFishInfo, long lTimeNow);

	//�����ȯ
	bool AddUserTicket(CGameRoleObj& stRoleObj, unsigned uFishUniqID, bool bIsFormFish, int iMultiple, int iNum, bool bIsTicketFish = false);

	//���º����Ϣ
	void UpdateRedPacketInfo(CGameRoleObj& stRoleObj, SeatUserData& stUserData);

	//���������
	void SendFishHorseLamp(CGameRoleObj& stRoleObj, int iLampID, int iParam1, int iParam2, int iParam3);

	//�������
	void Reset();

private:

	//��������ʱ��
	void SendFishFormTime(CGameRoleObj* pstRoleObj, bool bIsBegin);

	//���������λ��Ϣ
	void SendSeatUserInfo(CGameRoleObj* pstToRole, CGameRoleObj* pstSeatRole);

	//��������Ϣ
	void SendFishInfoToUser(CGameRoleObj* pstTargetRole, const std::vector<FishData>& vFishes);

	//�����������ӵ���Ϣ�����
	void SendBulletInfoToUser(CGameRoleObj& stRoleObj);

	//����������Ϣ�����
	void SendFishpondInfoToUser(CGameRoleObj& stRoleObj);

	//����������ϸ��Ϣ
	void SendFishFormInfo(CGameRoleObj* pstRoleObj);

	//�����˳�������Ϣ
	void SendExitFishpondAll(int iSeat, bool bForceExit);

	//������̨�л���֪ͨ
	void SendChangeGunNotify(SeatUserData& stUserData, int iNewGunID, bool bIsStyle);

	//�����ӵ���Ϣ
	void SendShootBulletNotify(BulletData& stBulletInfo);

	//�������������Ϣ
	void SendHitFishInfoNotify(SeatUserData& stUserData, unsigned uBulletUniqID, unsigned uFishUniqID, int iRewardSilver, 
		bool bIsForm, int iCost, int iFishIndex, int iMultipleFish);

	//���ͱ�ը��������Ϣ
	void SendBoomFishInfoNotify(SeatUserData& stUserData, const std::vector<unsigned>& vFishUniqIDs, const std::vector<BYTE>& vSmallFishs, 
		const std::vector<unsigned>& vFormUniqIDs);

	//���ͻ�õ�ȯ����Ϣ
	void SendAddTicketNotify(unsigned uiUin, unsigned uFishUniqID, bool bIsFormFish, long8 lRandTicketNum);

	//������Ϣ��������������
	void SendZoneMsgToFishpond(GameProtocolMsg& stMsg);

public:
	CFishpondObj* m_pPrev;		//ǰһ����ؽڵ�
	CFishpondObj* m_pNext;		//��һ����ؽڵ�

private:
	//���ID
	unsigned m_uTableID;

	//������ڷ���ID
	int m_iFishRoomID;

	//�������
	const FishRoomConfig* m_pstRoomConfig;

	//��λ��Ϣ
	std::vector<SeatUserData> m_vSeatUserData;

	//��س������
	std::vector<OutFishRule> m_vOutFishRule;

	//����е��ӵ�
	std::vector<BulletData> m_vBulletData;

	//������Ϣ
	FishFormData m_stFishFormData;

	//����������Ϣ
	std::vector<FishData> m_vFishData;

	//��س���������Ϣ
	std::map<int, FishLimitData> m_mFishLimitData;

	//�Ƿ�ֹͣ����
	bool m_bStopOutFish;

	//���͸��ͻ��˵���Ϣ
	static GameProtocolMsg ms_stZoneMsg;

	//����ϴ�tickʱ��,��λms
	long m_lLastTickTime;
};
