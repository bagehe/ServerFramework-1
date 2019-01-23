#pragma once

#include "GameProtocol.hpp"

//��ֵ����
class CRechargeProxy
{
public:

	//��ֵ������
	virtual int OnRechargeReq(const std::string& strURI, unsigned int uiSessionFD, const char* pszCodeBuff, int iCodeLen) = 0;

	//��ֵ���ش���
	virtual int OnRechargeResp(const GameProtocolMsg& stMsg) = 0;

protected:

	//���͵���Ӧ������
	int SendRechargeMsgToWorld(GameProtocolMsg& stMsg, int iWorldID);

protected:

	//���ͳ���
	int m_iSendLen;

	//���ͻ�����
	char m_szSendBuff[2048];
};
