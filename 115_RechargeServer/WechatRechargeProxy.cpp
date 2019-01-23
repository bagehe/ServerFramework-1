#include <string.h>

#include "GameProtocol.hpp"
#include "json/value.h"
#include "json/reader.h"
#include "json/writer.h"
#include "ErrorNumDef.hpp"
#include "LogAdapter.hpp"
#include "lib_md5.hpp"
#include "URLCoder.hpp"
#include "ServerBusManager.h"
#include "TimeUtility.hpp"
#include "IOHandler.hpp"
#include "OrderManager.h"

#include "WechatRechargeProxy.h"

using namespace ServerLib;

//����΢�Ź��ں�ID
static const int WECHAT_PUBLIC_ACCOUNTID = 1094;

//����secret key
static const char* WECHAT_RECHARGE_SECRETKEY = "b12812fbbfab7b98ba17dcc62c46b252";


//΢�ų�ֵ���ظ�ʽ��
//ע�⣺����Ĵ���ʽ�����Ҷ�
static const char* WECHAT_RESPONSE_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: %u\r\n\
Connection: keep-alive\r\n\
Cache-Control: private\r\n\r\n%s";

//΢����ȡ�����Ϣ���ظ�ʽ��
//ע�⣺����Ĵ���ʽ�����Ҷ�
static const char* GETUSERINFO_RESPONSE_FORMAT = "HTTP/1.1 200 OK\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Content-Length: %u\r\n\
Connection: keep-alive\r\n\
X-Error-Code: %d\r\n\
Cache-Control: private\r\n\r\n%s";

static GameProtocolMsg stSendMsg;

//��ֵ������
int CWechatRechargeProxy::OnRechargeReq(const std::string& strURI, unsigned int uiSessionFD, const char* pszCodeBuff, int iCodeLen)
{
	if (!pszCodeBuff)
	{
		return -1;
	}

	//��ӡ�յ�������
	LOGDEBUG("Recv: %s\n", pszCodeBuff);

	//���urlencode
	std::string strURLEncode;
	bool bIsURLEncoded = GetRechargeToken(pszCodeBuff, "x-www-form-urlencoded", strURLEncode);

	//��������
	m_mReqParams.clear();

	//�ҵ�������Ϣ��
	const char* pReqParams = strstr(pszCodeBuff, "\r\n\r\n");
	if (!pReqParams)
	{
		//û����Ϣ��
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

	if (strURI.compare("/userinfo") == 0)
	{
		//��ȡ�����Ϣ
		return GetRechargeUserInfo(uiSessionFD, pReqParams);
	}
	else if(strURI.compare("/wechatrecharge") == 0)
	{
		//΢�Ź��ںų�ֵ
		return WechatRecharge(uiSessionFD, bIsURLEncoded);
	}

	return 0;
}

//��ֵ���ش���
int CWechatRechargeProxy::OnRechargeResp(const GameProtocolMsg& stMsg)
{
	unsigned uiSessionFD = stMsg.sthead().uisessionfd();

	switch (stMsg.sthead().uimsgid())
	{
	case MSGID_WORLD_USERRECHARGE_RESPONSE:
	{
		//��ҳ�ֵ�ķ���
		const World_UserRecharge_Response& stResp = stMsg.stbody().m_stworld_userrecharge_response();

		//��ֵʧ��
		if (stResp.iresult() != 0)
		{
			//��set��ɾ��������
			COrderManager::Instance()->DeleteOrderID(stResp.strorderid());

			LOGERROR("Failed to user recharge, uin %u, recharge id %d, order id %s\n", stResp.uin(), stResp.irechargeid(), stResp.strorderid().c_str());
			SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_SERVER_ERROR);
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
			SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_SUCCESS);
		}
	}
	break;

	case MSGID_WORLD_GETUSERINFO_RESPONSE:
	{
		//��ȡ΢�ų�ֵ�����Ϣ�ķ���
		const World_GetUserInfo_Response& stResp = stMsg.stbody().m_stworld_getuserinfo_response();

		SendGetUserInfoResponse(uiSessionFD, stResp.straccount(), stResp.uin(), stResp.strnickname());
	}
	break;

	default:
		break;
	}

	return T_SERVER_SUCCESS;
}

