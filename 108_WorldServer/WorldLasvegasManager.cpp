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

#include "WorldLasvegasManager.h"

static const char* WORLD_LASVEGAS_RECORD_FILE = "../conf/LasvegasRecord.dat";

static GameProtocolMsg stMsg;

CWorldLasvegasManager* CWorldLasvegasManager::Instance()
{
	static CWorldLasvegasManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CWorldLasvegasManager;
	}

	return pInstance;
}

CWorldLasvegasManager::CWorldLasvegasManager()
{
	Reset();
}

CWorldLasvegasManager::~CWorldLasvegasManager()
{
	Reset();
}

//��ʼ��
void CWorldLasvegasManager::Init()
{
	//��������
	Reset();

	//������ʷ��¼
	LoadLasvegasInfo();
}

//����������ע��Ϣ
void CWorldLasvegasManager::UpdateBetNumber(int iNumber, int iBetCoins)
{
	if (m_iStepType != LASVEGAS_STEP_BET)
	{
		//������ע�׶�
		return;
	}

	//������ע���
	for (unsigned i = 0; i < m_vBetDatas.size(); ++i)
	{
		if (m_vBetDatas[i].iNumber == iNumber)
		{
			m_vBetDatas[i].iBetCoins += iBetCoins;
			return;
		}
	}

	//û�ҵ�������һ��
	LasvegasBetData stOneData;
	stOneData.iNumber = iNumber;
	stOneData.iBetCoins = iBetCoins;
	m_vBetDatas.push_back(stOneData);

	m_bSendUpdate = true;

	return;
}

//�����н���Ϣ
void CWorldLasvegasManager::UpdatePrizeInfo(const World_UpdatePrizeInfo_Request& stReq)
{
	//���²��뵽��ǰ
	LotteryPrizeData stOneData;
	
	for (int i = 0; i < stReq.stprizes_size(); ++i)
	{
		stOneData.strNickName = stReq.stprizes(i).strname();
		stOneData.iNumber = stReq.stprizes(i).inumber();
		stOneData.iRewardCoins = stReq.stprizes(i).irewardcoins();

		m_vPrizeDatas.insert(m_vPrizeDatas.begin(), stOneData);
	}

	//�Ƿ񳬹�����
	if (m_vPrizeDatas.size() > (unsigned)MAX_LASVEGAS_RECORD_NUM)
	{
		//ɾ��β����
		m_vPrizeDatas.erase(m_vPrizeDatas.begin()+MAX_LASVEGAS_RECORD_NUM, m_vPrizeDatas.end());
	}

	//���浽�ļ�
	SaveLasvegasInfo();

	m_bSendUpdate = true;

	return;
}

//��ʱ��
void CWorldLasvegasManager::OnTick(int iTimeNow)
{
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//����ת���߼�
	switch (m_iStepType)
	{
	case LASVEGAS_STEP_INVALID:
	{
		//ֱ�ӽ�����ע�׶�
		m_iStepType = LASVEGAS_STEP_BET;
		m_iStepEndTime = iTimeNow+stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LASVEGAS_BETTIME);

		//���͸��ͻ���
		SendUpdateLasvegasNotify(iTimeNow);
	}
	break;

	case LASVEGAS_STEP_BET:
	{
		//��ע�׶�
		if (m_iStepEndTime <= iTimeNow)
		{
			//���뿪���׶�
			m_iStepType = LASVEGAS_STEP_LOTTERY;
			m_iStepEndTime = iTimeNow + stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LASVEGAS_LOTTERYTIME);

			//�����ע��Ϣ
			m_vBetDatas.clear();

			//����
			const LasvegasConfig* pstConfig = stBaseCfgMgr.GetLasvegasConfig();
			if (pstConfig)
			{
				m_vLotteryIDs.push_back(pstConfig->iID);
				if (m_vLotteryIDs.size() > (unsigned)MAX_LASVEGAS_LOTTERY_NUM)
				{
					//ɾ����һ��
					m_vLotteryIDs.erase(m_vLotteryIDs.begin());
				}

				//���浽�ļ�
				SaveLasvegasInfo();
			}

			//���͸��ͻ�����Ϣ
			SendUpdateLasvegasNotify(iTimeNow);
		}
	}
	break;

	case LASVEGAS_STEP_LOTTERY:
	{
		//�����׶�
		if (m_iStepEndTime <= iTimeNow)
		{
			//�����׶ν�����������һ����ע
			m_iStepType = LASVEGAS_STEP_BET;
			m_iStepEndTime = iTimeNow + stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_LASVEGAS_BETTIME);

			//���͸��ͻ���
			SendUpdateLasvegasNotify(iTimeNow);
		}
	}
	break;

	default:
		break;
	}

	//�Ƿ�����ת����Ϣ,2s����һ��
	if (m_bSendUpdate && (m_iLastUpdateTime + 2) <= iTimeNow)
	{
		SendUpdateLasvegasNotify(iTimeNow);
	}

	return;
}

