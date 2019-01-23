
#include <algorithm>
#include <fstream>

#include "json/json.h"
#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"
#include "WorldMsgHelper.hpp"
#include "ConfigManager.hpp"
#include "WorldRoleStatus.hpp"
#include "WorldObjectAllocatorW.hpp"

#include "WorldMailManager.hpp"

using namespace ServerLib;

const static char* WORLD_SYSTEM_MAIL_FILE = "../conf/SystemMail.dat";

CWorldMailManager* CWorldMailManager::Instance()
{
	static CWorldMailManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CWorldMailManager;
	}

	return pInstance;
}

CWorldMailManager::CWorldMailManager()
{
	Reset();
}

CWorldMailManager::~CWorldMailManager()
{

}

//��ʼ��Mail
void CWorldMailManager::Init()
{
	Reset();

	//����ϵͳ�ʼ�
	LoadMailInfo();
}

//����GM�ʼ�
int CWorldMailManager::SendGMMail(const GameMaster_Request& stReq)
{
	//��������2������Ϊ3*X+2�ĸ�ʽ
	if (stReq.ioperatype() != GM_OPERA_SENDMAIL || stReq.strparams_size() < 2 || (stReq.strparams_size()-2)%3!=0)
	{
		return T_ZONE_PARA_ERROR;
	}

	World_SendMail_Request stSendMailReq;
	stSendMailReq.set_uitouin(stReq.utouin());
	OneMailInfo* pstMailInfo = stSendMailReq.mutable_stmailinfo();
	pstMailInfo->set_isendtime(CTimeUtility::GetNowTime());

	//����1 Title ����
	pstMailInfo->set_strtitle(stReq.strparams(0));

	//����2 Content ����
	pstMailInfo->add_strparams(stReq.strparams(1));

	//�������������Ϊ����
	for (int i = 2; i < stReq.strparams_size(); i+=3)
	{
		if ((i + 3) > stReq.strparams_size())
		{
			//������������
			break;
		}

		if (pstMailInfo->stappendixs_size() >= MAX_APPENDIX_NUM)
		{
			//������������
			break;
		}

		MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
		pstAppendix->set_itype(atoi(stReq.strparams(i).c_str()));
		pstAppendix->set_iid(atoi(stReq.strparams(i+1).c_str()));
		pstAppendix->set_inum(atoi(stReq.strparams(i+2).c_str()));
	}

	if (pstMailInfo->stappendixs_size() == 0)
	{
		//GM��ͨ�ʼ�
		pstMailInfo->set_imailid(MAIL_SEND_GMNORMAL);
	}
	else
	{
		//GM�����ʼ�
		pstMailInfo->set_imailid(MAIL_SEND_GMAPPENDIX);
	}

	if (stReq.utouin() == 0)
	{
		//����ϵͳ�ʼ�
		SendSystemMail(stReq, stSendMailReq.stmailinfo());
	}
	else
	{
		//���͸����ʼ�
		int iRet = CWorldMailManager::SendPersonalMail(stSendMailReq);
		if (iRet)
		{
			LOGERROR("Failes to GM send mail, ret %d, from %u, to %u, mail id %d\n", iRet, stReq.ufromuin(), stReq.utouin(), pstMailInfo->imailid());
			return iRet;
		}
	}

	LOGDEBUG("Success to GM send mail, from %u, to %u, mail id %d\n", stReq.ufromuin(), stReq.utouin(), pstMailInfo->imailid());

	return T_SERVER_SUCCESS;
}