//��ȡ����״̬
void CWechatRechargeProxy::GetServiceStatus(unsigned int uiSessionFD)
{
	//todo jasonxiong �������ӷ��񽡿�״̬�ж�
	m_iSendLen = SAFE_SPRINTF(m_szSendBuff, sizeof(m_szSendBuff) - 1, WECHAT_RESPONSE_FORMAT, 0, "");

	//���ͷ���
	int iRet = CIOHandler::Instance()->SendToExternalClient(uiSessionFD, m_szSendBuff, m_iSendLen);
	if (iRet)
	{
		LOGERROR("Failed to send ymn recharge response, ret %d, session fd %u\n", iRet, uiSessionFD);
		return;
	}

	return;
}

//��ȡ����ӿ�
void CWechatRechargeProxy::GetServiceSchema(unsigned int uiSessionFD)
{
	Json::Value jValue;
	Json::FastWriter jWriter;

	jValue["apihub"] = "2.0.0";

	Json::Value jInfo;
	jInfo["title"] = "FishGameAPI";
	jInfo["description"] = "get fish user info";
	jInfo["version"] = "v1";
	jValue["info"] = jInfo;

	jValue["basePath"] = "";

	Json::Value jServiceAPIs;
	
	Json::Value jUserInfo;
	Json::Value jPostUserInfo;
	jPostUserInfo["summary"] = "userinfo";
	jPostUserInfo["description"] = "get fish user info";
	jPostUserInfo["group"] = "Agent";
	jPostUserInfo["response_example"] = "{\"userid\":\"jason\",\"numid\":1000,\"nickname\":\"jason\"}";
	jPostUserInfo["tags"].append("Agent");
	
	//�������� properties
	Json::Value jProperties;
	Json::Value jPropBody;
	jPropBody["type"] = "object";

	Json::Value jSubPropBody;
	Json::Value jParams;
	jParams["name"] = "wid";
	jParams["required"] = true;
	jParams["description"] = "wechat";
	jParams["type"] = "number";
	jParams["format"] = "int";
	jSubPropBody["wid"] = jParams;

	jParams.clear();
	jParams["name"] = "numid";
	jParams["required"] = true;
	jParams["description"] = "fish user numid";
	jParams["type"] = "number";
	jParams["format"] = "int";
	jSubPropBody["numid"] = jParams;

	jPropBody["properties"] = jSubPropBody;

	jProperties["body"] = jPropBody;

	jPostUserInfo["properties"] = jProperties;

	//������
	Json::Value jErrors;
	jErrors["code"] = 9999;
	jErrors["comment"] = "Param Error";
	jPostUserInfo["errors"].append(jErrors);

	//����
	Json::Value jExample;
	jExample["wid"] = 124543;
	jExample["numid"] = 1001;
	jPostUserInfo["example"] = jExample;

	jUserInfo["post"] = jPostUserInfo;
		
	jServiceAPIs["userinfo"] = jUserInfo;

	jValue["paths"] = jServiceAPIs;

	std::string strSchema = jWriter.write(jValue);
	m_iSendLen = SAFE_SPRINTF(m_szSendBuff, sizeof(m_szSendBuff) - 1, WECHAT_RESPONSE_FORMAT, strSchema.size(), strSchema.c_str());

	//���ͷ���
	int iRet = CIOHandler::Instance()->SendToExternalClient(uiSessionFD, m_szSendBuff, m_iSendLen);
	if (iRet)
	{
		LOGERROR("Failed to send ymn recharge response, ret %d, session fd %u\n", iRet, uiSessionFD);
		return;
	}

	return;
}

//��ȡ��ֵ�����ֶ�
bool CWechatRechargeProxy::GetRechargeToken(const char* pszCodeBuff, const char* pszKey, std::string& strValue)
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
int CWechatRechargeProxy::SendWechatRechargeResponse(unsigned int uiSessionFD, int iErrorNum)
{
	char szResult[32] = { 0 };
	if (iErrorNum == 0)
	{
		//�ɹ�
		SAFE_SPRINTF(szResult, sizeof(szResult)-1, "OK");
	}
	else
	{
		//ʧ��
		SAFE_SPRINTF(szResult, sizeof(szResult)-1, "%d", iErrorNum);
	}

	m_iSendLen = SAFE_SPRINTF(m_szSendBuff, sizeof(m_szSendBuff) - 1, WECHAT_RESPONSE_FORMAT, strlen(szResult), szResult);

	//���ͷ���
	int iRet = CIOHandler::Instance()->SendToExternalClient(uiSessionFD, m_szSendBuff, m_iSendLen);
	if (iRet)
	{
		LOGERROR("Failed to send ymn recharge response, ret %d, session fd %u\n", iRet, uiSessionFD);
		return iRet;
	}

	return 0;
}

