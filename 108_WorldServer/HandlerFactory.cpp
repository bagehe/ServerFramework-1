#include <string.h>

#include "LogAdapter.hpp"

#include "HandlerFactory.hpp"

using namespace ServerLib;

IHandler* CHandlerFactory::m_apHandler[MAX_HANDLER_NUMBER] = {0};

IHandler* CHandlerFactory::GetHandler(const unsigned int uiMsgID)
{
	if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
        TRACESVR("NULL handler, msg id: %d\n", uiMsgID);
		return NULL;
	}

	return m_apHandler[uiMsgID];
}

int CHandlerFactory::RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler)
{
	if(uiMsgID >= (unsigned)MAX_HANDLER_NUMBER)
	{
		return -1;
	}

	m_apHandler[uiMsgID] = (IHandler*)pHandler;

	return 0;
}


