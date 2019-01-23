#include <algorithm>
#include <fstream>

#include "json/json.h"
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"
#include "WorldMsgHelper.hpp"
#include "ConfigManager.hpp"

#include "WorldExchangeLimitManager.h"

using namespace ServerLib;

const static char* WORLD_EXCHANGE_LIMIT_FILE = "../conf/ExchangeLimit.dat";
const static char* WORLD_EXCHANGE_RECORD_FILE = "../conf/ExchangeRecord.dat";

CWorldExchangeLimitManager* CWorldExchangeLimitManager::Instance()
{
	static CWorldExchangeLimitManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CWorldExchangeLimitManager;
	}

	return pInstance;
}

CWorldExchangeLimitManager::CWorldExchangeLimitManager()
{
	Reset();
}

CWorldExchangeLimitManager::~CWorldExchangeLimitManager()
{
	Reset();
}

//��ʼ��
void CWorldExchangeLimitManager::Init()
{
	Reset();

	//����������Ϣ
	LoadExchangeLimit();

	//���ضһ���¼
	LoadExchangeRecord();

	return;
}

//��ȡ������Ϣ
void CWorldExchangeLimitManager::GetExchangeLimit(Zone_GetLimitNum_Response& stResp)
{
	for (unsigned i = 0; i < m_vWorldExchangeLimits.size(); ++i)
	{
		LimitInfo* pstOneInfo = stResp.add_stlimits();
		pstOneInfo->set_iexchangeid(m_vWorldExchangeLimits[i].iExchangeID);
		pstOneInfo->set_inum(m_vWorldExchangeLimits[i].iLimitNum);
	}

	return;
}

//�޸�������Ϣ
int CWorldExchangeLimitManager::UpdateExchangeLimit(int iExchangeID, int iAddNum)
{
	//�Ȳ��Ҷһ���Ϣ
	for (unsigned i = 0; i < m_vWorldExchangeLimits.size(); ++i)
	{
		if (m_vWorldExchangeLimits[i].iExchangeID != iExchangeID)
		{
			continue;
		}

		if ((m_vWorldExchangeLimits[i].iLimitNum + iAddNum) < 0)
		{
			//�����޸�
			return T_WORLD_PARA_ERROR;
		}

		m_vWorldExchangeLimits[i].iLimitNum += iAddNum;

		//���浽�ļ�
		SaveExchangeLimit();

		return T_SERVER_SUCCESS;
	}

	//�Ҳ��������
	if (iAddNum < 0)
	{
		return T_WORLD_PARA_ERROR;
	}

	ExchangeLimit stLimitInfo;
	stLimitInfo.iExchangeID = iExchangeID;
	stLimitInfo.iLimitNum = iAddNum;

	m_vWorldExchangeLimits.push_back(stLimitInfo);

	//���浽�ļ�
	SaveExchangeLimit();

	return T_SERVER_SUCCESS;
}

//���Ӷһ���¼
void CWorldExchangeLimitManager::AddExchangeRec(const ExchangeRec& stRecord)
{
	UserExchangeRec stExchangeRec;
	stExchangeRec.strName = stRecord.strname();
	stExchangeRec.iExchangeID = stRecord.iexchangeid();
	stExchangeRec.iTime = stRecord.itime();

	//������ǰ��
	m_vUserExchangeRecs.insert(m_vUserExchangeRecs.begin(), stExchangeRec);

	
	if (m_vUserExchangeRecs.size() > MAX_EXCHANGE_RECORD_NUM)
	{
		m_vUserExchangeRecs.resize(MAX_EXCHANGE_RECORD_NUM);
	}

	//���浽�ļ�
	SaveExchangeRecord();

	return;
}

//��ȡ�һ���¼
int CWorldExchangeLimitManager::GetExchangeRec(int iFromIndex, int iNum, Zone_GetExchangeRec_Response& stResp)
{
	if (iFromIndex <= 0 || iNum <= 0)
	{
		return T_WORLD_PARA_ERROR;
	}

	for (int i = (iFromIndex-1); i < (iFromIndex-1 + iNum) && i < (int)m_vUserExchangeRecs.size(); ++i)
	{
		ExchangeRec* pstOneRec = stResp.add_strecords();
		pstOneRec->set_strname(m_vUserExchangeRecs[i].strName);
		pstOneRec->set_iexchangeid(m_vUserExchangeRecs[i].iExchangeID);
		pstOneRec->set_itime(m_vUserExchangeRecs[i].iTime);
	}

	return T_SERVER_SUCCESS;
}

