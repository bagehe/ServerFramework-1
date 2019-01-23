#ifndef __GAME_ROLE_HPP__
#define __GAME_ROLE_HPP__

#include <string.h>

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "ObjAllocator.hpp"
#include "TimeUtility.hpp"
#include "LogAdapter.hpp"
#include "AppTick.hpp"
#include "GameObjCommDef.hpp"

#include "RepThings/RepThingsManager.hpp"
#include "Exchange/ExchangeManager.h"
#include "Quest/QuestManager.h"
#include "Mail/MailManager.h"
#include "Recharge/RechargeManager.h"

using namespace ServerLib;

// Zoneά�� ����ɫ��������д���ݿ�ʱ�������룩
const int ROLEDATA_REWRITE_SECONDS = 10;

// ������״̬
typedef enum tagEnumAAStatus
{
    EAS_HEALTHY     = 0,        // ����0 ~ 3Сʱ
    EAS_TIRED       = 1,        // ƣ��3 ~ 5Сʱ
    EAS_UNHEALTHY   = 2,        // ������ >5Сʱ
} TAAStatus;

// ��Ϣ��¼
typedef struct
{
    unsigned short m_wMsgID;    //��ϢID
    unsigned char m_ucSource;   //��Դ
    timeval m_stTimeval;        //ʱ��
}MsgInfo;

const int MAX_MSG_QUEUE_LEN = 400;

typedef struct tagMsgInfoQueue
{
    int m_iMsgQueueIndex;
    MsgInfo m_astMsgInfo[MAX_MSG_QUEUE_LEN];

    tagMsgInfoQueue()
    {
        memset(this, 0, sizeof(*this));
    };
}MsgInfoQueue;

// ģ����
typedef enum
{
    MAIL_LOCK = 0,
    MARKET_LOCK = 1,
    MAX_LOCK_NUM = 2
}ENUM_LOCK;

class CGameSessionObj;
class CGameRoleObj : public CObj
{
public:
    CGameRoleObj();
    virtual ~CGameRoleObj();
    virtual int Initialize();
    virtual int Resume();
    DECLARE_DYN

///////////////////////////////////////////////////////////////////////
public:

    // ��ʼ���������
    int InitRole(const RoleID& stRoleID);

    void SetRoleID(const RoleID& stRoleID) { m_stRoleInfo.stRoleID.CopyFrom(stRoleID); };
    const RoleID& GetRoleID() { return m_stRoleInfo.stRoleID; };
    unsigned int GetUin() const { return m_stRoleInfo.stRoleID.uin(); };
    void SetUin(unsigned int uiUin) { m_stRoleInfo.stRoleID.set_uin(uiUin); };

    TROLEINFO& GetRoleInfo() { return m_stRoleInfo; };
    TUNITINFO& GetUnitInfo() { return m_stRoleInfo.stUnitInfo; };

    int GetSessionID();
    void SetSessionID(int iSessionID);
    CGameSessionObj* GetSession();

    //sznickname ����
    void SetNickName(const char* strNickName);
    const char* GetNickName();

    //uStatus ״̬
    unsigned GetRoleStatus() { return m_stRoleInfo.stUnitInfo.uiUnitStatus; };
    void SetRoleStatus(int uStatus) { m_stRoleInfo.stUnitInfo.uiUnitStatus = uStatus; };

    //iLastLogin �ϴε�¼ʱ��
    int GetLastLoginTime() ;
    void SetLastLoginTime(int iLastLoginTime) ;

    //iLastLogout �ϴ�����ʱ��
    int GetLastLogoutTime(){ return m_stRoleInfo.stBaseInfo.iLastLogout;}
    void SetLastLogoutTime(int iLastLogoutTime){m_stRoleInfo.stBaseInfo.iLastLogout = iLastLogoutTime;}

    //iCreateTime �ʺŴ�����ʱ��
    void SetCreateTime(int iCreateTime);
    int GetCreateTime();
    
    //������ʱ��
    void SetOnlineTotalTime(int iOnlineTime);
    int GetOnlineTotalTime();
    int GetOnlineThisTime()const;

    //iLoginCount ����ܵĵ�¼����
    int GetLoginCount() {return m_stRoleInfo.stBaseInfo.iLoginCount;}
    void SetLoginCount(int iCount) { m_stRoleInfo.stBaseInfo.iLoginCount = iCount;}

    //iLoginTime ���ε�¼��ʱ��
    void InitLoginTime();
    int GetLoginTime() ;

    //iLogoutTime ���εǳ���Ϸ��ʱ��
    int GetLogoutTime() ;
    void SetLogoutTime(int iLogoutTime) ;

