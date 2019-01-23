#include <string.h>

#include "GameProtocol.hpp"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "lib_md5.hpp"
#include "URLCoder.hpp"
#include "ServerBusManager.h"
#include "TimeUtility.hpp"
#include "IOHandler.hpp"
#include "OrderManager.h"

#include "YmnRechargeProxy.h"

using namespace ServerLib;

//��è�� ����AppKey
static const char* YMN_FISHGAME_APPKEY = "e8ee19a47d6c028e9ccdaf9701133769";

//��è�巵�ظ�ʽ��
//ע�⣺����Ĵ���ʽ�����Ҷ�
static const char* YMN_RESPONSE_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: %u\r\n\
Connection: keep-alive\r\n\
Cache-Control: private\r\n\r\n%s";

static GameProtocolMsg stSendMsg;

//��ֵ������
int CYmnRechargeProxy::OnRechargeReq(const std::string& strURI, unsigned int uiSessionFD, const char* pszCodeBuff, int iCodeLen)
{
	if (!pszCodeBuff)
	{
		SendRechargeResponseToClient(uiSessionFD, false);
		return -1;
	}

	//��ӡ�յ�������
	LOGDEBUG("Recv: %s\n", pszCodeBuff);

	//���urlencode
	std::string strURLEncode;
	bool bIsEncoded = GetRechargeToken(pszCodeBuff, "x-www-form-urlencoded", strURLEncode);

	//ǩ��У���㷨��
	//1.�������ֶ�Key���������ֶ�(�ų�sign),Ȼ��ƴ��appkey����md5,��sign�Ƚϣ�
	//2.ʾ����ʽ���£� a=1&b=2&c=3

	//��������
	m_mReqParams.clear();

	//�ҵ�������Ϣ��
	const char* pReqParams = strstr(pszCodeBuff, "\r\n\r\n");
	if (!pReqParams)
	{
		//û����Ϣ��
		SendRechargeResponseToClient(uiSessionFD, false);
		return -2;
	}

	//��������λ��
	pReqParams += strlen("\r\n\r\n");

	//��������
	bool bIsValue = false;
	std::string strKey;
	std::string strValue;

	for (unsigned i = 0; i < strlen(pReqParams); ++i)
	{
		if (pReqParams[i] == '=')
		{
			//����������value
			bIsValue = true;
			strValue.clear();
		}
		else if (pReqParams[i] == '&')
		{
			//�²���
			m_mReqParams[strKey] = strValue;

			bIsValue = false;
			strKey.clear();
		}
		else
		{
			//ʵ������
			if (!bIsValue)
			{
				strKey.push_back(pReqParams[i]);
			}
			else
			{
				strValue.push_back(pReqParams[i]);
			}
		}
	}

	//���һ������
	m_mReqParams[strKey] = strValue;

	//����У�鴮, ƴ�ӳ� key1=value1&key2=value2&app_key����ʽ��������sign
	std::string strCheckStr;
	for (std::map<std::string, std::string>::iterator it = m_mReqParams.begin(); it != m_mReqParams.end(); ++it)
	{
		if (it->first != "sign")
		{
			strCheckStr += it->first + "=" + URLEncode(it->second, bIsEncoded) + "&";
		}
	}

	//У�鴮����appkey
	strCheckStr += YMN_FISHGAME_APPKEY;

	//У��ǩ��
	unsigned char szDigest[64] = { 0 };
	MD5DigestHex((unsigned char*)strCheckStr.c_str(), strCheckStr.size(), szDigest, true);

	std::string strAppOrderID = m_mReqParams["app_order_id"];

	if (m_mReqParams["sign"].compare((char*)szDigest) != 0)
	{
		//��ǩʧ��
		LOGERROR("Failed to recharge, invalid sign, app order %s\n", strAppOrderID.c_str());
		SendRechargeResponseToClient(uiSessionFD, false);
		return -3;
	}

	//���֧��״̬,Ϊ0��ʾ֧��ʧ��
	if (atoi(m_mReqParams["pay_status"].c_str()) == 0)
	{
		LOGERROR("Failed to recharge, invalid pay status, app order %s\n", strAppOrderID.c_str());
		SendRechargeResponseToClient(uiSessionFD, false);
		return -2;
	}

	//����cp_private
	int iWorldID = 0;
	unsigned int uin = 0;
	int iRechargeID = 0;

	//��ʽΪ worldid_uin_rechargeid
	sscanf(m_mReqParams["cp_private"].c_str(), "%d_%u_%d", &iWorldID, &uin, &iRechargeID);

	//��鶩����
	int iTime = 0;
	unsigned uOrderUin = 0;
	sscanf(strAppOrderID.c_str(), "%10d%08u", &iTime, &uOrderUin);

	if (uOrderUin != uin || iTime < (CTimeUtility::GetNowTime() - 1 * 60 * 60))
	{
		//������У��ʧ��
		SendRechargeResponseToClient(uiSessionFD, false);
		return -4;
	}

	//�����Ƿ����
	if (COrderManager::Instance()->IsOrderIDExist(strAppOrderID))
	{
		//��������ߴ�����,ֱ�ӷ��سɹ�
		SendRechargeResponseToClient(uiSessionFD, true);
		return -5;
	}

	//���ͳ�ֵ����World
	stSendMsg.Clear();
	stSendMsg.mutable_sthead()->set_uisessionfd(uiSessionFD);
	stSendMsg.mutable_sthead()->set_uin(uin);
	stSendMsg.mutable_sthead()->set_uimsgid(MSGID_WORLD_USERRECHARGE_REQUEST);

	World_UserRecharge_Request* pstReq = stSendMsg.mutable_stbody()->mutable_m_stworld_userrecharge_request();
	pstReq->set_uin(uin);
	pstReq->set_strorderid(strAppOrderID);
	pstReq->set_irmb(atoi(m_mReqParams["amount"].c_str()));
	pstReq->set_irechargeid(iRechargeID);
	pstReq->set_itime(atoi(m_mReqParams["pay_time"].c_str()));
	pstReq->set_iplatform(LOGIN_PLATFORM_YMN);

	//���͵���Ӧ������
	int iRet = SendRechargeMsgToWorld(stSendMsg, iWorldID);
	if (iRet)
	{
		LOGERROR("Failed to send recharge to world, ret %d, app order %s\n", iRet, strAppOrderID.c_str());
		SendRechargeResponseToClient(uiSessionFD, false);
		return -6;
	}

	//���Ӷ����ż�¼
	OrderData stOneOrder;
	stOneOrder.strOrder = strAppOrderID;
	COrderManager::Instance()->AddOrder(stOneOrder, false);

	//��ղ���
	m_mReqParams.clear();

	return 0;
}

