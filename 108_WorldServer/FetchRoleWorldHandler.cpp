#include <string.h>

#include "ProtoDataUtility.hpp"
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "ErrorNumDef.hpp"

#include "FetchRoleWorldHandler.hpp"

using namespace ServerLib;

CFetchRoleWorldHandler::~CFetchRoleWorldHandler()
{
}

int CFetchRoleWorldHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->sthead().uimsgid())
	{
	case MSGID_WORLD_FETCHROLE_REQUEST:
	{
		OnRequestFetchRoleWorld();
	}
	break;

	case MSGID_WORLD_FETCHROLE_RESPONSE:
	{
		OnResponseFetchRoleWorld();
	}
	break;

	case MSGID_WORLD_CREATEROLE_RESPONSE:
	{
		//������ɫ���صĴ������
		OnResponseCreateRoleWorld();
	}
	break;

	default:
	{
		break;
	}
	}

	return 0;
}

int CFetchRoleWorldHandler::OnRequestFetchRoleWorld()
{
    const World_FetchRole_Request& rstRequest = m_pMsg->stbody().m_stworld_fetchrole_request();

    unsigned uiUin = rstRequest.stroleid().uin();
	if (uiUin == 0)
	{
		SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
		return -1;
	}
    
    LOGDEBUG("FetchRoleWorld: Uin = %d\n", uiUin);

    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(rstRequest.stroleid());
    // �����ɫ�����Ѿ�����, �����ǵ�¼����, �򷵻�ʧ��
    if (pWorldRoleStatusObj && rstRequest.bislogin())
    {
        LOGERROR("Role Already Login: Uin = %d.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -2;
    }

    // ������ǵ�¼���󣬲���Ҫ��ѯ�Ľ�ɫ�����ڣ��򷵻�ʧ��
    if (!pWorldRoleStatusObj && !rstRequest.bislogin())
    {
        LOGERROR("Role Non-Exists: Uin = %u.\n", uiUin);
        SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
        return -3;
    }
    
    // ����ǵ�¼���߶�ȡ�浵����, ������ɫ���󲢲�ѯ��ɫ��Ϣ
    if (rstRequest.bislogin())
    {
        //��¼������Ҫ���´�������Obj
        pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::CreateByUin(uiUin);
        if (!pWorldRoleStatusObj)
        {
            LOGERROR("CreateRoleStatusObj Failed: Uin = %u.\n", uiUin);
            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            return -4;
        }

        pWorldRoleStatusObj->SetZoneID(rstRequest.ireqid());
        pWorldRoleStatusObj->SetUin(uiUin);

        // �����ݿ�����ȡ����
        int iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(*m_pMsg);
        if (iRet < 0)
        {
            LOGERROR("SendFetchRoleDB Failed: Uin = %d, iRet = %d\n", uiUin, iRet);

            SendFailedFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid());
            WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
            return -6;
        }

        return 0;
    }
    
    // ����ֱ�ӷ��ؽ�ɫ����
    SendFetchRoleResponse(rstRequest.stroleid(), rstRequest.ireqid(), rstRequest.stkicker());

	return 0;
}

