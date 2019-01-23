#pragma once

#include <string>
#include <vector>
#include <string.h>

//���ù��ñ����ͽṹ����

typedef long long long8;			//long8���Ͷ���
typedef unsigned long long ulong8;	//ulong8���Ͷ���
typedef unsigned char BYTE;			//BYTE���Ͷ���
typedef unsigned short USHORT;		//USHORT���Ͷ���

static const unsigned SERVER_BUSID_LEN = 64;			// ������BUS ID�ĳ���
static const unsigned DATE_TIME_LEN = 64;				// ������ʱ���ַ����ĳ���
static const unsigned MAX_IP_ADDRESS_LENGTH = 64;		// IP��ַ��󳤶�
static const unsigned MAX_SERVER_BUS_ADDR_LEN = 64;		// ��������ZMQ BUS�ĵ�ַ����󳤶ȣ���ʽΪ ip:port
static const unsigned MAX_SESSIONKEY_LENGTH = 64;		// ���session key����󳤶�
static const unsigned ROLE_TABLE_SPLIT_FACTOR = 8;		// ROLEDB���ݿ���ʵķֱ�����
static const unsigned REGAUTH_TABLE_SPLIT_FACTOR = 8;	// RegAuthDB���ݿ���ʵķֱ�����
static const unsigned NAME_TABLE_SPLIT_FACTOR = 8;		// NameDB���ݿ���ʵķֱ�����
static const unsigned LOG_TABLE_SPLIT_FACTOR = 8;		// LogServer���ݿ���ʵķֱ�����
static const unsigned MAX_NICK_NAME_LENGTH = 128;		// ������ֵ���󳤶�
static const unsigned GAME_START_YEAR = 2017;			// ��Ϸ��ʽ��ʼ��Ӫ�����
static const unsigned UNACTIVE_ROLE_TIMEOUT = 7200;		// �ж���Ҳ���Ծ�ĳ�ʱʱ��,120*60s
static const unsigned TICK_TIMEVAL_UPDATEDB = 300;		// ����������ݿ��ʱ������5*60s
static const unsigned ONE_MIN_TIMEOUT = 60;				// һ���Ӷ�ʱ
static const unsigned TEN_SECOND_TIMER = 10;			// 10���Ӷ�ʱ

static const int MAX_HANDLER_NUMBER = 16384;			// ������֧�ֵ�������Ϣ����Handler������
static const int MAX_ZONE_PER_WORLD = 8;				// ��ǰ����������֧�ֵ�������
static const int MAX_REP_BLOCK_NUM = 500;				// �����󱳰�������
static const int MAX_EXCHANGE_ORDER_NUM = 50;			//�������ʾ�Ķһ�������
static const int MAX_ROOM_ALGORITHM_TYPE = 6;			//���֧�ֵķ����㷨����
static const int MAX_APPENDIX_NUM = 4;					//�����ʼ������������
static const int MAX_ROLE_MAIL_NUM = 500;				//�������ʼ�����
static const int MAX_RANK_INFO_NUM = 100;				//���֧�ֵ������������
static const int GOLD_WARHEAD_ID = 10003;				//�ƽ�ͷID
static const int WARHEAD_INRANK_NUM = 10;				//�����а���Ҫ�Ļƽ�ͷ��
static const int AIM_FISH_ITEM_ID = 30001;				//��׼���ߵ�ID
static const int MAX_PAYLOTTERY_NUM = 20;				//��ҳ齱��¼����
static const int RECHARGE_LOTTERY_COSTNUM = 30;			//��ֵ�齱����30��ֵ���֣�1����=��ֵ1RMB
static const int MAX_RECHARGE_LOTTERY_RECORD = 200;		//��ֵ�齱��¼����
static const int MAX_LASVEGAS_RECORD_NUM = 100;			//��ת������н���¼����
static const int MAX_LASVEGAS_LOTTERY_NUM = 20;			//��ת�̿�����¼����
static const int MAX_RECHARGE_RECORD_NUM = 50;			//��ҳ�ֵ��¼����
static const int MAX_APP_THREAD_NUM = 8;				//AppThread����߳���	