//���͸����ʼ�
int CWorldMailManager::SendPersonalMail(const World_SendMail_Request& stReq)
{
	static GameProtocolMsg stMailMsg;
	CWorldMsgHelper::GenerateMsgHead(stMailMsg, 0, MSGID_WORLD_SENDMAIL_REQUEST, stReq.uitouin());
	stMailMsg.mutable_stbody()->mutable_m_stworld_sendmail_request()->CopyFrom(stReq);

	CWorldRoleStatusWObj* pstRoleStatObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByUin(stReq.uitouin());
	if (pstRoleStatObj)
	{
		//Ŀ��������ߣ����������ʼ�
		return CWorldMsgHelper::SendWorldMsgToWGS(stMailMsg, pstRoleStatObj->GetZoneID());
	}
	else
	{
		//Ŀ����Ҳ����ߣ����������ʼ�
		return CWorldMsgHelper::SendWorldMsgToRoleDB(stMailMsg);
	}

	return T_SERVER_SUCCESS;
}

//��ȡϵͳ�ʼ�
void CWorldMailManager::GetSystemMail(unsigned uin, int iReason, unsigned uUniqID, int iVIPLevel, const std::string& strChannel, World_GetSystemMail_Response& stResp)
{
	stResp.set_usystemuniqid(m_uSystemUniqID);

	if (iReason==MAIL_REASON_NORMAL && uUniqID >= m_uSystemUniqID)
	{
		//��ͨ��ȡ����û�����ʼ�
		return;
	}

	int iTimeNow = CTimeUtility::GetNowTime();
	for (unsigned i = 0; i < m_vSystemMail.size(); ++i)
	{
		SystemMailData& stOne = m_vSystemMail[i];

		//�Ƿ����
		if (m_vSystemMail[i].iEndTime <= iTimeNow)
		{
			//�ʼ��ѹ���
			continue;
		}

		switch (iReason)
		{
		case MAIL_REASON_NORMAL:
		{
			//��ͨ��ȡ

			//VIP�Ƿ�����
			if ((stOne.iVIPMin != 0 || stOne.iVIPMax != 0) &&
				(stOne.iVIPMin>iVIPLevel || stOne.iVIPMax<iVIPLevel))
			{
				continue;
			}

			//�����Ƿ�����
			if (stOne.vChannelIDs.size() != 0 && std::find(stOne.vChannelIDs.begin(), stOne.vChannelIDs.end(), strChannel) == stOne.vChannelIDs.end())
			{
				continue;
			}

			//�Ƿ��Ѿ���ȡ
			if (stOne.stMail.uUniqID <= uUniqID)
			{
				continue;
			}
		}
		break;

		case MAIL_REASON_VIPLV:
		{
			//VIP�ȼ��ı���ȡ
			//ֻ���VIP�Ƿ�����
			if (stOne.iVIPMin == 0 && stOne.iVIPMax == 0)
			{
				//����ȡû��VIP�ȼ����Ƶ�
				continue;
			}

			if (stOne.iVIPMin != iVIPLevel)
			{
				//VIP������
				continue;
			}
		}
		break;

		default:
			break;
		}

		OneMailInfo* pstMailInfo = stResp.add_stmails();
		pstMailInfo->set_imailid(stOne.stMail.iMailID);
		pstMailInfo->set_uuniqid(stOne.stMail.uUniqID);
		pstMailInfo->set_isendtime(stOne.stMail.iSendTime);
		pstMailInfo->set_strtitle(stOne.stMail.strTitle);

		for (unsigned j = 0; j < stOne.stMail.vParams.size(); ++j)
		{
			pstMailInfo->add_strparams(stOne.stMail.vParams[j]);
		}

		for (unsigned j = 0; j < stOne.stMail.vAppendixes.size(); ++j)
		{
			MailAppendix* pstAppendix = pstMailInfo->add_stappendixs();
			pstAppendix->set_itype(stOne.stMail.vAppendixes[j].iType);
			pstAppendix->set_iid(stOne.stMail.vAppendixes[j].iRewardID);
			pstAppendix->set_inum(stOne.stMail.vAppendixes[j].iRewardNum);
		}
	}

	return;
}

