#include <assert.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "SessionManager.hpp"

#include "ClientClosedHandler.hpp"

using namespace ServerLib;

CClientClosedHandler::CClientClosedHandler()
{
}

void CClientClosedHandler::OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult, TNetHead_V2* pstNetHead)
{
    ASSERT_AND_LOG_RTN_VOID(pstNetHead);

    // ���cache�е�session���
    unsigned int uiSessionFD = ntohl(pstNetHead->m_uiSocketFD);
    TRACESVR("Handling ClientClosedRequest from lotus server, sockfd: %u\n", uiSessionFD);
    CSessionManager::Instance()->DeleteSession(uiSessionFD);

	return;
}
