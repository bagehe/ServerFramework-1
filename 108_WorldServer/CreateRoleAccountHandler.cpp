#include <string.h>

#include "ProtoDataUtility.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"
#include "ModuleHelper.hpp"

#include "WorldMsgHelper.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
#include "ErrorNumDef.hpp"

#include "CreateRoleAccountHandler.hpp"

using namespace ServerLib;

CCreateRoleAccountHandler::~CCreateRoleAccountHandler()
{

}

int CCreateRoleAccountHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
    ASSERT_AND_LOG_RTN_INT(pMsg);

    m_pMsg = (GameProtocolMsg*)pMsg;

    switch (m_pMsg->sthead().uimsgid())
    {
        case MSGID_ACCOUNT_CREATEROLE_REQUEST:
        {
            OnRequestCreateRoleAccount();
            break;
        }
        case MSGID_WORLD_CREATEROLE_RESPONSE:
        {
            OnResponseCreateRoleAccount();
            break;
        }
        default:
            {
                break;
            }
    }

    return 0;
}


int CCreateRoleAccountHandler::OnRequestCreateRoleAccount()
{
    const CreateRole_Account_Request& rstRequest = m_pMsg->stbody().m_staccountcreaterolerequest();
    unsigned int uiUin = m_pMsg->sthead().uin();
    if (uiUin == 0)
    {
        LOGERROR("Wrong CreateRoleAccount Request, Uin %u\n", uiUin);
        return -1;
    }

    LOGDEBUG("Correct Uin CreateRoleAccount Request, Uin %u\n", uiUin);

    //ת��������ɫ�ʺŵ������DB Server
    int iRet = SendCreateRoleRequestToDBSvr(rstRequest.sznickname());
    if(iRet)
    {
        CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->sthead().uisessionfd(), MSGID_ACCOUNT_CREATEROLE_RESPONSE, uiUin);
        m_stWorldMsg.mutable_stbody()->mutable_m_staccountcreateroleresponse()->set_iresult(T_WORLD_CANNOT_CREATE_ROLE);

        CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);
    }

    return iRet;
}

int CCreateRoleAccountHandler::OnResponseCreateRoleAccount()
{
    const World_CreateRole_Response& rstResponse = m_pMsg->stbody().m_stworld_createrole_response();

    unsigned int uiUin = rstResponse.stroleid().uin();
    if (uiUin == 0)
    {
        LOGERROR("Wrong CreateRoleAccount Response, Uin %d\n", uiUin);
        return -1;
    }

    // ת����MsgID_Account_CreateRole_Response���͸�Account
    int iRet = SendCreateRoleResponseToAccount();

    LOGDEBUG("CreateRoleAccount Response To Account, Uin %d\n", uiUin);

    return iRet;
}

int CCreateRoleAccountHandler::SendCreateRoleRequestToDBSvr(const std::string& szNickName)
{
    //������Ϣͷ
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->sthead().uisessionfd(), 
		MSGID_WORLD_CREATEROLE_REQUEST, m_pMsg->sthead().uin());

    //���ɴ�����ɫ�ʺŵ���Ϣ��
    World_CreateRole_Request* pstRequest = m_stWorldMsg.mutable_stbody()->mutable_m_stworld_createrole_request();

    int iRet = InitBirthInfo(*pstRequest, szNickName);
    if(iRet)
    {
        LOGERROR("Fail to init user init birth info, uin %u, ret 0x%0x\n", m_pMsg->sthead().uin(), iRet);
        return iRet;
    }

    iRet = CWorldMsgHelper::SendWorldMsgToRoleDB(m_stWorldMsg);

    return iRet;
}

int CCreateRoleAccountHandler::SendCreateRoleResponseToAccount()
{
    //��ʼ����Ϣͷ
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->sthead().uisessionfd(), 
		MSGID_ACCOUNT_CREATEROLE_RESPONSE, m_pMsg->sthead().uin());

    const World_CreateRole_Response& rstResp = m_pMsg->stbody().m_stworld_createrole_response();
    CreateRole_Account_Response* pstAccountResp = m_stWorldMsg.mutable_stbody()->mutable_m_staccountcreateroleresponse();

    pstAccountResp->set_iresult(rstResp.iresult());
	pstAccountResp->set_uworldid(CModuleHelper::GetWorldID());
    pstAccountResp->mutable_stroleid()->CopyFrom(rstResp.stroleid());

    int iRet = CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);

    return iRet;
}

