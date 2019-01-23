#include <string.h>

#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "NowTime.hpp"
#include "UnixTime.hpp"
#include "SeqConverter.hpp"
#include "StringUtility.hpp"
#include "RoleDBApp.hpp"
#include "AppDef.hpp"
#include "ConfigManager.hpp"
#include "ErrorNumDef.hpp"

#include "RoleDBCreateRoleHandler.hpp"

using namespace ServerLib;

CRoleDBCreateRoleHandler::CRoleDBCreateRoleHandler(DBClientWrapper* pDatabase, char** ppQueryBuff)
{
    m_pDatabase = pDatabase;
	m_ppQueryBuff = ppQueryBuff;
}

void CRoleDBCreateRoleHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
    m_pstRequestMsg = &stReqMsg;

	//����ͷ
	const GameCSMsgHead& stHead = m_pstRequestMsg->sthead();
    
	//������Ϣ
	GenerateMsgHead(pstHandleResult->stResponseMsg, stHead.uisessionfd(), MSGID_WORLD_CREATEROLE_RESPONSE, stHead.uin());
	World_CreateRole_Response* pstResp = pstHandleResult->stResponseMsg.mutable_stbody()->mutable_m_stworld_createrole_response();

	//������Ϣ
	const World_CreateRole_Request& stReq = m_pstRequestMsg->stbody().m_stworld_createrole_request();

    //��ȡ��ɫ������ʱ��
    unsigned short usCreateTime = 0;
    GetRoleCreateTime(usCreateTime);

	unsigned int uiDBSeq = 0;

    // ��Ӧ��Ϣ��
    pstResp->mutable_stroleid()->set_uin(stReq.uin());
    pstResp->mutable_stroleid()->set_uiseq(uiDBSeq);
	pstResp->set_irealnamestat(stReq.irealnamestat());
	pstResp->mutable_stkicker()->CopyFrom(stReq.stkicker());

    // �����е��û�����д�����ݱ�DBRoleInfo
    int iRet = InsertNewRoleRecord(stReq, uiDBSeq);
    if (iRet)
    {
		TRACE_THREAD(m_iThreadIdx, "Failed to insert new record, ret %d, uin %u\n", iRet, stReq.uin());

		pstResp->set_iresult(iRet);
		return;
    }

    DEBUG_THREAD(m_iThreadIdx, "create new role success, uin: %u\n", pstResp->stroleid().uin());

	pstResp->set_iresult(T_SERVER_SUCCESS);
    return;
}

int CRoleDBCreateRoleHandler::InsertNewRoleRecord(const World_CreateRole_Request& stReq, unsigned int uiSeq)
{
    //��XML�ļ��ж�ȡ����
    const OneDBInfo* pstDBInfoConfig = ConfigManager::Instance()->GetRoleDBConfigByIndex(m_iThreadIdx);
    if(!pstDBInfoConfig)
    {
        TRACE_THREAD(m_iThreadIdx, "Failed to insert new role record, invalid cfg, thread idx %d\n", m_iThreadIdx);
        return T_ROLEDB_PARA_ERR;
    }

    //����Ҫ���������ݿ������Ϣ
    m_pDatabase->SetMysqlDBInfo(pstDBInfoConfig->szDBHost, pstDBInfoConfig->szUserName, pstDBInfoConfig->szUserPasswd, pstDBInfoConfig->szDBName);

    //��ʼ��SQL���
    int iLength = 0;
    int iRet = GenerateQueryString(stReq, uiSeq, *m_ppQueryBuff, MAX_QUERYBUFF_SIZE-1, iLength);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate insert query sql string, ret %d\n", iRet);
        return iRet;
    }

    //ִ��SQL���
    iRet = m_pDatabase->ExecuteRealQuery(*m_ppQueryBuff, iLength, false);
    if(iRet)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to insert user data, uin %u, ret %d\n", stReq.uin(), iRet);
        return iRet;
    }

    DEBUG_THREAD(m_iThreadIdx, "The number of affected rows is %d\n", m_pDatabase->GetAffectedRows());
    return 0;
}

//��ȡ��ҽ�ɫ������ʱ��,�Ǵ�2014-01-01��ʼ�������
void CRoleDBCreateRoleHandler::GetRoleCreateTime(unsigned short& usCreateTime)
{
    CUnixTime stNow(time(NULL));
    stNow.GetDaysAfterYear(GAME_START_YEAR, usCreateTime);
}

//���ɲ����SQL Query���
int CRoleDBCreateRoleHandler::GenerateQueryString(const World_CreateRole_Request& stReq, unsigned int uiSeq, char* pszBuff, int iBuffLen, int& iLength)
{
    if(!pszBuff)
    {
        TRACE_THREAD(m_iThreadIdx, "Fail to generate query string, invlaid buff pointer!\n");
        return T_ROLEDB_PARA_ERR;
    }

    unsigned int uiUin = stReq.uin();

    SAFE_SPRINTF(pszBuff, iBuffLen-1, "insert into %s(uin,seq,base_info,quest_info,item_info,friend_info,mail_info,reserved1,reserved2) values(%u,%u,", MYSQL_USERINFO_TABLE, uiUin, uiSeq);
    char* pEnd = pszBuff + strlen(pszBuff);

    MYSQL& stDBConn = m_pDatabase->GetCurMysqlConn();

    const GameUserInfo& rstUserInfo = stReq.stbirthdata();

    //1.��һ�����Ϣ base_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strbaseinfo().c_str(), rstUserInfo.strbaseinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //2.��ҵ�������Ϣ quest_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strquestinfo().c_str(), rstUserInfo.strquestinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //3.��ҵ���Ʒ��Ϣ item_info
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.striteminfo().c_str(), rstUserInfo.striteminfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //4.��ҵĺ�����Ϣ
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strfriendinfo().c_str(), rstUserInfo.strfriendinfo().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

	//5.��ҵ��ʼ���Ϣ
	*pEnd++ = '\'';
	pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strmailinfo().c_str(), rstUserInfo.strmailinfo().size());
	*pEnd++ = '\'';
	*pEnd++ = ',';

    //6.��ҵı����ֶ�1
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved1().c_str(), rstUserInfo.strreserved1().size());
    *pEnd++ = '\'';
    *pEnd++ = ',';

    //7.��ҵı����ֶ�2
    *pEnd++ = '\'';
    pEnd += mysql_real_escape_string(&stDBConn, pEnd, rstUserInfo.strreserved2().c_str(), rstUserInfo.strreserved2().size());
    *pEnd++ = '\'';
    *pEnd++ = ')';

    iLength = pEnd - pszBuff;

    return T_SERVER_SUCCESS;
}
