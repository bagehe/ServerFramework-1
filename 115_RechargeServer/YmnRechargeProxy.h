#pragma once

#include <string>
#include <map>

#include "RechargeProxy.h"

class CYmnRechargeProxy : public CRechargeProxy
{
public:

	//��ֵ������
	virtual int OnRechargeReq(const std::string& strURI, unsigned int uiSessionFD, const char* pszCodeBuff, int iCodeLen);

	//��ֵ���ش���
	virtual int OnRechargeResp(const GameProtocolMsg& stMsg);

private:

	//��ȡ��ֵ�����ֶ�
	bool GetRechargeToken(const char* pszCodeBuff, const char* pszKey, std::string& strValue);

	//���ͷ���
	int SendRechargeResponseToClient(unsigned int uiSessionFD, bool bSuccess);

private:

	//HTTP�������
	std::map<std::string, std::string> m_mReqParams;
};