	bool IsGM() { return false; };

	//�����̨��Ϣ
	TWEAPONINFO& GetWeaponInfo();
	void SetWeaponInfo(const WeaponInfo& stWeapon);
	void UnlockWeaponStyle(int iStyleID);

	//�����Դ��Ϣ
	long8 GetResource(int iType);
	void SetResource(int iType, long8 lResource);
	bool AddResource(int iType, long8 lAddResource);

	//���VIP�ȼ�
	int GetVIPLevel();
	void SetVIPLevel(int iLevel);

	//���VIP����
	long8 GetVIPExp();
	void SetVIPExp(long8 iExp);
	void AddVIPExp(int iExp);

	//���VIP��Ȩ
	int GetVIPPriv();
	void SetVIPPriv(int iPriv);
	bool HasVIPPriv(int iPrivType);

	//��Ҿȼý��´ο���ȡʱ��
	int GetNextAlmsTime();
	void SetNextAlmsTime(int iTime);

	//��ҵ�������ȡ�ȼý����
	int GetAlmsNum();
	void SetAlmsNum(int iNum);

	//����ϴξȼý����ʱ��
	int GetLastAlmsUpdateTime();
	void SetLastAlmsUpdateTime(int iTime);

	//���ʣ��齱����
	int GetLotteryNum();
	void SetLotteryNum(int iRemainNum);

	//��ҳ�ֵ�齱��¼
	void SetRechargeLotteryRecord(const BASEDBINFO& stBaseInfo);
	void GetRechargeLotteryRecord(std::vector<int>& vRecords);
	void AddRechargeLotteryRecord(int iLotteryID);

	//��Ҳ�������ID
	unsigned GetTableID();
	void SetTableID(unsigned uTableID);

	//�����������Ϣ
	TEXPLINEINFO* GetExpLineInfo(int iAlgorithmType);
	void SetExpLineInfo(const BASEDBINFO& stBaseInfo);

	//��һ�����Ϣ
	TSCOREINFO& GetScoreInfo();
	void SetScoreInfo(const BASEDBINFO& stBaseInfo);
	void AddFishScore(int iScoreNum);

	//��ҵ�½����
	int GetLoginDays();
	void SetLoginDays(int iLoginDays);

	//�����ȡ��¼����
	int GetLoginReward();
	void SetLoginReward(int iLoginReward);
	bool HasGetLoginReward(int iGetDay);
	void SetLoginRewardStat(int iGetDay);

	//������ֺ��
	int GetRemainNewRedNum();
	void SetRemainNewRedNum(int iNum);
	int GetNowNewRedNum();
	void SetNowNewRedNum(int iNum);

	//����¿�
	int GetMonthEndTime();
	void SetMonthEndTime(int iTime);
	void AddMonthEndTime(int iAddTime);

	//����¿����ʱ��
	int GetLastMonthTime();
	void SetLastMonthTime(int iTime);

	//���������
	const char* GetChannel();
	void SetChannel(const std::string& strChannel);

	//���ͷ��
	const char* GetPicID();
	void SetPicID(const std::string& strPicID);

	//�Ƿ�ǿ�Ƹ������а�
	bool GetUpdateRank();
	void SetUpdateRank(bool bUpdateRank);

	//�Ƿ���VIP��Ҳ���
	bool GetIsVIPAddCoins();
	void SetIsVIPAddCoins(bool bVIPAddCoins);

	//���ʵ��״̬
	int GetRealNameStat();
	void SetRealNameStat(int iRealNameStat);

	//��ҵ�������ʱ��
	int GetDayOnlineTime();
	void SetDayOnlineTime(int iDayOnlineTime);

	//�������ʱ���������ʱ��
	int GetLastOnlineUpdate();
	void SetLastOnlineUpdate(int iTime);

	//����������������ID
	unsigned GetFinGuideIndexes();
	void SetFinGuideIndexes(unsigned uIndexes);
	void SetFinGuide(int iGuideID);
	bool IsGuideFin(int iGuideID);

	//����Ƿ����
	bool GetNameChanged();
	void SetNameChanged(bool bChanged);

	//��ҵ������˺�
	const char* GetAccount();
	void SetAccount(const char* szAccount);

	//����豸��
	const char* GetDeviceID();
	void SetDeviceID(const char* szDeviceID);

	//����ۼ���Ӯ
	long8 GetUserWinNum();
	void SetUserWinNum(long8 lWinNum);
	void AddUserWinNum(long8 lAddNum);
	void UpdateRoleWinNum();

