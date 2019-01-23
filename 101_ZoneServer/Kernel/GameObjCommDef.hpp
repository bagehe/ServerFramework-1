#pragma once

//���ļ�����Ҫ������Ϸ��ʹ�õĵ�λ�ͽ�ɫ�Ļ������ݽṹ

#include "GameProtocol.hpp"
#include "CommDefine.h"

//�����ϵ�λ�Ķ�����Ϣ
struct TUNITINFO
{
    int iUnitID;                    //������λ��ID
    unsigned int uiUnitStatus;      //������λ��ǰ��״̬

    TUNITINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//�����̨��Ϣ
struct TWEAPONINFO
{
	int iWeaponID;				//�����̨ID
	int iStyleID;				//�����̨��ʽID
	int aiUnlockStyleIDs[16];	//�ѽ�����̨ID

	TWEAPONINFO()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�����������Ϣ
struct TEXPLINEINFO
{
	int iExpLineID;			//������ID
	int iExpLineType;		//����������
	long8 lUserWinNum;		//������������
	long8 lIntervalWinNum;	//�������ڽ׶�����
	int iBulletNum;			//�������ڷ����ӵ�����
	long8 lCostNum;			//�������ڷ����ӵ�����

	TEXPLINEINFO()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//��һ�����Ϣ
struct TSCOREINFO
{
	int iLastScoreUpdate;		//�ϴλ��ָ���ʱ��
	long8 lDayScore;			//���ջ���
	long8 lWeekScore;			//���ܻ���

	TSCOREINFO()
	{
		memset(this, 0, sizeof(*this));
	}
};

//��ҽ�����Ϣ
struct TROLETALLYINFO 
{
	long8 alResource[RESOURCE_TYPE_MAX];	//�ϴν�����Դ����
	int aiBindWarheadNum[WARHEAD_TYPE_MAX];	//�ϴν���󶨵�ͷ����
	int aiWarheadNum[WARHEAD_TYPE_MAX];		//�ϴν���ǰ󶨵�ͷ����
	int aiSendGiftNum[WARHEAD_TYPE_MAX];	//���������ͷǰ󶨵�ͷ����
	int aiRecvGiftNum[WARHEAD_TYPE_MAX];	//�����ڽ��շǰ󶨵�ͷ����
	int iRechargeDiamonds;					//��������ҳ�ֵ��ʯ����
	long8 lCashTicketNum;					//���������������Ʊ��ֵ������ʵ��ͻ��ѵ���
	long8 lUserWinNum;						//���������ʵ����Ӯ
	bool bNeedLog;							//�������Ƿ���Ҫ��¼

	TROLETALLYINFO()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//��ҽ�ɫ������Ϣ�ṹ����
struct TROLEBASEINFO
{
    char szNickName[MAX_NICK_NAME_LENGTH];  //��ҵ�����
    int iLastLogin;							//����ϴε�¼��ʱ��
    int iLastLogout;						//����ϴεǳ���Ϸ��ʱ��
    int iCreateTime;						//����ʺŴ�����ʱ��
    int iOnlineTime;						//��ҵ�������ʱ��
    int iLoginCount;						//����ܵĵ�¼����
    int iLoginTime;							//��ұ��ε�¼��ʱ��
    int iLogoutTime;						//��ұ��εǳ���Ϸ��ʱ��
	TWEAPONINFO stWeaponInfo;				//���ʹ����̨��Ϣ
	long8 alResource[RESOURCE_TYPE_MAX];	//��ҵ���Դ��Ϣ
	int iVIPLevel;							//���VIP�ȼ�
	long8 lVIPExp;							//���VIP����
	int iVIPPriv;							//���VIP��Ȩ
	int iLotteryNum;						//��ҵ����ѳ齱����
	unsigned uTableID;						//������ڲ�������ID
	TEXPLINEINFO astExpLineInfo[MAX_ROOM_ALGORITHM_TYPE];//�����������Ϣ	
	TSCOREINFO stScoreInfo;					//��һ�����Ϣ
	int iNextAlmsTime;						//�ȼý��´ο���ȡʱ��
	int iAlmsNum;							//��ҵ���ȼý���ȡ����
	int iLastAlmsUpdateTime;				//����ϴξȼý���Ϣ����ʱ��
	int aiLotteryIDs[MAX_PAYLOTTERY_NUM];	//��ҳ�ֵ�齱��¼
	int iLoginDays;							//����ۼƵ�¼������������
	int iGetLoginReward;					//��ҵ�½�����Ƿ���ȡ����λ����
	int iRemainNewRedNum;					//���ʣ�����ֺ�����
	int iNowNewRedNum;						//��ҵ�ǰ�������ֺ�����
	int iMonthEndTime;						//����¿�����ʱ��
	int iLastMonthTime;						//����ϴ���ȡ�¿����ʱ��
	char szChannel[32];						//��ҵ�������
	char szPicID[256];						//���ͷ��ID
	bool bUpdateRank;						//�Ƿ�ǿ�Ƹ������а�
	bool bGetVIPCoins;						//����Ƿ���VIP��Ҳ���
	int iRealNameStat;						//���ʵ��״̬
	int iDayOnlineTime;						//��ҵ�������ʱ��
	int iLastOnlineUpdateTime;				//�������ʱ���������ʱ��
	unsigned uFinGuideIndexes;				//��������������ID,λ�����ʾ
	bool bNameChanged;						//����Ƿ��޸Ĺ�����
	char szAccount[128];					//��ҵ��˺�
	char szDeviceID[128];					//��ҵ��豸��
	TROLETALLYINFO stTallyInfo;				//��ҽ�����Ϣ
	long8 lUserWinNum;						//���ʵ����Ӯ
	char szSign[128];						//�����Ϸǩ��

    TROLEBASEINFO()
    {
        memset(this, 0, sizeof(*this));
    };
};

//��ɫ��������Ϣ
struct TROLEINFO
{
    RoleID stRoleID;            //��ɫID
    TROLEBASEINFO stBaseInfo;   //��ɫ�Ļ�����Ϣ
    TUNITINFO stUnitInfo;       //��ɫ��Unit��λ��Ϣ
};