//������ȡ��Ϣ����
int CWechatRechargeProxy::SendGetUserInfoResponse(unsigned int uiSessionFD, const std::string& strUserID, unsigned uin, const std::string& strNickName)
{
	Json::FastWriter jWriter;
	Json::Value jValue;

	jValue["userid"] = strUserID;
	jValue["numid"] = uin;
	jValue["nickname"] = strNickName;

	int iErrorNum = strNickName.empty() ? 9999 : 0;

	std::string strValue = jWriter.write(jValue);
	m_iSendLen = SAFE_SPRINTF(m_szSendBuff, sizeof(m_szSendBuff) - 1, GETUSERINFO_RESPONSE_FORMAT, strValue.size(), iErrorNum, strValue.c_str());

	//���ͷ���
	int iRet = CIOHandler::Instance()->SendToExternalClient(uiSessionFD, m_szSendBuff, m_iSendLen);
	if (iRet)
	{
		LOGERROR("Failed to send ymn recharge response, ret %d, session fd %u\n", iRet, uiSessionFD);
		return iRet;
	}

	return 0;
}

//��ȡ�����Ϣ
int CWechatRechargeProxy::GetRechargeUserInfo(unsigned int uiSessionFD, const char* pReqBody)
{
	Json::Value jValue;
	Json::Reader jReader;

	if (!jReader.parse(pReqBody, jValue))
	{
		//����bodyʧ��
		SendGetUserInfoResponse(uiSessionFD, "", 0, "");
		return -12;
	}

	//�жϹ��ں�
	if (jValue["wid"].asInt() != WECHAT_PUBLIC_ACCOUNTID)
	{
		//�Ƿ��Ĺ��ں�
		SendGetUserInfoResponse(uiSessionFD, "", 0, "");
		return -10;
	}

	unsigned int uin = jValue["numid"].asInt();

	//������ȡ������Ϣ����
	stSendMsg.Clear();
	stSendMsg.mutable_sthead()->set_uisessionfd(uiSessionFD);
	stSendMsg.mutable_sthead()->set_uin(uin);
	stSendMsg.mutable_sthead()->set_uimsgid(MSGID_WORLD_GETUSERINFO_REQUEST);

	World_GetUserInfo_Request* pstReq = stSendMsg.mutable_stbody()->mutable_m_stworld_getuserinfo_request();
	pstReq->set_uin(uin);
	pstReq->set_iplatformid(LOGIN_PLATFORM_WECHAT);

	//���͵���Ӧ������,todo jasonxiong WorldID��ʱʹ��1
	int iRet = SendRechargeMsgToWorld(stSendMsg, 1);
	if (iRet)
	{
		LOGERROR("Failed to send recharge msg to world, ret %d, uin %u\n", iRet, uin);
		SendGetUserInfoResponse(uiSessionFD, "", uin, "");
		return -11;
	}

	//��ղ���
	m_mReqParams.clear();

	return 0;
}