//����ϵͳ�ʼ�
void CWorldMailManager::SendSystemMail(const GameMaster_Request& stReq, const OneMailInfo& stMailInfo)
{
	SystemMailData stSystemMail;

	//VIP�ȼ�����
	stSystemMail.iVIPMin = stReq.ivipmin();
	stSystemMail.iVIPMax = stReq.ivipmax();
	
	//�ʼ�����ʱ��
	stSystemMail.iEndTime = stReq.imailendtime();

	//��������
	for (int i = 0; i < stReq.strchannelids_size(); ++i)
	{
		stSystemMail.vChannelIDs.push_back(stReq.strchannelids(i));
	}

	stSystemMail.stMail.uUniqID = ++m_uSystemUniqID;
	stSystemMail.stMail.iMailID = stMailInfo.imailid();
	stSystemMail.stMail.iSendTime = stMailInfo.isendtime();
	stSystemMail.stMail.iMailStat = MAIL_STAT_NOTREAD;
	stSystemMail.stMail.strTitle = stMailInfo.strtitle();

	for (int i = 0; i < stMailInfo.strparams_size(); ++i)
	{
		stSystemMail.stMail.vParams.push_back(stMailInfo.strparams(i));
	}

	RewardConfig stOneAppendix;
	for (int i = 0; i < stMailInfo.stappendixs_size(); ++i)
	{
		stOneAppendix.iType = stMailInfo.stappendixs(i).itype();
		stOneAppendix.iRewardID = stMailInfo.stappendixs(i).iid();
		stOneAppendix.iRewardNum = stMailInfo.stappendixs(i).inum();

		stSystemMail.stMail.vAppendixes.push_back(stOneAppendix);
	}

	m_vSystemMail.push_back(stSystemMail);

	SendSystemMailIDNotify();

	//���浽�ļ�
	SaveMailInfo();

	return;
}

//����
void CWorldMailManager::SendSystemMailIDNotify()
{
	static GameProtocolMsg stMsg;
	CWorldMsgHelper::GenerateMsgHead(stMsg, 0, MSGID_WORLD_SYSTEMMAILID_NOTIFY, 0);

	World_SystemMailID_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stworld_systemmailid_notify();
	pstNotify->set_usystemuniqid(m_uSystemUniqID);

	CWorldMsgHelper::SendWorldMsgToAllZone(stMsg);

	return;
}

//����Mail��Ϣ
void CWorldMailManager::LoadMailInfo()
{
	//���ļ��ж�ȡRank��Ϣ
	std::ifstream is;
	is.open(WORLD_SYSTEM_MAIL_FILE, std::ios::binary);
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
		LOGERROR("Failed to parse system mail info file %s\n", WORLD_SYSTEM_MAIL_FILE);
		return;
	}

	//ΨһID
	m_uSystemUniqID = jValue["uniqid"].asUInt();

	m_vSystemMail.clear();
	
	SystemMailData stOneData;
	RewardConfig stAppendix;
	for (unsigned i = 0; i < jValue["mail"].size(); ++i)
	{
		stOneData.Reset();

		//VIP����
		stOneData.iVIPMin = jValue["mail"][i]["vipmin"].asInt();
		stOneData.iVIPMin = jValue["mail"][i]["vipmax"].asInt();

		//�ʼ�����ʱ��
		stOneData.iEndTime = jValue["mail"][i]["endtime"].asInt();

		//��������
		for (unsigned j = 0; j < jValue["mail"][i]["channel"].size(); ++j)
		{
			stOneData.vChannelIDs.push_back(jValue["mail"][i]["channel"][j].asString());
		}

		//�ʼ�����
		stOneData.stMail.iMailID = jValue["mail"][i]["mailinfo"]["id"].asInt();
		stOneData.stMail.uUniqID = jValue["mail"][i]["mailinfo"]["uniqid"].asUInt();
		stOneData.stMail.iSendTime = jValue["mail"][i]["mailinfo"]["time"].asUInt();
		stOneData.stMail.strTitle = jValue["mail"][i]["mailinfo"]["title"].asString();

		for (unsigned j = 0; j < jValue["mail"][i]["mailinfo"]["param"].size(); ++j)
		{
			stOneData.stMail.vParams.push_back(jValue["mail"][i]["mailinfo"]["param"][j].asString());
		}

		for (unsigned j = 0; j < jValue["mail"][i]["mailinfo"]["appendix"].size(); ++j)
		{
			stAppendix.iType = jValue["mail"][i]["mailinfo"]["appendix"][j]["type"].asInt();
			stAppendix.iRewardID = jValue["mail"][i]["mailinfo"]["appendix"][j]["id"].asInt();
			stAppendix.iRewardNum = jValue["mail"][i]["mailinfo"]["appendix"][j]["num"].asInt();

			stOneData.stMail.vAppendixes.push_back(stAppendix);
		}

		m_vSystemMail.push_back(stOneData);
	}

	is.close();

	return;
}

