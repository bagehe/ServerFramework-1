#include <algorithm>
#include <fstream>

#include "json/json.h"
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"
#include "Random.hpp"
#include "WorldMsgHelper.hpp"
#include "ConfigManager.hpp"

#include "WorldLimitLotteryManager.h"

static const char* WORLD_LOTTERY_RECORD_FILE = "../conf/LotteryRecord.dat";

CWorldLimitLotteryManager* CWorldLimitLotteryManager::Instance()
{
	static CWorldLimitLotteryManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CWorldLimitLotteryManager;
	}

	return pInstance;
}

CWorldLimitLotteryManager::CWorldLimitLotteryManager()
{
	Reset();
}

CWorldLimitLotteryManager::~CWorldLimitLotteryManager()
{
	Reset();
}

//��ʼ��
void CWorldLimitLotteryManager::Init()
{
	//��ʼ��������Ϣ
	ResetLotteryInfo(true);

	//������ʷ��¼
	LoadRechargeLotteryRecord();
}

//�����齱
void CWorldLimitLotteryManager::LimitLottery(int iLimitType, const std::string& strNickName, bool bIsTenTimes, std::vector<int>& vLotteryIDs)
{
	//����������Ϣ
	ResetLotteryInfo(false);

	vLotteryIDs.clear();

	//����齱�߼�
	int iLotteryID = 0;
	int iLotteryTimes = bIsTenTimes ? 10 : 1;
	for (int i = 0; i < iLotteryTimes; ++i)
	{
		iLotteryID = LotteryOneTime(iLimitType);
		if (iLotteryID != 0)
		{
			vLotteryIDs.push_back(iLotteryID);
		}
	}

	if (iLimitType == LIMIT_LOTTERY_RECHARGE)
	{
		//���ӳ�ֵ�齱��¼
		AddLotteryRecord(strNickName, vLotteryIDs);
	}

	return;
}

//��ȡ��ֵ��¼
void CWorldLimitLotteryManager::GetLotteryRecord(int iFrom, int iNum, Zone_PayLotteryRecord_Response& stResp)
{
	int iTotalNum = m_vLotteryRecord.size();
	for (int i = 0; i < iNum; ++i)
	{
		if ((iFrom + i) <= 0 || (iFrom + i) > iTotalNum)
		{
			break;
		}

		//���Ӽ�¼
		PayLotteryRecord* pstOneRecord = stResp.add_strecords();
		pstOneRecord->set_strnickname(m_vLotteryRecord[iFrom+i-1].strNickName);
		pstOneRecord->set_ilotteryid(m_vLotteryRecord[iFrom + i - 1].iLotteryID);
		pstOneRecord->set_iindex(iFrom+i);
	}

	return;
}

//���س齱��¼
void CWorldLimitLotteryManager::LoadRechargeLotteryRecord()
{
	//���ļ��ж�ȡRank��Ϣ
	std::ifstream is;
	is.open(WORLD_LOTTERY_RECORD_FILE, std::ios::binary);
	if (is.fail())
	{
		//���ļ�ʧ��
		return;
	}

	Json::Reader jReader;
	Json::Value jValue;
	if (!jReader.parse(is, jValue))
	{
		//����json�ļ�ʧ��
		is.close();
		LOGERROR("Failed to parse lottery record file %s\n", WORLD_LOTTERY_RECORD_FILE);
		return;
	}

	m_vLotteryRecord.clear();

	RechargeLotteryRecord stOneRecord;
	for (unsigned i = 0; i < jValue.size(); ++i)
	{
		stOneRecord.strNickName = jValue[i]["name"].asString();
		stOneRecord.iLotteryID = jValue[i]["id"].asInt();

		m_vLotteryRecord.push_back(stOneRecord);
	}

	is.close();

	return;
}