//����Lasvegas��Ϣ
void CWorldLasvegasManager::LoadLasvegasInfo()
{
	//���ļ��ж�ȡת����Ϣ
	std::ifstream is;
	is.open(WORLD_LASVEGAS_RECORD_FILE, std::ios::binary);
	if (is.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open lasvegas record file %s\n", WORLD_LASVEGAS_RECORD_FILE);
		return;
	}

	Json::Reader jReader;
	Json::Value jValue;
	if (!jReader.parse(is, jValue))
	{
		//����json�ļ�ʧ��
		is.close();
		LOGERROR("Failed to parse lasvegas record file %s\n", WORLD_LASVEGAS_RECORD_FILE);
		return;
	}

	m_vLotteryIDs.clear();
	m_vPrizeDatas.clear();

	//������¼
	for (unsigned i = 0; i < jValue["lottery"].size(); ++i)
	{
		m_vLotteryIDs.push_back(jValue["lottery"][i].asInt());
	}

	//�н���¼
	LotteryPrizeData stData;
	for (unsigned i = 0; i < jValue["prize"].size(); ++i)
	{
		stData.strNickName = jValue["prize"][i]["name"].asString();
		stData.iNumber = jValue["prize"][i]["num"].asInt();
		stData.iRewardCoins = jValue["prize"][i]["coins"].asInt();

		m_vPrizeDatas.push_back(stData);
	}

	is.close();

	return;
}

//����Lasvegas��Ϣ
void CWorldLasvegasManager::SaveLasvegasInfo()
{
	//����ת����Ϣ���ļ�
	std::ofstream os;
	os.open(WORLD_LASVEGAS_RECORD_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open lasvegas record file %s\n", WORLD_LASVEGAS_RECORD_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;
	Json::Value jOneInfo;

	//������¼
	for (unsigned i = 0; i < m_vLotteryIDs.size(); ++i)
	{
		jOneInfo.append(m_vLotteryIDs[i]);
	}
	jValue["lottery"] = jOneInfo;

	//�н���¼
	Json::Value jOneRewardInfo;
	for (unsigned i = 0; i < m_vPrizeDatas.size(); ++i)
	{
		jOneRewardInfo.clear();
		jOneRewardInfo["name"] = m_vPrizeDatas[i].strNickName;
		jOneRewardInfo["num"] = m_vPrizeDatas[i].iNumber;
		jOneRewardInfo["coins"] = m_vPrizeDatas[i].iRewardCoins;

		jValue["prize"].append(jOneRewardInfo);
	}

	os << jValue.toStyledString();
	os.close();

	return;
}

//���ʹ�ת����Ϣ
void CWorldLasvegasManager::SendUpdateLasvegasNotify(int iTimeNow)
{
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_WORLD_UPDATELASVEGAS_NOTIFY, 0);

	World_UpdateLasvegas_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stworld_updatelasvegas_notify();
	pstNotify->mutable_stinfo()->set_isteptype(m_iStepType);
	pstNotify->mutable_stinfo()->set_istependtime(m_iStepEndTime);
	
	//������Ϣ
	for (unsigned i = 0; i < m_vLotteryIDs.size(); ++i)
	{
		pstNotify->mutable_stinfo()->add_ilotteryids(m_vLotteryIDs[i]);
	}

	//�н���Ϣ
	for (unsigned i = 0; i < m_vPrizeDatas.size(); ++i)
	{
		PrizeInfo* pstOneInfo = pstNotify->mutable_stinfo()->add_stprizes();
		pstOneInfo->set_strname(m_vPrizeDatas[i].strNickName);
		pstOneInfo->set_inumber(m_vPrizeDatas[i].iNumber);
		pstOneInfo->set_irewardcoins(m_vPrizeDatas[i].iRewardCoins);
	}

	//��ע��Ϣ
	for (unsigned i = 0; i < m_vBetDatas.size(); ++i)
	{
		NumberBetInfo* pstOneInfo = pstNotify->mutable_stinfo()->add_stbets();
		pstOneInfo->set_inumber(m_vBetDatas[i].iNumber);
		pstOneInfo->set_lbetcoins(m_vBetDatas[i].iBetCoins);
	}

	CWorldMsgHelper::SendWorldMsgToAllZone(stMsg);

	m_iLastUpdateTime = iTimeNow;
	m_bSendUpdate = false;

	return;
}

//����
void CWorldLasvegasManager::Reset()
{
	m_iStepType = 0;
	m_iStepEndTime = 0;
	
	m_bSendUpdate = false;
	m_iLastUpdateTime = 0;

	m_vLotteryIDs.clear();
	m_vPrizeDatas.clear();
	m_vBetDatas.clear();
}
