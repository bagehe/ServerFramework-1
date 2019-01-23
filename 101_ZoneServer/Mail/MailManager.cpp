
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Random.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Reward/RewardUtility.h"
#include "RepThings/RepThingsUtility.hpp"
#include "MailUtility.h"

#include "MailManager.h"

using namespace ServerLib;

//������ϵͳ�ʼ�ΨһID
unsigned CMailManager::m_uServerSystemMailUniqID = 0;

CMailManager::CMailManager()
{
	m_uiUin = 0;

	m_uRoleSystemUniqID = 0;
	m_uPersonalUniqID = 0;

	m_vMails.clear();
}

CMailManager::~CMailManager()
{
	m_uiUin = 0;

	m_uRoleSystemUniqID = 0;
	m_uPersonalUniqID = 0;

	m_vMails.clear();
}

//��ʼ��
int CMailManager::Initialize()
{
	return 0;
}

void CMailManager::SetOwner(unsigned int uin)
{
	m_uiUin = uin;
}

CGameRoleObj* CMailManager::GetOwner()
{
	return CUnitUtility::GetRoleByUin(m_uiUin);
}

//�����ʼ�
void CMailManager::AddNewMail(const OneMailInfo& stMailInfo, bool bSendNotify)
{
	std::vector<unsigned> vDelUniqIDs;
	if (m_vMails.size() >= (unsigned)MAX_ROLE_MAIL_NUM)
	{
		//������������ɾ���ϵ�
		for (unsigned i = 0; i < (m_vMails.size() - MAX_ROLE_MAIL_NUM + 1); ++i)
		{
			vDelUniqIDs.push_back(m_vMails[i].uUniqID);
		}

		m_vMails.erase(m_vMails.begin(), m_vMails.begin()+(m_vMails.size()-MAX_ROLE_MAIL_NUM+1));
	}

	//�������ʼ�
	MailData stOneMail;
	stOneMail.uUniqID = ++m_uPersonalUniqID;
	stOneMail.iMailID = stMailInfo.imailid();
	stOneMail.iSendTime = stMailInfo.isendtime();
	stOneMail.iMailStat = MAIL_STAT_NOTREAD;
	stOneMail.strTitle = stMailInfo.strtitle();

	for (int i = 0; i < stMailInfo.strparams_size(); ++i)
	{
		stOneMail.vParams.push_back(stMailInfo.strparams(i));
	}

	RewardConfig stOneReward;
	for (int i = 0; i < stMailInfo.stappendixs_size(); ++i)
	{
		stOneReward.iType = stMailInfo.stappendixs(i).itype();
		stOneReward.iRewardID = stMailInfo.stappendixs(i).iid();
		stOneReward.iRewardNum = stMailInfo.stappendixs(i).inum();

		stOneMail.vAppendixes.push_back(stOneReward);
	}

	m_vMails.push_back(stOneMail);

	//��ӡ��Ӫ��־ ����յ��ʼ�
	CGameRoleObj *stRoleObj = GetOwner();

	//�ж��Ƿ��и���
	if (stOneMail.vAppendixes.size() == 0)
	{
		//û�и���
		CZoneOssLog::TraceMail(stRoleObj->GetUin(), stRoleObj->GetChannel(), stRoleObj->GetNickName(), stOneMail.uUniqID, stOneMail.strTitle.c_str(), 0, 0, 0, stOneMail.iMailID, stOneMail.iSendTime);
	}
	else
	{
		//�и���
		for (unsigned i = 0; i < stOneMail.vAppendixes.size(); ++i)
		{
			CZoneOssLog::TraceMail(stRoleObj->GetUin(), stRoleObj->GetChannel(), stRoleObj->GetNickName(), stOneMail.uUniqID, stOneMail.strTitle.c_str(),
				stOneMail.vAppendixes[i].iType, stOneMail.vAppendixes[i].iRewardID, stOneMail.vAppendixes[i].iRewardNum, stOneMail.iMailID, stOneMail.iSendTime);
		}
	}

	if (bSendNotify)
	{
		//������Ϣ
		SendAddNewMailNotify(stOneMail, vDelUniqIDs);
	}

	return;
}

