#pragma once

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "ExchangePublic.hpp"
#include "ExchangeProtocolEngine.hpp"
#include "ExchangeHandlerSet.hpp"

//���߳���Ϣ����
class CExchangeMsgProcessor
{
public:
	static CExchangeMsgProcessor* Instance();
	~CExchangeMsgProcessor();

	//��ʼ��
	int Initialize();

	//���մ�����Ϣ
	void ProcessRecvMsg(int& iNewMsgCount);

private:
	CExchangeMsgProcessor();

private:

	//ʵ����Ϣ����
	int m_iCodeLen;

	//���ͻ�����
	unsigned char m_szCodeBuff[MAX_CODE_LEN];
};