//����Mail��Ϣ
void CWorldMailManager::SaveMailInfo()
{
	//����Rank��Ϣ���ļ�
	std::ofstream os;
	os.open(WORLD_SYSTEM_MAIL_FILE, std::ios::binary);
	if (os.fail())
	{
		//���ļ�ʧ��
		LOGERROR("Failed to open system mail info file %s\n", WORLD_SYSTEM_MAIL_FILE);
		return;
	}

	//������ݳ�json��
	Json::Value jValue;

	//ΨһID
	jValue["uniqid"] = m_uSystemUniqID;

	Json::Value jOneSystemMail;
	std::vector<SystemMailData> vSystemMail;
	int iTimeNow = CTimeUtility::GetNowTime();
	for (unsigned i = 0; i < m_vSystemMail.size(); ++i)
	{
		if (m_vSystemMail[i].iEndTime <= iTimeNow)
		{
			//�޳����ڵ�
			continue;
		}

		vSystemMail.push_back(m_vSystemMail[i]);

		jOneSystemMail.clear();

		//VIP�ȼ�����
		jOneSystemMail["vipmin"] = m_vSystemMail[i].iVIPMin;
		jOneSystemMail["vipmax"] = m_vSystemMail[i].iVIPMax;

		//�ʼ�����ʱ��
		jOneSystemMail["endtime"] = m_vSystemMail[i].iEndTime;

		//��������
		for (unsigned j = 0; j < m_vSystemMail[i].vChannelIDs.size(); ++j)
		{
			jOneSystemMail["channel"].append(m_vSystemMail[i].vChannelIDs[j]);
		}

		//�ʼ�����
		jOneSystemMail["mailinfo"]["id"] = m_vSystemMail[i].stMail.iMailID;
		jOneSystemMail["mailinfo"]["uniqid"] = m_vSystemMail[i].stMail.uUniqID;
		jOneSystemMail["mailinfo"]["time"] = m_vSystemMail[i].stMail.iSendTime;
		jOneSystemMail["mailinfo"]["title"] = m_vSystemMail[i].stMail.strTitle;

		for (unsigned j = 0; j < m_vSystemMail[i].stMail.vParams.size(); ++j)
		{
			jOneSystemMail["mailinfo"]["param"].append(m_vSystemMail[i].stMail.vParams[j]);
		}
		
		for (unsigned j = 0; j < m_vSystemMail[i].stMail.vAppendixes.size(); ++j)
		{
			Json::Value jOneAppendix;

			jOneAppendix["type"] = m_vSystemMail[i].stMail.vAppendixes[j].iType;
			jOneAppendix["id"] = m_vSystemMail[i].stMail.vAppendixes[j].iRewardID;
			jOneAppendix["num"] = m_vSystemMail[i].stMail.vAppendixes[j].iRewardNum;

			jOneSystemMail["mailinfo"]["appendix"].append(jOneAppendix);
		}

		jValue["mail"].append(jOneSystemMail);
	}

	m_vSystemMail = vSystemMail;

	os << jValue.toStyledString();
	os.close();

	return;
}

//����
void CWorldMailManager::Reset()
{
	m_uSystemUniqID = 1;	//Ĭ�ϴ�1��ʼ
	m_vSystemMail.clear();
}