//��ѯ���󷵻ص�World�Ĵ������
int CFetchRoleWorldHandler::OnResponseFetchRoleWorld()
{
    const World_FetchRole_Response& stResp = m_pMsg->stbody().m_stworld_fetchrole_response();
    unsigned uiUin = stResp.stroleid().uin();

    //����World����ı����������
    CWorldRoleStatusWObj* pUserStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stResp.stroleid());
    if(!pUserStatusObj)
    {
        LOGERROR("World No Cache Data: Uin = %u\n", uiUin);
        return -1;
    }

	int iZoneID = pUserStatusObj->GetZoneID();
	
	//�����DB��������ʧ��
	if (stResp.iresult())
	{
		LOGERROR("Fail to fetch role from DB, uin %u, ret 0x%0x\n", uiUin, stResp.iresult());

		WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
		SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
		return -2;
	}

	//������ǵ�¼���ҽ�ɫ������
	if (!stResp.bislogin() && !stResp.bexist())
	{
		LOGERROR("Fail to fetch role from DB, uin %u, ret 0x%0x\n", uiUin, stResp.iresult());

		WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
		SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
		return -3;
	}

	if (!stResp.bexist())
	{
		//��ɫ�����ڣ��ߴ�������
		int iRet = SendCreateRoleRequestToDBSvr(stResp.stkicker());
		if (iRet)
		{
			//���ʹ�����ɫ��Ϣʧ��
			LOGERROR("Failed to create role, ret %d, uin %u\n", iRet, stResp.stroleid().uin());

			WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
			SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
			return -4;
		}

		pUserStatusObj->GetRoleInfo().CopyFrom(m_stWorldMsg.stbody().m_stworld_createrole_request().stbirthdata());
		return 0;
	}
	else
	{
		//��ɫ���ڣ�����ȡ�ɹ�����
		int iRet = OnFetchSuccess(stResp.stroleid(), stResp.stuserinfo(), stResp.bislogin(), stResp.stkicker());
		if (iRet)
		{
			LOGERROR("Failed to fetch role, uin %u, ret %d\n", uiUin, iRet);

			WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
			SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
			return -5;
		}
	}

    return 0;
}

//������ɫ���صĴ������
int CFetchRoleWorldHandler::OnResponseCreateRoleWorld()
{
	const World_CreateRole_Response& stResp = m_pMsg->stbody().m_stworld_createrole_response();
	unsigned uiUin = stResp.stroleid().uin();

	//����World����ı����������
	CWorldRoleStatusWObj* pUserStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stResp.stroleid());
	if (!pUserStatusObj)
	{
		LOGERROR("World No Cache Data: Uin = %u\n", uiUin);
		return -1;
	}

	int iZoneID = pUserStatusObj->GetZoneID();
	if (stResp.iresult() != 0)
	{
		//����ʧ��
		LOGERROR("Failed to create role, uin %u, zone id %d\n", uiUin, iZoneID);

		WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
		SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
		return -2;
	}

	//�����ɹ�
	int iRet = OnFetchSuccess(stResp.stroleid(), pUserStatusObj->GetRoleInfo(), true, stResp.stkicker());
	if (iRet)
	{
		LOGERROR("Failed to fetch role, uin %u, ret %d\n", uiUin, iRet);

		WorldTypeK32<CWorldRoleStatusWObj>::DeleteByUin(uiUin);
		SendFailedFetchRoleResponse(stResp.stroleid(), iZoneID);
		return iRet;
	}

	return 0;
}

//���ʹ�����ɫ������
int CFetchRoleWorldHandler::SendCreateRoleRequestToDBSvr(const KickerInfo& stKicker)
{
	//������Ϣͷ
	CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->sthead().uisessionfd(),
		MSGID_WORLD_CREATEROLE_REQUEST, m_pMsg->sthead().uin());

	//���ɴ�����ɫ�ʺŵ���Ϣ��
	World_CreateRole_Request* pstRequest = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_createrole_request();

	int iRet = InitRoleBirthInfo(*pstRequest, stKicker);
	if (iRet)
	{
		LOGERROR("Fail to init role birth info, uin %u, ret 0x%0x\n", m_pMsg->sthead().uin(), iRet);
		return iRet;
	}

	iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(m_stWorldMsg);

	return iRet;
}

