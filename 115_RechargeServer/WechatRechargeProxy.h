#pragma once

#include <string>
#include <map>

#include "RechargeProxy.h"

//΢�ų�ֵ������
enum WechatRechargeErrorNo
{
	WECHAT_RECHARGE_SUCCESS		= 0,		//��ֵ�ɹ�
	WECHAT_RECHARGE_SERVER_ERROR= 10001,	//�����ڲ�����
	WECHAT_RECHARGE_SIGN_ERROR	= 10002,	//�Ƿ���ǩ��
	WETCHAT_RECHARGE_TYPE_ERROR	= 10003,	//�Ƿ���֪ͨ����
	WECHAT_RECHARGE_PAY_FAILED	= 10004,	//�Ƿ���֧��״̬
	WECHAT_RECHARGE_ORDER_STATUS= 10005,	//�Ƿ��Ķ���״̬
	WECHAT_RECHARGE_INVALID_NUM	= 10006,	//�Ƿ��Ĺ�������
};

class CWechatRechargeProxy : public CRechargeProxy
{
public:

	//��ֵ������
	virtual int OnRechargeReq(const std::string& strURI, unsigned int uiSessionFD, const char* pszCodeBuff, int iCodeLen);

	//��ֵ���ش���
	virtual int OnRechargeResp(const GameProtocolMsg& stMsg);

	//��ȡ����״̬
	void GetServiceStatus(unsigned int uiSessionFD);

	//��ȡ����ӿ�
	void GetServiceSchema(unsigned int uiSessionFD);

private:

	//��ȡ��ֵ�����ֶ�
	bool GetRechargeToken(const char* pszCodeBuff, const char* pszKey, std::string& strValue);

	//���ͳ�ֵ����
	int SendWechatRechargeResponse(unsigned int uiSessionFD, int iErrorNum);

	//������ȡ��Ϣ����
	int SendGetUserInfoResponse(unsigned int uiSessionFD, const std::string& strUserID, unsigned uin, const std::string& strNickName);

	//��ȡ�����Ϣ
	int GetRechargeUserInfo(unsigned int uiSessionFD, const char* pReqBody);

	//΢�Ź��ںų�ֵ
	int WechatRecharge(unsigned int uiSessionFD, bool bIsURLEncoded);

private:

	//HTTP�������
	std::map<std::string, std::string> m_mReqParams;
};
