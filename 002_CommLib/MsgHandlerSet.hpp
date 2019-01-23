#pragma once

#include <string.h>

#include "MsgHandler.hpp"

enum enMsgType
{
	EKMT_CLIENT = 1,    // �ͻ�����Ϣ
	EKMT_SERVER = 2,    // ��������Ϣ
};

struct TMsgHandler
{
	CMsgHandler* m_pHandler;       // ��Ϣ������
	enMsgType m_enMsgType;   // ��Ϣ����
};

const unsigned int MAX_MSG_HANDLER_NUMBER = 16384;

// ��Ϣ�����߹�����
class CMsgHandlerSet
{
public:
	virtual ~CMsgHandlerSet() {}

	// ��ʼ����Ϣ�����߼����еĸ�����Ϣ�����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
	virtual int Initialize(int iThreadIndex = -1) = 0;

	// ������Ϣid���ظ���Ϣ�Ĵ�����
	CMsgHandler* GetHandler(unsigned int uiMsgID, enMsgType enMsgType = EKMT_SERVER);

protected:
	CMsgHandlerSet();

	// ������Ϣidע�����Ĵ����ߣ�����ֵΪ0��ʾ�ɹ���������ʾʧ��
	int RegisterHandler(unsigned int uiMsgID, CMsgHandler* pHandler, enMsgType enMsgType = EKMT_SERVER);

protected:

	// �������ʾ����Ϣ�����߼���
	TMsgHandler m_apHandler[MAX_MSG_HANDLER_NUMBER];
};