//�����ʼ�
int CMailManager::OperateMail(int iOperaType, unsigned uUniqID)
{
	if (iOperaType <= MAIL_OPERA_INVALID || iOperaType >= MAIL_OPERA_MAX)
	{
		LOGERROR("Failed to opera mail, invalid opera type %d, uin %u\n", iOperaType, m_uiUin);
		return T_ZONE_PARA_ERROR;
	}

	int iMailIndex = -1;
	if (uUniqID != 0)
	{
		//��ȡ�ʼ���Ϣ
		iMailIndex = GetMailIndexByUniqID(uUniqID);
		if (iMailIndex < 0)
		{
			LOGERROR("Failed to opera mail, invalid mail uniqid %u, uin %u\n", uUniqID, m_uiUin);
			return T_ZONE_PARA_ERROR;
		}
	}

	int iRet = T_SERVER_SUCCESS;

	//�����ʼ�
	switch (iOperaType)
	{
	case MAIL_OPERA_READ:
	{
		//�Ķ��ʼ�
		m_vMails[iMailIndex].iMailStat = MAIL_STAT_READ;
	}
	break;

	case MAIL_OPERA_RECIEVE:
	{
		//��ȡ����
		if (m_vMails[iMailIndex].iMailStat == MAIL_STAT_GETAPPENDIX || m_vMails[iMailIndex].vAppendixes.size() == 0)
		{
			LOGERROR("Failed to get mail appendix, no items, uin %u, mail uniq id %u\n", m_uiUin, m_vMails[iMailIndex].uUniqID);
			return T_ZONE_PARA_ERROR;
		}

		iRet = GetMailAppendix(m_vMails[iMailIndex]);
		if (iRet)
		{
			LOGERROR("Failed to get mail appendix, mail id %d, ret %d, uin %u\n", m_vMails[iMailIndex].iMailID, iRet, m_uiUin);
			return iRet;
		}

		//���Ϊ����ȡ
		m_vMails[iMailIndex].iMailStat = MAIL_STAT_GETAPPENDIX;
	}
	break;

	case MAIL_OPERA_DELETE:
	{
		//ɾ���ʼ�
		m_vMails.erase(m_vMails.begin()+iMailIndex);
	}
	break;

	case MAIL_OPERA_ONERECIEVE:
	{
		//һ����ȡ����
		for (unsigned i = 0; i < m_vMails.size(); ++i)
		{
			if (m_vMails[i].iMailStat == MAIL_STAT_GETAPPENDIX || m_vMails[i].vAppendixes.size() == 0)
			{
				//û�и���
				continue;
			}

			//��ȡ����
			iRet = GetMailAppendix(m_vMails[i]);
			if (iRet)
			{
				LOGERROR("Failed to get mail appendix, mail id %d, ret %d, uin %u\n", m_vMails[i].iMailID, iRet, m_uiUin);
				return iRet;
			}

			//���Ϊ����ȡ
			m_vMails[i].iMailStat = MAIL_STAT_GETAPPENDIX;
		}
	}
	break;

	case MAIL_OPERA_ONEDELETE:
	{
		//һ��ɾ������ɾ����������
		std::vector<MailData> vMails;
		for (unsigned i = 0; i < m_vMails.size(); ++i)
		{
			if (m_vMails[i].iMailID == MAIL_SEND_CARDNO)
			{
				//�����ʼ�����һ��ɾ��
				continue;
			}

			if (m_vMails[i].iMailStat != MAIL_STAT_GETAPPENDIX && m_vMails[i].vAppendixes.size() != 0)
			{
				//���и���δ��ȡ
				vMails.push_back(m_vMails[i]);
			}
		}

		m_vMails = vMails;
	}
	break;

	default:
		break;
	}

	return T_SERVER_SUCCESS;
}

//������ϵͳ�ʼ�ΨһID
unsigned CMailManager::GetServerMailUniqID()
{
	return m_uServerSystemMailUniqID;
}

void CMailManager::SetServerMailUniqID(unsigned uUniqID)
{
	m_uServerSystemMailUniqID = uUniqID;
}

//���ϵͳ�ʼ�ΨһID
unsigned CMailManager::GetRoleSystemUniqID()
{
	return m_uRoleSystemUniqID;
}

