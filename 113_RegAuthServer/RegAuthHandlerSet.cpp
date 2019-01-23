
#include "GameProtocol.hpp"
#include "CommonDef.hpp"

#include "RegAuthHandlerSet.hpp"

using namespace ServerLib;

int CRegAuthHandlerSet::RegisterAllHandlers()
{
    //ע�ᴴ��ƽ̨�ʺŵ�Handler
    int iRet = RegisterHandler(MSGID_REGAUTH_REGACCOUNT_REQUEST, &m_stRegisterAccountHandler, EKMT_CLIENT);
    if (iRet)
    {
        return -1;
    }

	iRet = RegisterHandler(MSGID_REGAUTHDB_ADDACCOUNT_RESPONSE, &m_stRegAuthDBAddHandler, EKMT_CLIENT);
    if(iRet)
    {
        return -2;
    }

	iRet = RegisterHandler(MSGID_REGAUTHDB_FETCH_RESPONSE, &m_stRegAuthDBFetchHandler, EKMT_CLIENT);
    if(iRet)
    {
        return -3;
    }

    //ע����֤ƽ̨�ʺŵ�Handler
	iRet = RegisterHandler(MSGID_REGAUTH_AUTHACCOUNT_REQUEST, &m_stAuthAccountHandler, EKMT_CLIENT);
    if(iRet)
    {
        return -4;
    }

	iRet = RegisterHandler(MSGID_LOGOUTSERVER_REQUEST, &m_stClientClosedHandler, EKMT_CLIENT);
    if (iRet)
    {
        return -7;
    }

	iRet = RegisterHandler(MSGID_LOGOUTSERVER_RESPONSE, &m_stClientClosedHandler, EKMT_CLIENT);
    if (iRet)
    {
        return -8;
    }

	//ƽ̨��֤����Handler
	iRet = RegisterHandler(MSGID_REGAUTH_PLATFORMAUTH_RESPONSE, &m_stAuthAccountHandler);
	if (iRet)
	{
		return -9;
	}

	//RegAuthDB��֤����Handler
	iRet = RegisterHandler(MSGID_REGAUTH_AUTHACCOUNT_RESPONSE, &m_stAuthAccountHandler);
	if (iRet)
	{
		return -10;
	}

	//���߳�Handler
	iRet = RegisterHandler(MSGID_REGAUTH_PLATFORMAUTH_REQUEST, &m_stPlatformAuthHandler);
	if (iRet)
	{
		return -11;
	}
	m_stPlatformAuthHandler.SetThreadIdx(m_iThreadIndex);

    return 0;
}

CRegAuthHandlerSet::CRegAuthHandlerSet()
{

}

CRegAuthHandlerSet::~CRegAuthHandlerSet()
{
}

int CRegAuthHandlerSet::Initialize(int iThreadIndex)
{
	m_iThreadIndex = iThreadIndex;

	int iRet = RegisterAllHandlers();
	if (iRet)
	{
		return iRet;
	}

	return 0;
}