//��ʼ�������Ϣ
int CFetchRoleWorldHandler::InitRoleBirthInfo(World_CreateRole_Request& rstRequest, const KickerInfo& stKicker)
{
	rstRequest.set_uin(m_pMsg->sthead().uin());
	rstRequest.set_world(CModuleHelper::GetWorldID());
	rstRequest.set_irealnamestat(stKicker.irealnamestat());
	rstRequest.mutable_stkicker()->CopyFrom(stKicker);

	//��ʼ����һ�������
	unsigned int uTimeNow = time(NULL);

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	BASEDBINFO stBaseInfo;
	stBaseInfo.set_sznickname(stKicker.strnickname());
	stBaseInfo.set_strchannel(stKicker.strchannel());
	stBaseInfo.set_icreatetime(uTimeNow);

	//��ʼ����Դ
	for (int i = RESOURCE_TYPE_INVALID; i<RESOURCE_TYPE_MAX; ++i)
	{
		stBaseInfo.add_iresources(0);
	}
	stBaseInfo.set_iresources(RESOURCE_TYPE_COIN, stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_COIN));
	stBaseInfo.set_luserwinnum(stBaseInfo.iresources(RESOURCE_TYPE_COIN));

	//��ʼ����̨����ʽ
	stBaseInfo.mutable_stweapon()->set_iweaponid(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_WEAPON));
	stBaseInfo.mutable_stweapon()->set_iweaponstyleid(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_WEAPONSTYLE));
	stBaseInfo.mutable_stweapon()->add_iunlockstyleids(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_WEAPONSTYLE));

	//��ʼ��ͷ��
	char szPicID[32] = { 0 };
	SAFE_SPRINTF(szPicID, sizeof(szPicID)-1, "%d", stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_PICID));
	stBaseInfo.set_strpicid(szPicID);

	//��ʼ��VIP�ȼ��ͳ齱
	stBaseInfo.set_iviplevel(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_VIPLEVEL));
	stBaseInfo.set_lvipexp(0);

	//��ȡVIP����
	const VipLevelConfig* pstVIPConfig = stBaseCfgMgr.GetVipConfig(stBaseInfo.iviplevel());
	if (!pstVIPConfig)
	{
		LOGERROR("Failed to get vip config, invalid level %d\n", stBaseInfo.iviplevel());
		return -1;
	}

	int iVIPPriv = 0;
	for (unsigned i = 0; i < pstVIPConfig->vPrivs.size(); ++i)
	{
		iVIPPriv |= pstVIPConfig->vPrivs[i].iPrivType;
	}
	stBaseInfo.set_ivippriv(iVIPPriv);

	stBaseInfo.set_ilotterynum(0);

	//��ʼ������������
	for (int i = 0; i < MAX_ROOM_ALGORITHM_TYPE; ++i)
	{
		stBaseInfo.add_stexplines()->set_iexplinetype(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_EXPTYPE));
	}

	//��ʼ�����ֺ��
	stBaseInfo.set_inownewrednum(stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREDNUM)/stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREDTIMES));
	stBaseInfo.set_iremainnewrednum(stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_NEWREDNUM)-stBaseInfo.inownewrednum());

	//��ʼ����ҵ�������Ϣ
	QUESTDBINFO stQuestInfo;

	//�����ó�ʼ������
	const std::vector<QuestConfig>& stAllQuests = stBaseCfgMgr.GetQuestConfig();
	for (unsigned i = 0; i < stAllQuests.size(); ++i)
	{
		switch (stAllQuests[i].iType)
		{
		case QUEST_TYPE_NEW:
		case QUEST_TYPE_DAILY:
		{
			OneQuest* pstOneQuestInfo = stQuestInfo.add_stquestinfos();
			pstOneQuestInfo->set_iquestid(stAllQuests[i].iID);
			pstOneQuestInfo->set_iquesttype(stAllQuests[i].iType);
			pstOneQuestInfo->set_ineedtype(stAllQuests[i].iNeedType);
			pstOneQuestInfo->set_inum(0);
			pstOneQuestInfo->set_bisfin(false);
		}
		break;

		case QUEST_TYPE_ACHIEVE:
		{
			//ֻ���ӵ�һ�׶�
			if (stAllQuests[i].iQuestIndex == 1)
			{
				//��ʼ�ɾ�����
				OneQuest* pstOneQuestInfo = stQuestInfo.add_stquestinfos();
				pstOneQuestInfo->set_iquestid(stAllQuests[i].iID);
				pstOneQuestInfo->set_iquesttype(stAllQuests[i].iType);
				pstOneQuestInfo->set_ineedtype(stAllQuests[i].iNeedType);
				pstOneQuestInfo->set_inum(0);
				pstOneQuestInfo->set_bisfin(false);
			}
		}
		break;

		case QUEST_TYPE_ADVENTURE:
		{

		}
		break;

		default:
			break;
		}
	}

	int iNextDayNowTime = CTimeUtility::GetNowTime() + 24 * 60 * 60;
	stQuestInfo.set_iadventnextupdatetime(CTimeUtility::GetTodayTime(iNextDayNowTime, stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_ADVENTRESETTIME)));
	stQuestInfo.set_idailynextupdatetime(CTimeUtility::GetTodayTime(iNextDayNowTime, stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_DAILYRESETTIME)));

	//��ʼ����ҵı�����Ϣ
	ITEMDBINFO stItemInfo;

	//�����¿��������
	OneSlotInfo* pstItemInfo = stItemInfo.mutable_stitemslot()->add_stslots();
	pstItemInfo->set_iitemid(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_MONTHCARD));
	pstItemInfo->set_iitemnum(1);

	//��ʼ����ҵĺ�����Ϣ
	FRIENDDBINFO stFriendInfo;

	//��ʼ����ҵ��ʼ���Ϣ
	MAILDBINFO stMailInfo;

	//��ʼ�������ֶ�1
	RESERVED1DBINFO stReserved1;

	//��ʼ�������ֶ�2
	RESERVED2DBINFO stReserved2;

	//1.����һ�����Ϣ���뵽������
	if (!EncodeProtoData(stBaseInfo, *rstRequest.mutable_stbirthdata()->mutable_strbaseinfo()))
	{
		LOGERROR("Failed to encode base proto data, uin %u!\n", rstRequest.uin());
		return -1;
	}

	LOGDEBUG("Base proto Info compress rate %d:%zu, uin %u\n", stBaseInfo.ByteSize(), rstRequest.stbirthdata().strbaseinfo().size(), rstRequest.uin());

	//2.�����������Ϣ���뵽������
	if (!EncodeProtoData(stQuestInfo, *rstRequest.mutable_stbirthdata()->mutable_strquestinfo()))
	{
		LOGERROR("Failed to encode quest proto data, uin %u!\n", rstRequest.uin());
		return -2;
	}

	LOGDEBUG("quest proto Info compress rate %d:%zu, uin %u\n", stQuestInfo.ByteSize(), rstRequest.stbirthdata().strquestinfo().size(), rstRequest.uin());

	//3.�������Ʒ��Ϣ���뵽������
	if (!EncodeProtoData(stItemInfo, *rstRequest.mutable_stbirthdata()->mutable_striteminfo()))
	{
		LOGERROR("Failed to encode item proto data, uin %u!\n", rstRequest.uin());
		return -3;
	}

	LOGDEBUG("item proto Info compress rate %d:%zu, uin %u\n", stItemInfo.ByteSize(), rstRequest.stbirthdata().striteminfo().size(), rstRequest.uin());

	//4.����Һ�����Ϣ��Ϣ���뵽������
	if (!EncodeProtoData(stFriendInfo, *rstRequest.mutable_stbirthdata()->mutable_strfriendinfo()))
	{
		LOGERROR("Failed to encode friend proto data, uin %u!\n", rstRequest.uin());
		return -4;
	}

	LOGDEBUG("friend proto Info compress rate %d:%zu, uin %u\n", stFriendInfo.ByteSize(), rstRequest.stbirthdata().strfriendinfo().size(), rstRequest.uin());

	//5.������ʼ���Ϣ���뵽������
	if (!EncodeProtoData(stMailInfo, *rstRequest.mutable_stbirthdata()->mutable_strmailinfo()))
	{
		LOGERROR("Failed to encode mail proto data, uin %u!\n", rstRequest.uin());
		return -4;
	}

	LOGDEBUG("mail proto Info compress rate %d:%zu, uin %u\n", stMailInfo.ByteSize(), rstRequest.stbirthdata().strmailinfo().size(), rstRequest.uin());

	//6.�����Reserved1�ֶα��뵽������
	if (!EncodeProtoData(stReserved1, *rstRequest.mutable_stbirthdata()->mutable_strreserved1()))
	{
		LOGERROR("Failed to encode reserved1 proto data, uin %u!\n", rstRequest.uin());
		return -6;
	}

	LOGDEBUG("reserved1 proto Info compress rate %d:%zu, uin %u\n", stReserved1.ByteSize(), rstRequest.stbirthdata().strreserved1().size(), rstRequest.uin());

	//7.�����Reserved2�ֶα��뵽������
	if (!EncodeProtoData(stReserved2, *rstRequest.mutable_stbirthdata()->mutable_strreserved2()))
	{
		LOGERROR("Failed to encode reserved2 proto data, uin %u!\n", rstRequest.uin());
		return -7;
	}

	LOGDEBUG("reserved2 proto Info compress rate %d:%zu, uin %u\n", stReserved2.ByteSize(), rstRequest.stbirthdata().strreserved2().size(), rstRequest.uin());

	return 0;
}