//����������Ϣ
void CWorldExchangeLimitManager::LoadExchangeLimit()
{
	//���ļ��ж�ȡRank��Ϣ
	std::ifstream is;
	is.open(WORLD_EXCHANGE_LIMIT_FILE, std::ios::binary);
	if (is.fail())
	{
		//������Ϣ�����ڣ������ö�ȡ
		LoadLimitInfoFromConfig();

		return;
	}

	Json::Reader jReader;
	Json::Value jValue;
	if (!jReader.parse(is, jValue))
	{
		//����json�ļ�ʧ��
		is.close();
		LOGERROR("Failed to parse exchange limit file %s\n", WORLD_EXCHANGE_LIMIT_FILE);
		return;
	}

	ExchangeLimit stOneInfo;
	for (unsigned i = 0; i < jValue.size(); ++i)
	{
		stOneInfo.Reset();
		stOneInfo.iExchangeID = jValue[i]["id"].asInt();
		stOneInfo.iLimitNum = jValue[i]["num"].asInt();

		m_vWorldExchangeLimits.push_back(stOneInfo);
	}

	is.close();

	return;
}

//����������Ϣ
void CWorldExchangeLimitManager::SaveExchangeLimit()
{
	//����Rank��Ϣ���ļ�
	std::ofstream os;
	os.open(WORLD_EXCHANGE_LIMIT_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open exchange limit file %s\n", WORLD_EXCHANGE_LIMIT_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;
	Json::Value jOneInfo;

	for(unsigned i=0; i<m_vWorldExchangeLimits.size(); ++i)
	{
		jOneInfo.clear();
		jOneInfo["id"] = m_vWorldExchangeLimits[i].iExchangeID;
		jOneInfo["num"] = m_vWorldExchangeLimits[i].iLimitNum;

		jValue.append(jOneInfo);
	}

	os << jValue.toStyledString();
	os.close();

	return;
}

//���ضһ���¼
void CWorldExchangeLimitManager::LoadExchangeRecord()
{
	//���ļ��ж�ȡ��¼��Ϣ
	std::ifstream is;
	is.open(WORLD_EXCHANGE_RECORD_FILE, std::ios::binary);
	if (is.fail())
	{
		return;
	}

	Json::Reader jReader;
	Json::Value jValue;
	if (!jReader.parse(is, jValue))
	{
		//����json�ļ�ʧ��
		is.close();
		LOGERROR("Failed to parse exchange record file %s\n", WORLD_EXCHANGE_RECORD_FILE);
		return;
	}

	UserExchangeRec stOneRec;
	for (unsigned i = 0; i < jValue.size(); ++i)
	{
		stOneRec.Reset();
		stOneRec.iExchangeID = jValue[i]["id"].asInt();
		stOneRec.strName = jValue[i]["name"].asString();
		stOneRec.iTime = jValue[i]["time"].asInt();

		m_vUserExchangeRecs.push_back(stOneRec);
	}

	is.close();

	return;
}

//����һ���¼
void CWorldExchangeLimitManager::SaveExchangeRecord()
{
	//����Rank��Ϣ���ļ�
	std::ofstream os;
	os.open(WORLD_EXCHANGE_RECORD_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open exchange record file %s\n", WORLD_EXCHANGE_RECORD_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;
	Json::Value jOneInfo;

	for (unsigned i = 0; i<m_vUserExchangeRecs.size(); ++i)
	{
		jOneInfo.clear();
		jOneInfo["id"] = m_vUserExchangeRecs[i].iExchangeID;
		jOneInfo["name"] = m_vUserExchangeRecs[i].strName;
		jOneInfo["time"] = m_vUserExchangeRecs[i].iTime;

		jValue.append(jOneInfo);
	}

	os << jValue.toStyledString();
	os.close();

	return;
}

//������Ϣ�����ڣ������ö�ȡ
void CWorldExchangeLimitManager::LoadLimitInfoFromConfig()
{
	//��ȡ��������
	std::vector<ExchangeConfig> vConfigs;
	CConfigManager::Instance()->GetBaseCfgMgr().GetExchangeConfig(true, vConfigs);

	//����������Ϣ
	ExchangeLimit stOneLimit;
	for (unsigned i = 0; i < vConfigs.size(); ++i)
	{
		stOneLimit.Reset();
		stOneLimit.iExchangeID = vConfigs[i].iID;
		stOneLimit.iLimitNum = vConfigs[i].iLimitNum;

		m_vWorldExchangeLimits.push_back(stOneLimit);
	}

	return;
}

//����
void CWorldExchangeLimitManager::Reset()
{
	m_vWorldExchangeLimits.clear();
	m_vUserExchangeRecs.clear();
}