	//�����Ϸǩ��
	const char* GetSign();
	void SetSign(const char* szSign);

	//��ҽ�����Ϣ
	TROLETALLYINFO& GetTallyInfo();
	
	//���½�����Ϣ
	void UpdateTallyInfo();

public:
    //������Ϣ�����ݿ��������
    void UpdateBaseInfoToDB(BASEDBINFO& rstBaseInfo);
    void InitBaseInfoFromDB(const BASEDBINFO& rstBaseInfo, const KickerInfo& stKicker);

///////////////////////////////////////////////////////////////////////
public:

    void SetKicker(const World_KickRole_Request& rstKicker) {m_stKicker.CopyFrom(rstKicker);}
    World_KickRole_Request& GetKicker() {return m_stKicker;}

///////////////////////////////////////////////////////////////////////
///
public:
    // ˢ����ҵ��������Ϣ
    void ActiveRefresh();
    bool IsUnactive();

public:
    // Tick ����
    void OnTick();

	//����GameRole
	void ResetRole();

public:
    // ������״̬
    void SetAAStatus(TAAStatus enAAStatus) {m_enAAStatus = enAAStatus;};
    TAAStatus GetAAStatus() {return m_enAAStatus;};

/////////////////////////////////////////////////////////////////////////////////////////
public:
    // ����ʹ��
    void PrintMyself();

public:
    // ����״̬
    void SetOnline();
    void SetOffline();
    bool IsOnline();

/////////////////////////////////////////////////////////////////////////////////////////
// ��Ա����
private:
    int m_iSessionID;

    //��ҽ�ɫ��������Ϣ
    TROLEINFO m_stRoleInfo;

    // ������¼
    int m_iLastActiveTime;

    // ��¼�������ߵ���
    World_KickRole_Request m_stKicker;

    // Tick��¼
    int m_iLastTickTime;
    int m_iLastUpdateDBTickTime;
    int m_iLastMinTime;			//1min ��ʱ��
    int m_iLastTenSecondTime;	//10s ��ʱ��
	int m_iLastFiveSecondTime;	//5s ��ʱ��
        
    // ������״̬
    TAAStatus m_enAAStatus;

public:
    bool IsLock(ENUM_LOCK eLock)
    {
        return m_abLock[eLock];
    }

    void UnLock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = false;
    }

    void Lock(ENUM_LOCK eLock)
    {
        m_abLock[eLock] = true;
    }

private:
    //��Ϊ��
    //  �����޴Σ���ֹ������������첽�����쳣�����ֵ��߸��Ƶ�����
    bool m_abLock[MAX_LOCK_NUM];

public:
    void SetLogoutReason(int iReason);
    int GetLogoutReason() {return m_iLogoutReason;}

private:
    int m_iLogoutReason;

public:
    bool IsMsgFreqLimit(const int iMsgID) const;
    void PushMsgID(const int iMsgID, const unsigned char ucSource);
private:

    //��Ϣ����
    MsgInfoQueue m_stMsgInfoQueue;

public:
	
	//��ұ���������
	CRepThingsManager& GetRepThingsManager();
	void UpdateRepThingsToDB(ITEMDBINFO& rstItemDBInfo);
	void InitRepThingsFromDB(const ITEMDBINFO& rstItemDBInfo);

	//������������
	CQuestManager& GetQuestManager();
	void UpdateQuestToDB(QUESTDBINFO& stQuestDBInfo);
	void InitQuestFromDB(const QUESTDBINFO& stQuestDBInfo);

	//��Ҷһ�������
	CExchangeManager& GetExchangeManager();
	void UpdateExchangeToDB(EXCHANGEDBINFO& stExchangeDBInfo);
	void InitExchangeFromDB(const EXCHANGEDBINFO& stExchangeDBInfo);

	//����ʼ�������
	CMailManager& GetMailManager();
	void UpdateMailToDB(MAILDBINFO& stMailDBInfo);
	void InitMailFromDB(const MAILDBINFO& stMailDBInfo);

	//��ҳ�ֵ������
	CRechargeManager& GetRechargeManager();
	void UpdateRechargeToDB(RECHARGEDBINFO& stRechargeInfo);
	void InitRechargeFromDB(const RECHARGEDBINFO& stRechargeInfo);

private:

	//����������
	CRepThingsManager	m_stRepThingsManager;

	//���������
	CQuestManager		m_stQuestManager;

	//�һ�������
	CExchangeManager	m_stExchangeManager;

	//�ʼ�������
	CMailManager		m_stMailManager;

	//��ֵ������
	CRechargeManager	m_stRechargeManager;
};

#endif