//�����ɹ�
int CFetchRoleWorldHandler::OnFetchSuccess(const RoleID& stRoleID, const GameUserInfo& stUserInfo, bool bIsLogin, const KickerInfo& stKicker)
{
	unsigned uiUin = stRoleID.uin();

	//����World����ı����������
	CWorldRoleStatusWObj* pUserStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stRoleID);
	if (!pUserStatusObj)
	{
		LOGERROR("World No Cache Data: Uin = %u\n", uiUin);
		return -1;
	}

	int iZoneID = pUserStatusObj->GetZoneID();

	//����ǵ�¼����֤��ҵĲ�����״̬
	if (bIsLogin && (pUserStatusObj->GetRoleStatus() & EGUS_ONLINE))
	{
		LOGERROR("Fail to fetch role from DB, already online, uin %u\n", uiUin);
		return -2;
	}

	LOGDEBUG("Fetch ROLEDB OK, uin %u, from zone id %d\n", uiUin, iZoneID);

	//����World����һ���������Ϣ
	BASEDBINFO stBaseInfo;
	if (!DecodeProtoData(stUserInfo.strbaseinfo(), stBaseInfo))
	{
		LOGERROR("Failed to decode base proto data, uin %u\n", uiUin);
		return -3;
	}

	pUserStatusObj->GetRoleInfo().CopyFrom(stUserInfo);
	pUserStatusObj->SetRoleStatus(stBaseInfo.ustatus() | EGUS_ONLINE);

	//���ؿͻ��˲�ѯ���
	CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());

	World_FetchRole_Response* pstResp = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_fetchrole_response();
	pstResp->set_iresult(0);
	pstResp->mutable_stroleid()->CopyFrom(stRoleID);
	pstResp->set_bexist(true);
	pstResp->set_bislogin(bIsLogin);
	pstResp->mutable_stkicker()->CopyFrom(stKicker);
	pstResp->mutable_stuserinfo()->CopyFrom(stUserInfo);

	int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iZoneID);
	if (iRet)
	{
		LOGERROR("SendFetchRoleResponse to GS failed, uin %u, zone id %d\n", uiUin, iZoneID);
		return iRet;
	}

	return 0;
}

