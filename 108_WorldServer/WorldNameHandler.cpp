#include <string.h>

#include "ProtoDataUtility.hpp"
#include "ModuleHelper.hpp"
#include "WorldMsgHelper.hpp"
#include "LogoutHandler.hpp"
#include "LogAdapter.hpp"
#include "HashUtility.hpp"
#include "ErrorNumDef.hpp"

#include "WorldNameHandler.hpp"

using namespace ServerLib;

GameProtocolMsg CWorldNameHandler::m_stWorldMsg;

static unsigned int GetNameHash(const std::string& strName)
{
    long lHash = CHashUtility::BKDRHash(strName.c_str(), strName.size());

    return (unsigned)lHash;
}

CWorldNameHandler::~CWorldNameHandler()
{
}

int CWorldNameHandler::OnClientMsg(GameProtocolMsg* pMsg)
{
	ASSERT_AND_LOG_RTN_INT(pMsg);

	m_pMsg = (GameProtocolMsg*)pMsg;

	switch (m_pMsg->sthead().uimsgid())
	{
	case MSGID_ADDNEWNAME_REQUEST:
		{
			OnRequestAddNewName();
			break;
		}

	case MSGID_ADDNEWNAME_RESPONSE:
		{
			OnResponseAddNewName();
			break;
		}

	case MSGID_DELETENAME_REQUEST:
		{
			OnRequestDeleteName();
			break;
		}

	default:
		{
			break;
		}
	}

	return 0;
}

//������������Ĵ���
int CWorldNameHandler::OnRequestAddNewName()
{
	AddNewName_Request* pstReq = m_pMsg->mutable_stbody()->mutable_m_staddnewname_request();
	unsigned uiUin = pstReq->name_id();

    LOGDEBUG("AddNewName: Uin = %u\n", uiUin);

	//����Ϣͷ�е�uin�滻Ϊname��hash
	m_pMsg->mutable_sthead()->set_uin(GetNameHash(pstReq->strname()));

    //ת���������ֵ�����NameDB
    int iRet = CWorldMsgHelper::SendWorldMsgToNameDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("AddNewName Failed: iRet = %d\n", iRet);
		SendAddNewNameResponseToAccount(pstReq->strname(), uiUin, T_WORLD_SERVER_BUSY, pstReq->itype());

        return -2;
    }

    return 0;
}

//�������ַ��صĴ���
int CWorldNameHandler::OnResponseAddNewName()
{
	//ֱ��ת����AccountServer
	const AddNewName_Response& rstResp = m_pMsg->stbody().m_staddnewname_response();
	unsigned int uiUin = rstResp.name_id();
	int iResult = rstResp.iresult();

	SendAddNewNameResponseToAccount(rstResp.strname(), uiUin, iResult, rstResp.itype());

	return T_SERVER_SUCCESS;
}

int CWorldNameHandler::SendAddNewNameResponseToAccount(const std::string& strName, unsigned int uiUin, int iResult, int iType)
{
    CWorldMsgHelper::GenerateMsgHead(m_stWorldMsg, m_pMsg->sthead().uisessionfd(), MSGID_ADDNEWNAME_RESPONSE, uiUin);

	AddNewName_Response* pstResp = m_stWorldMsg.mutable_stbody()->mutable_m_staddnewname_response();
	pstResp->set_iresult(iResult);
	pstResp->set_name_id(uiUin);
	pstResp->set_strname(strName);
	pstResp->set_itype(iType);
    
    CWorldMsgHelper::SendWorldMsgToAccount(m_stWorldMsg);
	return 0;
}

//ɾ�����ֵ�������
int CWorldNameHandler::OnRequestDeleteName()
{
	DeleteName_Request* pstReq = m_pMsg->mutable_stbody()->mutable_m_stdeletename_request();
    LOGDEBUG("Delete Name: name = %s, type = %d\n", pstReq->strname().c_str(), pstReq->itype());

	//����Ϣͷ�е�uin�滻Ϊname��hash
	m_pMsg->mutable_sthead()->set_uin(GetNameHash(pstReq->strname()));

    //ת��ɾ�����ֵ�����NameDB
    int iRet = CWorldMsgHelper::SendWorldMsgToNameDB(*m_pMsg);
    if(iRet < 0)
    {
        LOGERROR("Delete Name Failed: iRet = %d\n", iRet);

        return -2;
    }

    return 0;
}