//��ֵ���ش���
int CYmnRechargeProxy::OnRechargeResp(const GameProtocolMsg& stMsg)
{
	//��ȡ������Ϣ
	const World_UserRecharge_Response& stResp = stMsg.stbody().m_stworld_userrecharge_response();
	
	//��ֵʧ��
	if (stResp.iresult() != 0)
	{
		//��set��ɾ��������
		COrderManager::Instance()->DeleteOrderID(stResp.strorderid());

		LOGERROR("Failed to user recharge, uin %u, recharge id %d, order id %s\n", stResp.uin(), stResp.irechargeid(), stResp.strorderid().c_str());
		SendRechargeResponseToClient(stMsg.sthead().uisessionfd(), false);
	}
	else
	{
		//��ֵ�ɹ�

		//�����¶�����¼
		OrderData stOneOrder;
		stOneOrder.strOrder = stResp.strorderid();
		stOneOrder.iTime = CTimeUtility::GetNowTime();
		stOneOrder.uiUin = stResp.uin();
		stOneOrder.iRechargeID = stResp.irechargeid();

		COrderManager::Instance()->AddOrder(stOneOrder, true);

		LOGDEBUG("Success to user recharge, uin %u, recharge id %d, order id %s\n", stResp.uin(), stResp.irechargeid(), stResp.strorderid().c_str());
		SendRechargeResponseToClient(stMsg.sthead().uisessionfd(), true);
	}

	return T_SERVER_SUCCESS;
}

//��ȡ��ֵ�����ֶ�
bool CYmnRechargeProxy::GetRechargeToken(const char* pszCodeBuff, const char* pszKey, std::string& strValue)
{
	if (!pszCodeBuff || !pszKey)
	{
		return false;
	}

	strValue.clear();

	const char* pValue = strstr(pszCodeBuff, pszKey);
	if (pValue == NULL)
	{
		return false;
	}
	else
	{
		//����key
		pValue += strlen(pszKey);
		for (int i = 0; pValue[i] != '&' && pValue[i] != ' ' && pValue[i] != '\n' && pValue[i] != '\0'; i++)
		{
			strValue.push_back(pValue[i]);
		}
	}

	return true;
}

//���ͷ���
int CYmnRechargeProxy::SendRechargeResponseToClient(unsigned int uiSessionFD, bool bSuccess)
{
	const char* pResult = bSuccess ? "success" : "fail";

	m_iSendLen = SAFE_SPRINTF(m_szSendBuff, sizeof(m_szSendBuff)-1, YMN_RESPONSE_FORMAT, strlen(pResult), pResult);

	//���ͷ���
	int iRet = CIOHandler::Instance()->SendToExternalClient(uiSessionFD, m_szSendBuff, m_iSendLen);
	if (iRet)
	{
		LOGERROR("Failed to send ymn recharge response, ret %d, session fd %u\n", iRet, uiSessionFD);
		return iRet;
	}

	return 0;
}
