#pragma once

//��Ҷһ�������

#include <string>
#include <vector>

#include "CommDefine.h"
#include "GameProtocol.hpp"
#include "Kernel/ConfigManager.hpp"

//�һ���ͷID
static const int EXCHANGE_WARHEAD_ID = 3;
//�һ�5Ԫ 10ԪID
static const int EXCHANGE_FIVEBILL_ID = 50003;
static const int EXCHANGE_TENBILL_ID = 50004;

enum PersonLimitType
{
	PERSONLIMIT_TYPE_INVALID = 0,	//�Ƿ��ĳ�ʼ������
	PERSONLIMIT_TYPE_FIVEBILL = 1,	//��ʼ��5Ԫ������Ϣ
	PERSONLIMIT_TYPE_TENBILL = 2,	//��ʼ��10Ԫ������Ϣ
};

//���˶һ���Ϣ
struct UserExchangeData
{
	std::string strName;	//�һ�����
	std::string strPhone;	//�һ��绰
	std::string strMailNum;	//�һ��ʱ�
	std::string strAddress;	//�һ���ַ
	std::string strRemarks;	//��ұ�ע
	std::string strQQNum;	//���QQ��
	int iLastSetTime;		//�ϴ�����ʱ��
	unsigned int auPersonLimit[2];	//��Ҹ�������

	UserExchangeData()
	{
		Reset();
	}

	void Reset()
	{
		strName.clear();
		strPhone.clear();
		strMailNum.clear();
		strAddress.clear();
		strRemarks.clear();
		strQQNum.clear();

		iLastSetTime = 0;
		memset(auPersonLimit, 0, sizeof(auPersonLimit));
	}
};

//���˶�����Ϣ
struct OrderData
{
	int iExchangeID;		//�һ���ID
	std::string strOrderID;	//������
	std::string strPhone;	//�һ��ֻ���
	int iExchangeTime;		//�һ�ʱ��
	int iStat;				//����״̬

	OrderData()
	{
		Reset();
	}

	void Reset()
	{
		iExchangeID = 0;
		strOrderID.clear();
		strPhone.clear();
		iExchangeTime = 0;
		iStat = 0;
	}
};

class CGameRoleObj;
class CExchangeManager
{
public:
	CExchangeManager();
	~CExchangeManager();

public:

	//��ʼ��
	int Initialize();

	void SetOwner(unsigned int uin);
	CGameRoleObj* GetOwner();

	//���öһ���Ϣ
	int SetUserExchangeInfo(const ExchangeUser& stUserInfo, Zone_SetExchange_Response& stResp);

	//��Ҷһ�
	int ExchangeItem(const ExchangeConfig& stConfig, ExchangeOrder& stOrderInfo);

	//�۳��һ�����
	int DoExchangeCost(const ExchangeConfig& stConfig, bool bReturn = false);

	//�û���Ϣ�Ƿ�������
	bool IsUserInfoSet();

	//��ʼ������������Ϣ
	unsigned GetPersonLimitInfo(int iLimitType);

	//�Ƿ�������������һ�
	bool CheckIsLimit(int iLimitType);

	//�һ�������˶һ�����
	void SetPersonLimit(int iLimitType, unsigned iNum);

	//������ȡ���ܵ�����
	static int SendGetCardNoRequest(unsigned uin, const ExchangeConfig& stConfig);

	//�һ���Ϣ���ݿ��������
	void UpdateExchangeToDB(EXCHANGEDBINFO& stExchangeInfo);
	void InitExchangeFromDB(const EXCHANGEDBINFO& stExchangeInfo);

private:

	//��ȡ������
	std::string GetOrderID(int iTimeNow);

private:
	
	//���uin
	unsigned m_uiUin;

	//�һ���Ϣ
	UserExchangeData m_stExchangeData;

	//�һ�������Ϣ
	std::vector<OrderData> m_vOrderData;
};