enum ServerState
{
	SERVER_STATE_IDLE = 1,
	SERVER_STATE_BUSY = 2,
	SERVER_STATE_FULL = 3,
};

// Server������ID
typedef enum enGameServerID
{
	GAME_SERVER_UNDEFINE	= 0,
	GAME_SERVER_WORLD		= 1,
	GAME_SERVER_ZONE		= 2,
	GAME_SERVER_ROLEDB		= 3,
	GAME_SERVER_LOTUS		= 4,
	GAME_SERVER_REGAUTH		= 5,
	GAME_SERVER_REGAUTHDB	= 6,
	GAME_SERVER_NAMEDB		= 7,
	GAME_SERVER_RECHARGE	= 8,
	GAME_SERVER_PLATFORM	= 9,
	GAME_SERVER_EXCHANGE	= 10,
	GAME_SERVER_LOGSERVER	= 11,
	GAME_SERVER_MAX,
} EGameServerID;

typedef enum enServerStatus
{
	// ����������
	// 1) Zone��World����������Ϣ
	// 2) World��Cluster����������Ϣ
	GAME_SERVER_STATUS_INIT = 1,

	// �ָ�������
	GAME_SERVER_STATUS_RESUME = 2,

	// ����������
	GAME_SERVER_STATUS_IDLE = 3,

	// æ������
	GAME_SERVER_STATUS_BUSY = 4,

	// ׼��ֹͣ
	// 1) ֪ͨ�ͻ���׼��ͣ������
	// 2) ��ʼ����STOP״̬����ʱ
	GAME_SERVER_STATUS_PRESTOP = 5,

	// ֹͣ��
	// 1) ֪ͨLotusֹͣ���е��������
	// 2) �������е�Lotus����
	// 3) �������е�TBus����
	// 4) �������������������, ��֪ͨLotus�Ͽ�����
	// 5) ֹͣLotus��Zone������
	GAME_SERVER_STATUS_STOP = 6,

} EGameServerStatus;

enum GameUnitType
{
	EUT_ROLE = 1, // ��ɫ
};

enum EUnitStatus
{
	EGUS_ONLINE = 1,
	EGUS_LOGOUT = 2,
	EGUS_ISGM = 4,
	EGUS_DELETE = 8,
};

static const int NETHEAD_V2_SIZE = 24;	//����ͷ

struct TNetHead_V2
{
	unsigned int	m_uiSocketFD;	//�׽���
	unsigned int	m_uiSocketTime;	//�׽��ִ���ʱ��
	unsigned int	m_uiSrcIP;		//Դ��ַ
	unsigned short	m_ushSrcPort;	//Դ�˿�
	unsigned short	m_ushReservedValue01; //�ֽڶ��룬δ��
	unsigned int	m_uiCodeTime;	//��Ϣʱ��
	unsigned int	m_uiCodeFlag;	//��Ϣ��־������ʵ���׽��ֿ���

