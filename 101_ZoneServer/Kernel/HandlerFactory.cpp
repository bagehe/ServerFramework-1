#include <string.h>

#include "Kernel/HandlerFactory.hpp"
#include "LogAdapter.hpp"
#include "CommDefine.h"
#include "Kernel/ConfigManager.hpp"

using namespace ServerLib;

IHandler* CHandlerFactory::m_apHandler[MAX_HANDLER_NUMBER];
TZoneMsgConfig CHandlerFactory::m_astMsgConfig[MAX_HANDLER_NUMBER];


IHandler* CHandlerFactory::GetHandler(const unsigned int uiMsgID, EZoneMsgType enMsgType)
{
	if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
		return NULL;
	}

	if (!m_apHandler[uiMsgID])
	{
		return NULL;
	}

	TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
	if (!pstMsgConfig)
	{
		return NULL;
	}

    // �ͻ�����Ϣֻ�ܵ��ÿͻ���Handler, ��������Ϣ����ֱ�ӵ��ÿͻ���Handler
    if(enMsgType == pstMsgConfig->m_enMsgType || EKMT_SERVER == enMsgType)
    {
        return m_apHandler[uiMsgID];
    }
    else
    {
        return NULL;
    }
}

TZoneMsgConfig* CHandlerFactory::GetMsgConfig(const unsigned int uiMsgID)
{
	if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
		TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
		return NULL;
	}

	return &m_astMsgConfig[uiMsgID];
}

int CHandlerFactory::RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler, 
	EZoneMsgType enMsgType, unsigned int uiHandlerFlag)
{
	if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
		TRACESVR("Inv MsgID: %d\n", uiMsgID);
		return -1;
	}

    // ��ֹ�ظ�ע��
    if (m_apHandler[uiMsgID])
    {
        TRACESVR("Duplicated Handler Registered: uiMsgID = %d, Handler = %p\n", uiMsgID, pHandler);
        abort();
    }

	TZoneMsgConfig* pstMsgConfig = CHandlerFactory::GetMsgConfig(uiMsgID);
	if (!pstMsgConfig)
	{
		abort();
	}

	m_apHandler[uiMsgID] = (IHandler*)pHandler;
    pstMsgConfig->m_enMsgType = enMsgType;
	pstMsgConfig->m_uiMsgFlag = uiHandlerFlag;

	pstMsgConfig->m_iMsgInterval = DEFAULT_MSG_INTERVAL;

	return 0;
}


void CHandlerFactory::DisabledMsg(unsigned int uiMsgID)
{
	if (uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
		TRACESVR("DisabledMsg failed, MsgID:%u\n", uiMsgID);
		return;
	}

	m_astMsgConfig[uiMsgID].m_bDisabled = true;
}

void CHandlerFactory::ClearAllDisableMsg()
{
	for (int i = 0; i < MAX_HANDLER_NUMBER; i++)
	{
		m_astMsgConfig[i].m_iMsgInterval = 0;
		m_astMsgConfig[i].m_bDisabled = false;
	}
}
bool CHandlerFactory::IsMsgDisabled(const unsigned int uiMsgID)
{
	if (uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
		TRACESVR("DisabledMsg failed, MsgID:%u\n", uiMsgID);
		return false;
	}

	return m_astMsgConfig[uiMsgID].m_bDisabled;
}
