#include "GameProtocol.hpp"
#include "ghttp.h"
#include "json/reader.h"
#include "json/value.h"
#include "json/writer.h"
#include "TimeUtility.hpp"
#include "StringUtility.hpp"
#include "lib_md5.hpp"
#include "ThreadLogManager.hpp"

#include "YmnPlatformProxy.h"

using namespace ServerLib;
using namespace std;

//����AppID
static const char* YMN_FISHGAME_APPID = "1228";

//��è����֤��ַ
//static const char* YMN_PLATFORM_AUTH_URL = "http:\//mobile.bfun.cn/v1/login/check";	//�ϵ���֤��ַ
static const char* YMN_PLATFORM_AUTH_URL = "http://ymnsec.bianfeng.com/v1/login/check";	//�µ���֤��ַ

//��è���ȡʵ����֤��ַ
static const char* YMN_PLATFORM_REALNAME_URL = "http://ymn.bianfeng.com/v1/player/getRealName";

//��è�� ����AppKey
static const char* YMN_FISHGAME_APPKEY = "e8ee19a47d6c028e9ccdaf9701133769";

//���캯��
CYmnPlatformProxy::CYmnPlatformProxy()
{

}

//ƽ̨��֤
int CYmnPlatformProxy::PlatformAuth(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex)
{
	//��֤�˺�
	int iRet = AuthYmnAccount(stInfo, stResp, iThreadIndex);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to auth ymn account, ret %d, account %s, type %d\n", iRet, stInfo.straccount().c_str(), stInfo.itype());
		return iRet;
	}

	//��ȡʵ����֤��Ϣ
	GetRealNameStat(stInfo, stResp, iThreadIndex);

	return 0;
}

//��֤�˺�
int CYmnPlatformProxy::AuthYmnAccount(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex)
{
	//��������
	ghttp_request* pstRequest = ghttp_request_new();

	//��װURL
	if (ghttp_set_uri(pstRequest, (char*)YMN_PLATFORM_AUTH_URL) == -1)
	{
		ghttp_request_destroy(pstRequest);

		return -1;
	}

	if (ghttp_set_type(pstRequest, ghttp_type_post) == -1)
	{
		ghttp_request_destroy(pstRequest);
		return -2;
	}

	ghttp_set_sync(pstRequest, ghttp_sync);

	//��������ͷ
	ghttp_set_header(pstRequest, "Content-Type", "application/json");

	//���ó�ʱ
	ghttp_set_header(pstRequest, http_hdr_Timeout, "5");

	//��װ����Body
	Json::Value jRequest;
	Json::FastWriter jWriter;

	jRequest["appid"] = YMN_FISHGAME_APPID;
	jRequest["thirdparty"] = stInfo.strthirdparty();
	jRequest["sid"] = stInfo.straccount();
	std::string strBody = jWriter.write(jRequest);

	ghttp_set_body(pstRequest, (char*)strBody.c_str(), strBody.size());

	//��������
	ghttp_prepare(pstRequest);
	ghttp_status iStatus = ghttp_process(pstRequest);
	if (iStatus == ghttp_error)
	{
		ghttp_request_destroy(pstRequest);
		return -3;
	}

	//��ȡ����
	char* pstRespBody = ghttp_get_body(pstRequest);
	if (!pstRespBody)
	{
		ghttp_request_destroy(pstRequest);
		return -4;
	}

	//������
	Json::Value jResponse;
	Json::Reader jReader;
	if (!jReader.parse(pstRespBody, jResponse))
	{
		TRACE_THREAD(iThreadIndex, "Failed to json decode response, account %s, type %d\n", stInfo.straccount().c_str(), stInfo.itype());
		ghttp_request_destroy(pstRequest);
		return -5;
	}

	int iRetCode = jResponse["code"].asInt();
	if (iRetCode)
	{
		//��֤ʧ��
		TRACE_THREAD(iThreadIndex, "Failed to auth ymn, ret code %d, account %s, type %d\n", iRetCode, stInfo.straccount().c_str(), stInfo.itype());
		ghttp_request_destroy(pstRequest);
		return -6;
	}

	//��֤�ɹ�
	stResp.mutable_stinfo()->set_straccount(jResponse["data"]["uid"].asString());

	if (!jResponse["data"]["headimgurl"].isNull())
	{
		//ͷ��ID
		stResp.mutable_stinfo()->set_strpicid(jResponse["data"]["headimgurl"].asString());
	}

	//��������
	ghttp_request_destroy(pstRequest);

	return 0;
}