	TNetHead_V2()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ��̬�ڴ����ʹ������

//////////////////////////////////////////
// Zone
/////////////////////////////////////////

// ��ɫ����
const int MAX_ROLE_OBJECT_NUMBER_IN_ZONE = 4000;

// ��λID
const int MAX_UNIT_NUMBER_IN_ZONE = (MAX_ROLE_OBJECT_NUMBER_IN_ZONE) * 2;

//��ض���
const int MAX_FISHPOND_NUMBER_IN_ZONE = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

//////////////////////////////////////////
// World
/////////////////////////////////////////
const int MAX_ROLE_OBJECT_NUMBER_IN_WORLD = MAX_ROLE_OBJECT_NUMBER_IN_ZONE * 4;

//////////////////////////////////////////
// Name
///////////////////////////////////////
//��HashMap�ڵ��С
const int MAX_ROLE_NAME_NUMBER = 100000;

//////////////////////////////////////////
//RegAuth
const int MAX_ACCOUNT_OBJ_CACHE_NUMBER = 5000;

enum ResourceType
{
	RESOURCE_TYPE_INVALID	= 0,	// �Ƿ�����Դ����
	RESOURCE_TYPE_COIN		= 1,	// ��Ϸ���
	RESOURCE_TYPE_DIAMOND	= 2,	// ��Ϸ��ֵ��ʯ
	RESOURCE_TYPE_TICKET	= 3,	//��Ϸ����Ʊ
	RESOURCE_TYPE_LIVENESS	= 4,	//��Ϸ�ڻ�Ծ��
	RESOURCE_TYPE_ACHEIVE	= 5,	//��Ϸ�ڳɾ͵�
	RESOURCE_TYPE_PAYSCORE	= 6,	//��ֵ�齱����
	RESOURCE_TYPE_MAX,				// ��Ϸ��Դ���
};

//�����������
enum FishItemType
{
	FISH_ITEM_INVALID	= 0,	//�Ƿ��ĵ�������
	FISH_ITEM_BOX		= 1,	//�����������
	FISH_ITEM_WARHEAD	= 2,	//��ͷ��������
	FISH_ITEM_SKILL		= 3,	//���ܵ�������
	FISH_ITEM_STYLE		= 5,	//��̨��ʽ��������
	FISH_ITEM_MONTH		= 6,	//�¿�����
	FISH_ITEM_USE		= 7,	//��ʹ�õ�������
};

//����VIP��Ȩ����
enum VIPPrivType
{
	VIP_PRIV_GETALMS	= 0x01,	//��ȡ�ȼý�
	VIP_PRIV_GETITEM	= 0x02,	//�������߽���
	VIP_PRIV_LOTTERY	= 0x04,	//�������س齱
	VIP_PRIV_WILDNUM	= 0x08,	//�񱩵��Ӵ���
	VIP_PRIV_ADDCOINS	= 0x10,	//ÿ���״ε�½������
	VIP_PRIV_SENDGIFT	= 0x20,	//�������͹���
	VIP_PRIV_EXCHANGE	= 0x40,	//������ͷ�һ�����
	VIP_PRIV_CUSTOMIZE	= 0x80,	//����˽�˶���
};

//�ϱ���־����
enum ReportLogType
{
	REPORT_LOG_INVALID		= 0,	//�Ƿ�����־����
	REPORT_LOG_MOFANGLOGIN	= 1,	//ħ����¼��־
	REPORT_LOG_MOFANGPAY	= 2,	//ħ����ֵ��־
	REPORT_LOG_TALLYINFO	= 3,	//��ҽ�����־
};

//��ҵ�ͷ����
enum WarheadType
{
	WARHEAD_TYPE_INVALID	= 0,	//�Ƿ��ĵ�ͷ����
	WARHEAD_TYPE_BRONZE		= 1,	//��ͭ��ͷ
	WARHEAD_TYPE_SILVER		= 2,	//������ͷ
	WARHEAD_TYPE_GOLD		= 3,	//�ƽ�ͷ
	WARHEAD_TYPE_MAX		= 4,	//��ͷ��󣬳���ʹ��
};

//��ҵ�����������
enum ItemChannelType
{
	ITEM_CHANNEL_INVALID	= 0,	//�Ƿ�������
	ITEM_CHANNEL_EXCHANGE	= 1,	//�һ����ĵ���
	ITEM_CHANNEL_GMADD		= 2,	//GM�޸ĵ���
	ITEM_CHANNEL_FISHADD	= 3,	//�����õ���
	ITEM_CHANNEL_USEITEM	= 4,	//ʹ�õ���
	ITEM_CHANNEL_QUEST		= 5,	//�����õ���
	ITEM_CHANNEL_SENDGIFT	= 6,	//�����޸ĵ���
	ITEM_CHANNEL_ROLEMAIL	= 7,	//�����ʼ����
	ITEM_CHANNEL_SYSMAIL	= 8,	//ϵͳ�ʼ����
	ITEM_CHANNEL_OPENBOX	= 9,	//�������õ���
	ITEM_CHANNEL_LOTTERY	= 10,	//�齱��õ���
	ITEM_CHANNEL_LOGINADD	= 11,	//��¼��������
};

//���λ��
struct FISHPOS
{
	int iX;		//X����
	int iY;		//Y����

	FISHPOS()
	{
		memset(this, 0, sizeof(*this));
	}

	FISHPOS(int iPosX, int iPosY)
	{
		iX = iPosX;
		iY = iPosY;
	}

	FISHPOS& operator=(const FISHPOS& stPos)
	{
		if (this == &stPos)
		{
			return *this;
		}

		this->iX = stPos.iX;
		this->iY = stPos.iY;

		return *this;
	}

	bool operator==(const FISHPOS& stPos)
	{
		if (this == &stPos)
		{
			return true;
		}

		return ((this->iX == stPos.iX) && (this->iY == stPos.iY));
	}
};

//������Ϣ
struct RankData
{
	unsigned uiUin;				//���ID
	std::string strNickName;	//����ǳ�
	std::string strPicID;		//���ͷ��ID
	int iVIPLevel;				//���VIP�ȼ�
	long8 iNum;					//���ӵ�е����������,��ͷ�����
	std::string strSign;		//�����Ϸǩ��

	RankData()
	{
		Reset();
	}

	void Reset()
	{
		uiUin = 0;
		strNickName.clear();
		strPicID.clear();
		iVIPLevel = 0;
		iNum = 0;
		strSign.clear();
	}

	bool operator==(const RankData& stData) const
	{
		return (this->uiUin == stData.uiUin);
	}
};

//���а��б�
struct RankList
{
	unsigned uVersionID;					//��ǰ���а�汾ID���и���+1
	std::vector<RankData> vRankInfos;		//������Ϣ
	std::vector<RankData> vLastRankInfos;	//�ϸ��������а���Ϣ
	int iLastUpdateTime;					//���а��ϴθ���ʱ��

	RankList()
	{
		Reset();
	}

	void Reset()
	{
		uVersionID = 1;		//��1��ʼ
		iLastUpdateTime = 0;
		vRankInfos.clear();
		vLastRankInfos.clear();
	}
};

//���ת���н���Ϣ
struct LotteryPrizeData
{
	std::string strNickName;	//�������
	int iNumber;				//��������
	int iRewardCoins;			//�н����

	LotteryPrizeData()
	{
		Reset();
	}

	void Reset()
	{
		strNickName.clear();
		iNumber = 0;
		iRewardCoins = 0;
	}
};

//ת�̽�����ע��Ϣ
struct LasvegasBetData
{
	int iNumber;		//���������
	int iBetCoins;	//��ע���

	LasvegasBetData()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//��������
struct RewardConfig
{
	int iType;		//��������
	int iRewardID;	//����ID���������ͱ�ʾ���߻���Դ
	int iRewardNum;	//��������

	RewardConfig()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�ʼ���Ϣ
struct MailData
{
	unsigned uUniqID;					//�ʼ�ΨһID
	int iMailID;						//�ʼ�����ID
	int iSendTime;						//�ʼ�����ʱ��
	int iMailStat;						//�ʼ�״̬
	std::string strTitle;				//�ʼ�����
	std::vector<std::string> vParams;	//�ʼ�����
	std::vector<RewardConfig> vAppendixes;//�ʼ�����

	MailData()
	{
		Reset();
	}

	void Reset()
	{
		uUniqID = 0;
		iMailID = 0;
		iSendTime = 0;
		iMailStat = 0;
		strTitle.clear();
		vParams.clear();
		vAppendixes.clear();
	}
};