void CMailManager::SetRoleSystemMailUniqID(unsigned uUniqID)
{
	m_uRoleSystemUniqID = uUniqID;
}

//��ʱ��
void CMailManager::OnTick(int iTimeNow)
{
	if (m_uServerSystemMailUniqID == 0 || m_uRoleSystemUniqID != m_uServerSystemMailUniqID)
	{
		//��ȡϵͳ�ʼ�
		CMailUtility::GetSystemMail(*GetOwner(), MAIL_REASON_NORMAL, m_uRoleSystemUniqID);
	}

	return;
}

//�����ʼ���DB
void CMailManager::UpdateMailToDB(MAILDBINFO& stMailDBInfo)
{
	//ϵͳ�ʼ�ΨһID
	stMailDBInfo.set_usystemmaxid(m_uRoleSystemUniqID);

	//�����ʼ�ΨһID
	stMailDBInfo.set_upersonalmaxid(m_uPersonalUniqID);

	//�ʼ���ϸ��Ϣ
	for (unsigned i = 0; i < m_vMails.size(); ++i)
	{
		OneMailInfo* pstMailInfo = stMailDBInfo.add_stmails();
		pstMailInfo->set_uuniqid(m_vMails[i].uUniqID);
		pstMailInfo->set_imailid(m_vMails[i].iMailID);
		pstMailInfo->set_isendtime(m_vMails[i].iSendTime);
		pstMailInfo->set_imailstat(m_vMails[i].iMailStat);
		pstMailInfo->set_strtitle(m_vMails[i].strTitle);

		for (unsigned j = 0; j < m_vMails[i].vParams.size(); ++j)
		{
			pstMailInfo->add_strparams(m_vMails[i].vParams[j]);
		}

		for (unsigned j = 0; j < m_vMails[i].vAppendixes.size(); ++j)
		{
			MailAppendix* pstOneAppendix = pstMailInfo->add_stappendixs();
			pstOneAppendix->set_itype(m_vMails[i].vAppendixes[j].iType);
			pstOneAppendix->set_iid(m_vMails[i].vAppendixes[j].iRewardID);
			pstOneAppendix->set_inum(m_vMails[i].vAppendixes[j].iRewardNum);
		}
	}

	return;
}

//��DB��ʼ���ʼ�
void CMailManager::InitMailFromDB(const MAILDBINFO& stMailDBInfo)
{
	//ϵͳ�ʼ�ΨһID
	m_uRoleSystemUniqID = stMailDBInfo.usystemmaxid();

	//�����ʼ�ΨһID
	m_uPersonalUniqID = stMailDBInfo.upersonalmaxid();

	//�ʼ���ϸ��Ϣ
	m_vMails.clear();
	MailData stOneMail;
	for (int i = 0; i < stMailDBInfo.stmails_size(); ++i)
	{
		stOneMail.Reset();

		stOneMail.uUniqID = stMailDBInfo.stmails(i).uuniqid();
		stOneMail.iMailID = stMailDBInfo.stmails(i).imailid();
		stOneMail.iSendTime = stMailDBInfo.stmails(i).isendtime();
		stOneMail.iMailStat = stMailDBInfo.stmails(i).imailstat();
		stOneMail.strTitle = stMailDBInfo.stmails(i).strtitle();

		for (int j = 0; j < stMailDBInfo.stmails(i).strparams_size(); ++j)
		{
			stOneMail.vParams.push_back(stMailDBInfo.stmails(i).strparams(j));
		}

		RewardConfig stOneReward;
		for (int j = 0; j < stMailDBInfo.stmails(i).stappendixs_size(); ++j)
		{
			stOneReward.iType = stMailDBInfo.stmails(i).stappendixs(j).itype();
			stOneReward.iRewardID = stMailDBInfo.stmails(i).stappendixs(j).iid();
			stOneReward.iRewardNum = stMailDBInfo.stmails(i).stappendixs(j).inum();

			stOneMail.vAppendixes.push_back(stOneReward);
		}

		m_vMails.push_back(stOneMail);
	}

	return;
}

//��ȡ�ʼ�λ�ã������ڷ���-1
int CMailManager::GetMailIndexByUniqID(unsigned uUniqID)
{
	for (unsigned i = 0; i < m_vMails.size(); ++i)
	{
		if (m_vMails[i].uUniqID == uUniqID)
		{
			return i;
		}
	}

	return -1;
}