//��ȡʵ����֤��Ϣ
void CYmnPlatformProxy::GetRealNameStat(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex)
{
	//��������
	ghttp_request* pstRequest = ghttp_request_new();

	//Ĭ������Ϊδʵ��
	stResp.mutable_stinfo()->set_irealnamestat(REAL_STAT_NONAME);

	//��װURL
	if (ghttp_set_uri(pstRequest, (char*)YMN_PLATFORM_REALNAME_URL) == -1)
	{
		ghttp_request_destroy(pstRequest);
		return;
	}

	if (ghttp_set_type(pstRequest, ghttp_type_post) == -1)
	{
		ghttp_request_destroy(pstRequest);
		return;
	}

	ghttp_set_sync(pstRequest, ghttp_sync);

	//��������ͷ
	ghttp_set_header(pstRequest, "Content-Type", "application/x-www-form-urlencoded");

	//���ó�ʱ
	ghttp_set_header(pstRequest, http_hdr_Timeout, "5");

	//��װ����Body
	std::string strBody;
	
	char szParams[64] = { 0 };
	SAFE_SPRINTF(szParams, sizeof(szParams)-1, "%d", (int)CTimeUtility::GetNowTime());

	strBody += std::string("appid=") + YMN_FISHGAME_APPID;		// appid
	strBody += std::string("&area_id=") + stInfo.strplatformid();//type=0 ��è��platform_id; type=1 ����Ϸareaid
	strBody += std::string("&numid=") + stInfo.straccount();	//type=0 ��è��uid; type=1�������û��˺�
	strBody += std::string("&time=") + szParams;				//ʱ��
	strBody += std::string("&type=") + "0";						//0 ��è���û��� 1�����û�

	//sign���ɹ��� key = md5(appid+appkey)
	//sign = md5(appid=XXX&area_id=YYY.......&key)
	unsigned char szDigest[64] = { 0 };
	SAFE_SPRINTF(szParams, sizeof(szParams)-1, "%s%s", YMN_FISHGAME_APPID, YMN_FISHGAME_APPKEY);
	MD5DigestHex((unsigned char*)szParams, strlen(szParams), szDigest, true);

	std::string strCheckStr = strBody + std::string("&") + (char*)szDigest;
	MD5DigestHex((unsigned char*)strCheckStr.c_str(), strCheckStr.size(), szDigest, true);

	strBody += std::string("&sign=") + (char*)szDigest;

	ghttp_set_body(pstRequest, (char*)strBody.c_str(), strBody.size());

	//��������
	ghttp_prepare(pstRequest);
	ghttp_status iStatus = ghttp_process(pstRequest);
	if (iStatus == ghttp_error)
	{
		ghttp_request_destroy(pstRequest);
		return;
	}

	//��ȡ����
	char* pstRespBody = ghttp_get_body(pstRequest);
	if (!pstRespBody)
	{
		ghttp_request_destroy(pstRequest);
		return;
	}

	//������
	Json::Value jResponse;
	Json::Reader jReader;
	if (!jReader.parse(pstRespBody, jResponse))
	{
		TRACE_THREAD(iThreadIndex, "Failed to json decode response, account %s, type %d\n", stInfo.straccount().c_str(), stInfo.itype());
		ghttp_request_destroy(pstRequest);
		return;
	}

	int iRetCode = jResponse["code"].asInt();
	if (iRetCode)
	{
		//��֤ʧ��
		TRACE_THREAD(iThreadIndex, "Failed to get realname, ret code %d, account %s, type %d\n", iRetCode, stInfo.straccount().c_str(), stInfo.itype());
		ghttp_request_destroy(pstRequest);
		return;
	}

	//��ȡʵ����Ϣ�ɹ�
	stResp.mutable_stinfo()->set_irealnamestat(jResponse["data"]["status"].asInt());

	//��������
	ghttp_request_destroy(pstRequest);

	return;
}