//����齱��¼
void CWorldLimitLotteryManager::SaveRechargeLotterRecord()
{
	//����Rank��Ϣ���ļ�
	std::ofstream os;
	os.open(WORLD_LOTTERY_RECORD_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open lottery record file %s\n", WORLD_LOTTERY_RECORD_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;
	Json::Value jOneInfo;

	for (unsigned i = 0; i<m_vLotteryRecord.size(); ++i)
	{
		jOneInfo.clear();
		jOneInfo["name"] = m_vLotteryRecord[i].strNickName;
		jOneInfo["id"] = m_vLotteryRecord[i].iLotteryID;

		jValue.append(jOneInfo);
	}

	os << jValue.toStyledString();
	os.close();

	return;
}

//�������ó齱��Ϣ
void CWorldLimitLotteryManager::ResetLotteryInfo(bool bIsInit)
{
	LimitLotteryInfo stOneInfo;
	int iTimeNow = CTimeUtility::GetNowTime();

	for (int i = LIMIT_LOTTERY_INVALID + 1; i < LIMIT_LOTTERY_MAX; ++i)
	{
		const std::vector<LimitLotteryConfig>* pvConfigs = CConfigManager::Instance()->GetBaseCfgMgr().GetLimitLotteryConfig(i);
		if (!pvConfigs)
		{
			continue;
		}

		if (bIsInit)
		{
			m_avLotteryInfo[i].clear();

			//��ʼ��
			for (unsigned j = 0; j < pvConfigs->size(); ++j)
			{
				if ((*pvConfigs)[j].iLimitType == LIMIT_TYPE_CLOSED)
				{
					//��ѡ��δ����
					continue;
				}

				stOneInfo.Reset();
				stOneInfo.iLotteryID = (*pvConfigs)[j].iID;
				stOneInfo.iWeight = (*pvConfigs)[j].iWeight;
				stOneInfo.iLimitType = (*pvConfigs)[j].iLimitType;
				stOneInfo.iConfigDayLimit = (*pvConfigs)[j].iDayNum;
				stOneInfo.iDailyLimit = (*pvConfigs)[j].iDayNum;
				stOneInfo.iTotalLimit = (*pvConfigs)[j].iTotalNum;

				m_avLotteryInfo[i].push_back(stOneInfo);
			}
		}
		else
		{
			//���ǳ�ʼ��������ÿ������
			if (CTimeUtility::IsInSameDay(iTimeNow, m_iLastUpdateTime))
			{
				//���Ǹ��죬������
				return;
			}

			for (unsigned j = 0; j < m_avLotteryInfo[i].size(); ++j)
			{
				switch (m_avLotteryInfo[i][j].iLimitType)
				{
				case LIMIT_TYPE_DAYLIMIT:
				{
					//ÿ������
					m_avLotteryInfo[i][j].iDailyLimit = m_avLotteryInfo[i][j].iConfigDayLimit;
				}
				break;

				case LIMIT_TYPE_LIMITALL:
				{
					//��������
					if (m_avLotteryInfo[i][j].iConfigDayLimit < m_avLotteryInfo[i][j].iTotalLimit)
					{
						m_avLotteryInfo[i][j].iDailyLimit = m_avLotteryInfo[i][j].iConfigDayLimit;
					}
					else
					{
						m_avLotteryInfo[i][j].iDailyLimit = m_avLotteryInfo[i][j].iTotalLimit;
					}
				}
				break;

				default:
				{

				}
				break;
				}
				
			}
		}
	}

	m_iLastUpdateTime = iTimeNow;

	return;
}

//һ�γ齱�����س齱���
int CWorldLimitLotteryManager::LotteryOneTime(int iLimitType)
{
	if (iLimitType <= LIMIT_LOTTERY_INVALID || iLimitType >= LIMIT_LOTTERY_MAX)
	{
		//�Ƿ�������
		return 0;
	}

	//��ʼ�齱
	int iRandMax = 0;
	for (unsigned i = 0; i < m_avLotteryInfo[iLimitType].size(); ++i)
	{
		switch (m_avLotteryInfo[iLimitType][i].iLimitType)
		{
		case LIMIT_TYPE_DAYLIMIT:
		case LIMIT_TYPE_LIMITALL:
		{
			//ÿ������
			if (m_avLotteryInfo[iLimitType][i].iDailyLimit == 0)
			{
				continue;
			}
		}
		break;

		case LIMIT_TYPE_TOTALLIMIT:
		{
			//������
			if (m_avLotteryInfo[iLimitType][i].iTotalLimit == 0)
			{
				continue;
			}
		}
		break;

		case LIMIT_TYPE_UNLIMIT:
		{
			//������
		}
		break;

		default:
		{
			//�Ƿ�������
			continue;
		}
		break;
		}

		iRandMax += m_avLotteryInfo[iLimitType][i].iWeight;
	}

	int iRandNum = CRandomCalculator::GetRandomNumberInRange(iRandMax);
	LimitLotteryInfo* pstInfo = NULL;
	for (unsigned i = 0; i < m_avLotteryInfo[iLimitType].size(); ++i)
	{
		switch (m_avLotteryInfo[iLimitType][i].iLimitType)
		{
		case LIMIT_TYPE_DAYLIMIT:
		case LIMIT_TYPE_LIMITALL:
		{
			//ÿ������
			if (m_avLotteryInfo[iLimitType][i].iDailyLimit == 0)
			{
				continue;
			}
		}
		break;

		case LIMIT_TYPE_TOTALLIMIT:
		{
			//������
			if (m_avLotteryInfo[iLimitType][i].iTotalLimit == 0)
			{
				continue;
			}
		}
		break;

		case LIMIT_TYPE_UNLIMIT:
		{
			//������
		}
		break;

		default:
		{
			//�Ƿ�������
			continue;
		}
		break;
		}

		if (iRandNum >= m_avLotteryInfo[iLimitType][i].iWeight)
		{
			iRandNum -= m_avLotteryInfo[iLimitType][i].iWeight;
			continue;
		}

		//�ҵ�����Ϣ
		pstInfo = &m_avLotteryInfo[iLimitType][i];
		break;
	}

	if (!pstInfo)
	{
		//�Ҳ�������
		return 0;
	}

	//�۳�����
	switch (pstInfo->iLimitType)
	{
	case LIMIT_TYPE_DAYLIMIT:
	{
		//ÿ������
		--pstInfo->iDailyLimit;
	}
	break;

	case LIMIT_TYPE_TOTALLIMIT:
	{
		//������
		--pstInfo->iTotalLimit;
	}
	break;

	case LIMIT_TYPE_LIMITALL:
	{
		//��������
		--pstInfo->iDailyLimit;
		--pstInfo->iTotalLimit;
	}
	break;

	case LIMIT_TYPE_UNLIMIT:
	{
		//������
	}
	break;

	default:
	{

	}
	break;
	}

	return pstInfo->iLotteryID;
}

//���ӳ齱��¼
void CWorldLimitLotteryManager::AddLotteryRecord(const std::string& strNickName, const std::vector<int>& vLotteryIDs)
{
	if (vLotteryIDs.size() == 0)
	{
		return;
	}

	RechargeLotteryRecord stOneRecord;
	stOneRecord.strNickName = strNickName;
	for (unsigned i = 0; i < vLotteryIDs.size(); ++i)
	{
		stOneRecord.iLotteryID = vLotteryIDs[i];

		m_vLotteryRecord.insert(m_vLotteryRecord.begin(), stOneRecord);
	}

	if (m_vLotteryRecord.size() > (unsigned)MAX_RECHARGE_LOTTERY_RECORD)
	{
		//ɾ��������
		m_vLotteryRecord.erase(m_vLotteryRecord.begin()+ MAX_RECHARGE_LOTTERY_RECORD, m_vLotteryRecord.end());
	}

	//���浽�ļ�
	SaveRechargeLotterRecord();

	return;
}

//����
void CWorldLimitLotteryManager::Reset()
{
	for (int i = LIMIT_LOTTERY_INVALID + 1; i < LIMIT_LOTTERY_MAX; ++i)
	{
		m_avLotteryInfo[i].clear();
	}

	m_vLotteryRecord.clear();
}