// ����ʧ����Ϣ
int CFetchRoleWorldHandler::SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());
    
    World_FetchRole_Response* rstResp = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_fetchrole_response();
    rstResp->mutable_stroleid()->CopyFrom(stRoleID);
    rstResp->set_iresult(T_WORLD_FETCHROLE_FAIED);
	
    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

	return iRet;
}

// ���ؽ�ɫ����
int CFetchRoleWorldHandler::SendFetchRoleResponse(const RoleID& stRoleID, int iReqID, const KickerInfo& stKicker)
{
    CWorldRoleStatusWObj* pWorldRoleStatusObj = WorldTypeK32<CWorldRoleStatusWObj>::GetByRoleID(stRoleID);
    if (!pWorldRoleStatusObj)
    {
        return -1;
    }

    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, 0, MSGID_WORLD_FETCHROLE_RESPONSE, stRoleID.uin());

    World_FetchRole_Response* pstResp = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_fetchrole_response();
    pstResp->set_iresult(T_SERVER_SUCCESS);
    pstResp->mutable_stroleid()->CopyFrom(stRoleID);

    pstResp->mutable_stuserinfo()->CopyFrom(pWorldRoleStatusObj->GetRoleInfo());
	pstResp->mutable_stkicker()->CopyFrom(stKicker);

    int iRet = CWorldMsgHelper::SendWorldMsgToWGS(m_stWorldMsg, iReqID);

    return iRet;
}