//�������ʼ���Ϣ
void CMailManager::SendAddNewMailNotify(const MailData& stOneMail, const std::vector<unsigned>& vDelUniqIDs)
{
	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_NEWMAILINFO_NOTIFY);
	Zone_NewMailInfo_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_newmailinfo_notify();

	for (unsigned i = 0; i < vDelUniqIDs.size(); ++i)
	{
		pstNotify->add_udeluniqids(vDelUniqIDs[i]);
	}

	OneMailInfo* pstMailInfo = pstNotify->mutable_stmailinfo();
	pstMailInfo->set_uuniqid(stOneMail.uUniqID);
	pstMailInfo->set_imailid(stOneMail.iMailID);
	pstMailInfo->set_isendtime(stOneMail.iSendTime);
	pstMailInfo->set_imailstat(stOneMail.iMailStat);
	pstMailInfo->set_strtitle(stOneMail.strTitle);

	for (unsigned i = 0; i < stOneMail.vParams.size(); ++i)
	{
		pstMailInfo->add_strparams(stOneMail.vParams[i]);
	}

	for (unsigned i = 0; i < stOneMail.vAppendixes.size(); ++i)
	{
		MailAppendix* pstOneAppendix = pstMailInfo->add_stappendixs();
		pstOneAppendix->set_itype(stOneMail.vAppendixes[i].iType);
		pstOneAppendix->set_iid(stOneMail.vAppendixes[i].iRewardID);
		pstOneAppendix->set_inum(stOneMail.vAppendixes[i].iRewardNum);
	}

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());
	
	return;
}

//��ȡ�ʼ�����
int CMailManager::GetMailAppendix(MailData& stOneMail)
{
	//ƴװ����
	int iAppendixNum = stOneMail.vAppendixes.size();
	RewardConfig astRewards[MAX_APPENDIX_NUM];
	for (int i = 0; i < iAppendixNum; ++i)
	{
		if (i >= MAX_APPENDIX_NUM)
		{
			break;
		}

		astRewards[i].iType = stOneMail.vAppendixes[i].iType;
		astRewards[i].iRewardID = stOneMail.vAppendixes[i].iRewardID;
		astRewards[i].iRewardNum = stOneMail.vAppendixes[i].iRewardNum;
	}
	
	//��ȡ��������
	int iRet = T_SERVER_SUCCESS;
	if (stOneMail.iMailID == MAIL_SEND_MONTHCARD)
	{
		//�¿������������
		iRet = CRepThingsUtility::OpenBoxGift(*GetOwner(), astRewards[0].iRewardID);
		if (iRet)
		{
			LOGERROR("Failed to get month card gift, ret %d, uin %u\n", iRet, m_uiUin);
			return iRet;
		}
	}
	else
	{
		//�����ʼ�ֱ����ȡ
		int iItemChannel = (stOneMail.iMailID == MAIL_SEND_ITEMGIFT) ? ITEM_CHANNEL_ROLEMAIL : ITEM_CHANNEL_SYSMAIL;	//�����Ǹ����ʼ�
		iRet = CRewardUtility::GetReward(*GetOwner(), 1, astRewards, iAppendixNum, iItemChannel);
		if (iRet)
		{
			LOGERROR("Failed to get mail appendix, ret %d, uin %u, mail id %d\n", iRet, m_uiUin, stOneMail.iMailID);
			return iRet;
		}

		//��ӡ��Ӫ��־  ��Ҵ��ʼ���ȡ����
		for (int i = 0; i < iAppendixNum; ++i)
		{
			//��ȡ��ҵ�������
			int iNewNum = GetOwner()->GetRepThingsManager().GetRepItemNum(astRewards[i].iRewardID);

			CZoneOssLog::TraceGetRewardFromMail(GetOwner()->GetUin(), GetOwner()->GetChannel(), GetOwner()->GetNickName(),
				astRewards[i].iRewardID, astRewards[i].iRewardNum, iNewNum - astRewards[i].iRewardNum, iNewNum);
		}
	}

	return T_SERVER_SUCCESS;
}