int CCreateRoleAccountHandler::InitBirthInfo(World_CreateRole_Request& rstRequest, const std::string& strNickName)
{
    rstRequest.set_uin(m_pMsg->sthead().uin());
	rstRequest.set_world(CModuleHelper::GetWorldID());

    //��ʼ����һ�������
    unsigned int uTimeNow = time(NULL);

	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

    BASEDBINFO stBaseInfo;
    stBaseInfo.set_sznickname(strNickName);
    stBaseInfo.set_icreatetime(uTimeNow);

    //��ʼ����Դ
    for(int i=RESOURCE_TYPE_INVALID; i<RESOURCE_TYPE_MAX; ++i)
    {
        stBaseInfo.add_iresources(0);
    }
	stBaseInfo.set_iresources(RESOURCE_TYPE_COIN, stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_COIN));

	//��ʼ����̨
	stBaseInfo.mutable_stweapon()->set_iweaponid(stBaseCfgMgr.GetBirthInitConfig(BIRTH_INIT_WEAPON));

    //��ʼ����ҵ�������Ϣ
    QUESTDBINFO stQuestInfo;

    //��ʼ����ҵı�����Ϣ
    ITEMDBINFO stItemInfo;

    //��ʼ����ҵĺ�����Ϣ
    FRIENDDBINFO stFriendInfo;

	//��ʼ����ҵ��ʼ���Ϣ
	MAILDBINFO stMailInfo;

    //��ʼ�������ֶ�1
    RESERVED1DBINFO stReserved1;

    //��ʼ�������ֶ�2
    RESERVED2DBINFO stReserved2;

    //1.����һ�����Ϣ���뵽������
    if(!EncodeProtoData(stBaseInfo, *rstRequest.mutable_stbirthdata()->mutable_strbaseinfo()))
    {
        LOGERROR("Failed to encode base proto data, uin %u!\n", rstRequest.uin());
        return -1;
    }

    LOGDEBUG("Base proto Info compress rate %d:%zu, uin %u\n", stBaseInfo.ByteSize(), rstRequest.stbirthdata().strbaseinfo().size(), rstRequest.uin());

    //2.�����������Ϣ���뵽������
    if(!EncodeProtoData(stQuestInfo, *rstRequest.mutable_stbirthdata()->mutable_strquestinfo()))
    {
        LOGERROR("Failed to encode quest proto data, uin %u!\n", rstRequest.uin());
        return -2;
    }

    LOGDEBUG("quest proto Info compress rate %d:%zu, uin %u\n", stQuestInfo.ByteSize(), rstRequest.stbirthdata().strquestinfo().size(), rstRequest.uin());

    //3.�������Ʒ��Ϣ���뵽������
    if(!EncodeProtoData(stItemInfo, *rstRequest.mutable_stbirthdata()->mutable_striteminfo()))
    {
        LOGERROR("Failed to encode item proto data, uin %u!\n", rstRequest.uin());
        return -3;
    }

    LOGDEBUG("item proto Info compress rate %d:%zu, uin %u\n", stItemInfo.ByteSize(), rstRequest.stbirthdata().striteminfo().size(), rstRequest.uin());

    //4.����Һ�����Ϣ��Ϣ���뵽������
    if(!EncodeProtoData(stFriendInfo, *rstRequest.mutable_stbirthdata()->mutable_strfriendinfo()))
    {
        LOGERROR("Failed to encode friend proto data, uin %u!\n", rstRequest.uin());
        return -4;
    }

    LOGDEBUG("friend proto Info compress rate %d:%zu, uin %u\n", stFriendInfo.ByteSize(), rstRequest.stbirthdata().strfriendinfo().size(), rstRequest.uin());

	//5.������ʼ���Ϣ���뵽������
	if (!EncodeProtoData(stMailInfo, *rstRequest.mutable_stbirthdata()->mutable_strmailinfo()))
	{
		LOGERROR("Failed to encode mail proto data, uin %u!\n", rstRequest.uin());
		return -5;
	}

	LOGDEBUG("mail proto Info compress rate %d:%zu, uin %u\n", stMailInfo.ByteSize(), rstRequest.stbirthdata().strmailinfo().size(), rstRequest.uin());

    //6.�����Reserved1�ֶα��뵽������
    if(!EncodeProtoData(stReserved1, *rstRequest.mutable_stbirthdata()->mutable_strreserved1()))
    {
        LOGERROR("Failed to encode reserved1 proto data, uin %u!\n", rstRequest.uin());
        return -6;
    }

    LOGDEBUG("reserved1 proto Info compress rate %d:%zu, uin %u\n", stReserved1.ByteSize(), rstRequest.stbirthdata().strreserved1().size(), rstRequest.uin());

    //7.�����Reserved2�ֶα��뵽������
    if(!EncodeProtoData(stReserved2, *rstRequest.mutable_stbirthdata()->mutable_strreserved2()))
    {
        LOGERROR("Failed to encode reserved2 proto data, uin %u!\n", rstRequest.uin());
        return -7;
    }

    LOGDEBUG("reserved2 proto Info compress rate %d:%zu, uin %u\n", stReserved2.ByteSize(), rstRequest.stbirthdata().strreserved2().size(), rstRequest.uin());

    return 0;
}