//΢�Ź��ںų�ֵ
int CWechatRechargeProxy::WechatRecharge(unsigned int uiSessionFD, bool bIsURLEncoded)
{
	if (bIsURLEncoded)
	{
		//�Ƚ���URLDecode
		m_mReqParams["body"] = URLDecode(m_mReqParams["body"]);
	}

	//signУ�鷽ʽ�� signature = md5(customer_id + secret + timestamp + body)
	std::string strCheckStr = m_mReqParams["customer_id"] + WECHAT_RECHARGE_SECRETKEY + m_mReqParams["timestamp"] + m_mReqParams["body"];

	//У��ǩ��
	unsigned char szDigest[64] = { 0 };
	MD5DigestHex((unsigned char*)strCheckStr.c_str(), strCheckStr.size(), szDigest, true);

	//����body
	Json::Value jBody;
	Json::Reader jReader;

	if (!jReader.parse(m_mReqParams["body"], jBody))
	{
		LOGERROR("Failed to parse wechat recharge body, invalid json\n");
		return -20;
	}

	if (jBody["order_no"].isNull())
	{
		//�Ƿ���body
		LOGERROR("Failed to wechat recharge, invalid body!\n");
		return -21;
	}

	std::string strAppOrderID = jBody["order_no"].asString();

	if (m_mReqParams["signature"].compare((char*)szDigest) != 0)
	{
		//��ǩʧ��
		LOGERROR("Failed to recharge, invalid sign, app order %s\n", strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_SIGN_ERROR);
		return -22;
	}

	//���֪ͨ�����Ƿ��Ƿ���
	if (jBody["type"].asString().compare("TYPE_DELIVER") != 0)
	{
		//���Ƿ�����֪ͨ
		LOGERROR("Failed to recharge, invalid type %s, app order %s\n", jBody["type"].asString().c_str(), strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WETCHAT_RECHARGE_TYPE_ERROR);
		return -23;
	}

	//���֧��״̬,false��ʾ֧��ʧ��
	if (!jBody["is_success"].asBool())
	{
		LOGERROR("Failed to recharge, invalid pay status, app order %s\n", strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_PAY_FAILED);
		return -24;
	}

	//��鶩��״̬
	if (jBody["status"].asString().compare("PAID") != 0)
	{
		LOGERROR("Failed to recharge, invalid order status, status %s, app order %s\n", jBody["status"].asString().c_str(), strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_ORDER_STATUS);
		return -25;
	}

	//��ȡ�������ID
	unsigned uin = atoi(jBody["numid"].asString().c_str());

	//��ȡ֧���۸�
	int iRMB = atoi(jBody["par_value"].asCString());

	//�Ƿ���Զ���
	//bool bIsTestOrder = jBody["is_test"].isNull() ? false : jBody["is_test"].asBool();

	//��ȡ����ID
	int iRechargeID = 0;
	sscanf(jBody["item_code"].asString().c_str(), "prop%d", &iRechargeID);

	//��ȡ����������ֻ��Ϊ1
	if (jBody["item_qty"].asInt() != 1)
	{
		LOGERROR("Failed to recharge , invalid num %d, order id %s\n", jBody["item_qty"].asInt(), strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_INVALID_NUM);
		return -26;
	}

	//�����Ƿ����
	if (COrderManager::Instance()->IsOrderIDExist(strAppOrderID))
	{
		//��������ߴ�����,ֱ�ӷ��سɹ�
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_SUCCESS);
		return -27;
	}

	//���ͳ�ֵ����World
	stSendMsg.Clear();
	stSendMsg.mutable_sthead()->set_uisessionfd(uiSessionFD);
	stSendMsg.mutable_sthead()->set_uin(uin);
	stSendMsg.mutable_sthead()->set_uimsgid(MSGID_WORLD_USERRECHARGE_REQUEST);

	World_UserRecharge_Request* pstReq = stSendMsg.mutable_stbody()->mutable_m_stworld_userrecharge_request();
	pstReq->set_uin(uin);
	pstReq->set_strorderid(strAppOrderID);
	pstReq->set_irmb(iRMB);
	pstReq->set_irechargeid(iRechargeID);
	pstReq->set_itime(atoi(m_mReqParams["timestamp"].c_str()));
	pstReq->set_iplatform(LOGIN_PLATFORM_WECHAT);

	//���͵���Ӧ������,Ŀǰֻ��1����
	int iRet = SendRechargeMsgToWorld(stSendMsg, 1);
	if (iRet)
	{
		LOGERROR("Failed to send recharge to world, ret %d, app order %s\n", iRet, strAppOrderID.c_str());
		SendWechatRechargeResponse(uiSessionFD, WECHAT_RECHARGE_SERVER_ERROR);
		return -28;
	}

	//���Ӷ����ż�¼
	OrderData stOneOrder;
	stOneOrder.strOrder = strAppOrderID;
	COrderManager::Instance()->AddOrder(stOneOrder, false);

	//��ղ���
	m_mReqParams.clear();

	return 0;
}
