
#include "GameProtocol.hpp"
#include "CommonDef.hpp"
#include "ExchangePublic.hpp"

#include "ExchangeHandlerSet.hpp"

using namespace ServerLib;

int CExchangeHandlerSet::RegisterAllHandlers()
{
	//���߶һ�Handler
	int iRet = RegisterHandler(MSGID_WORLD_ONLINEEXCHANGE_REQUEST, &m_stOnlineExchangeHandler);
	if (iRet)
	{
		return -1;
	}
	m_stOnlineExchangeHandler.SetThreadIdx(m_iThreadIndex);

	//��ȡ����Handler
	iRet = RegisterHandler(MSGID_WORLD_GETCARDNO_REQUEST, &m_stGetCardNoHandler);
	if (iRet)
	{
		return -2;
	}
	m_stGetCardNoHandler.SetThreadIdx(m_iThreadIndex);

    return 0;
}

CExchangeHandlerSet::CExchangeHandlerSet():
	m_stGetCardNoHandler(&m_oDBClient, &m_pQueryBuff)
{

}

CExchangeHandlerSet::~CExchangeHandlerSet()
{
}

int CExchangeHandlerSet::Initialize(int iThreadIndex)
{
	m_iThreadIndex = iThreadIndex;

	int iRet = RegisterAllHandlers();
	if (iRet)
	{
		return iRet;
	}

	//��ʼ��DB
	m_oDBClient.Init(true);

	//DB����������
	m_pQueryBuff = new char[MAX_QUERYBUFF_SIZE];

	return 0;
}

