#pragma once

#include <vector>
#include "GameProtocol.hpp"
#include "CommDefine.h"

//�һ���¼������
static const unsigned int MAX_EXCHANGE_RECORD_NUM = 100;

//�����һ�
struct ExchangeLimit
{
	int iExchangeID;		//�һ�ID
	int iLimitNum;			//����ʣ������

	ExchangeLimit()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

//�һ���¼
struct UserExchangeRec
{
	std::string strName;
	int iExchangeID;
	int iTime;

	UserExchangeRec()
	{
		Reset();
	}

	void Reset()
	{
		strName.c_str();
		iExchangeID = 0;
		iTime = 0;
	}
};

class CWorldExchangeLimitManager
{
public:
	static CWorldExchangeLimitManager* Instance();

	~CWorldExchangeLimitManager();

	//��ʼ��
	void Init();

	//��ȡ������Ϣ
	void GetExchangeLimit(Zone_GetLimitNum_Response& stResp);

	//�޸�������Ϣ
	int UpdateExchangeLimit(int iExchangeID, int iAddNum);

	//���Ӷһ���¼
	void AddExchangeRec(const ExchangeRec& stRecord);

	//��ȡ�һ���¼
	int GetExchangeRec(int iFromIndex, int iNum, Zone_GetExchangeRec_Response& stResp);

private:

	CWorldExchangeLimitManager();

	//����������Ϣ
	void LoadExchangeLimit();

	//����������Ϣ
	void SaveExchangeLimit();

	//���ضһ���¼
	void LoadExchangeRecord();

	//����һ���¼
	void SaveExchangeRecord();

	//������Ϣ�����ڣ������ö�ȡ
	void LoadLimitInfoFromConfig();

	//����
	void Reset();

private:

	//�����һ���Ϣ
	std::vector<ExchangeLimit> m_vWorldExchangeLimits;

	//�����һ���¼
	std::vector<UserExchangeRec> m_vUserExchangeRecs;
};
