#include <algorithm>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "GameProtocol.hpp"
#include "TimeUtility.hpp"
#include "SectionConfig.hpp"
#include "SocketOperator.hpp"
#include "LogAdapter.hpp"
#include "ServerBusManager.h"
#include "Statistic.hpp"
#include "ServerStatistic.hpp"

#include "IOHandler.hpp"

using namespace ServerLib;

int CIOHandler::m_iAppCmd = 0;

CIOHandler* CIOHandler::Instance()
{
	static CIOHandler* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CIOHandler;
	}

	return pInstance;
}

CIOHandler::CIOHandler()
{

}

CIOHandler::~CIOHandler()
{

}

int CIOHandler::Initialize(bool bNagelOff)
{
	m_bNagelOff = bNagelOff;
	LOGDEBUG("Nagel off: %s\n", m_bNagelOff ? "true" : "false");

	//��ʼ��������
	m_stSafeguradStrategy.Initialize(&m_stSafegurad, APP_CONFIG_FILE);
	m_stSafegurad.SetStrategy(&m_stSafeguradStrategy);
	m_stSafegurad.Reset();

	//����FDPool
	m_pstFDPool = new CFDPool;
	m_pstFDPool->Initialize();

	//��ʼ��AppCmd
	m_iAppCmd = 0;
	m_tLastCheckTimeout = 0;
	m_tLastSafeguradTimeout = 0;

	//ƽ̨��ֵ�����ʼ��
	m_apstRechargeProxy[LOGIN_PLATFORM_YMN] = new CYmnRechargeProxy;

	//΢�Ź��ںų�ֵ�����ʼ��
	m_apstRechargeProxy[LOGIN_PLATFORM_WECHAT] = new CWechatRechargeProxy;

	//��ʼ��ͨ��BUS
	int iRet = CServerBusManager::Instance()->Init("RechargeServer", CConfigManager::Instance()->GetBusConfigMgr());
	if (iRet)
	{
		printf("Failed to init server bus, ret %d\n", iRet);
		return -1;
	}

	return 0;
}

int CIOHandler::CreateEpoll()
{
	//����Epoll
	int iRet = m_stEpollWrapper.EpollCreate(FD_SIZE);
	if (iRet < 0)
	{
		LOGERROR("In CIOHandler::CreateEpoll EpollCreate Ret:%d\n", iRet);
		return -1;
	}

	//����Epoll�¼����ʱ��Ϊ4����
	m_stEpollWrapper.SetEpollWaitingTime(4);

	//ע��Epoll�������������
	m_stEpollWrapper.SetHandler_Error(CIOHandler::OnError);
	m_stEpollWrapper.SetHandler_Read(CIOHandler::OnRead);
	m_stEpollWrapper.SetHandler_Write(CIOHandler::OnWrite);

	return 0;
}

int CIOHandler::CreateAllListener()
{
	m_uiListenerNumber = 0;

	//�����ⲿ������
	const TAddressGroup& stExternalAddressGroup = CConfigManager::Instance()->GetExternalListenerAddress();
	int iSendBuffSize = CConfigManager::Instance()->GetExternalSNDBufferSize();
	for (int i = 0; i < stExternalAddressGroup.m_iNumber; ++i)
	{
		const TAddress& stAddress = stExternalAddressGroup.m_astAddress[i];
		int iRet = CreateOneListener(stAddress.m_uiIP, stAddress.m_ushPort, iSendBuffSize);
		if (iRet < 0)
		{
			return -1;
		}

		++m_uiListenerNumber;
	}

	return 0;
}

int CIOHandler::CreateOneListener(unsigned int uiIP, unsigned short ushPort, unsigned int uiSendBufferSize)
{
	int iRet = -1;

	CServerListener& rstLotusListener = m_astLotusListener[m_uiListenerNumber];
	rstLotusListener.Init(uiIP, ushPort, uiSendBufferSize);

	//�����׽���
	iRet = rstLotusListener.CreateTCPSocket();
	if (iRet)
	{
		LOGERROR("In CIOHandler::CreateOneListener CreateTCPSocket Ret:%d\n", iRet);
		return -1;
	}

	//����Epoll���õ��׽��ֲ���
	iRet = rstLotusListener.SetForEpoll();
	if (iRet)
	{
		LOGERROR("In CIOHandler::CreateOneListener SetForEpoll Ret:%d\n", iRet);
		return -2;
	}

	//���÷��ͻ������ߴ�
	iRet = rstLotusListener.SetSendBufferSize(uiSendBufferSize);
	if (iRet)
	{
		LOGERROR("In CIOHandler::CreateOneListener SetSendBufferSize Ret:%d\n", iRet);
		return -3;
	}

	//�󶨵�ַ
	iRet = rstLotusListener.Bind(uiIP, ushPort);
	if (iRet)
	{
		struct in_addr stAddr;
		stAddr.s_addr = uiIP;

		LOGERROR("In CIOHandler::CreateOneListener Bind on %s:%d Ret:%d\n", inet_ntoa(stAddr), ushPort, iRet);
		return -1;
	}

	//��ʼ����
	iRet = rstLotusListener.Listen();
	if (iRet)
	{
		LOGERROR("In CIOHandler::CreateOneListener Listen Ret:%d\n", iRet);
		return -1;
	}

	int iFD = rstLotusListener.GetFD();

	//����Epoll����
	AddFD(iFD);

	m_pstFDPool->SetListeningForExternalClient(iFD);

	LOGDEBUG("In CIOHandler::CreateOneListener Add Listener FD:%d IP:%u Port:%d OK\n", iFD, uiIP, ushPort);

	return 0;
}

CServerListener* CIOHandler::GetListenerByFD(int iListeningSocket)
{
	if (iListeningSocket < 0)
	{
		return NULL;
	}

	CServerListener *pstLotusListener = NULL;

	for (int i = 0; i < (int)m_uiListenerNumber; i++)
	{
		pstLotusListener = &m_astLotusListener[m_uiListenerNumber];
		if (iListeningSocket == pstLotusListener->GetFD())
		{
			return pstLotusListener;
		}
	}

	return NULL;
}

int CIOHandler::ReCreateOneListener(CServerListener *pstLotusListener)
{
	if (NULL == pstLotusListener)
	{
		return -1;
	}

	int iRet = pstLotusListener->CreateTCPSocket();
	if (iRet < 0)
	{
		LOGERROR("In CIOHandler::ReCreateOneListener CreateTCPSocket Ret:%d\n", iRet);
		return -1;
	}

	// ����Epoll���õ��׽��ֲ���
	iRet = pstLotusListener->SetForEpoll();
	if (iRet < 0)
	{
		LOGERROR("In CIOHandler::ReCreateOneListener SetForEpoll Ret:%d\n", iRet);
		return -1;
	}

	// �󶨵�ַ
	iRet = pstLotusListener->Bind(pstLotusListener->m_uiIP, pstLotusListener->m_ushPort);
	if (iRet < 0)
	{
		LOGERROR("In CIOHandler::ReCreateOneListener Bind Ret:%d\n", iRet);
		return -1;
	}

	// ��ʼ����
	iRet = pstLotusListener->Listen();
	if (iRet < 0)
	{
		LOGERROR("In CIOHandler::ReCreateOneListener Listen Ret:%d\n", iRet);
		return -1;
	}

	int iFD = pstLotusListener->GetFD();

	// ����Epoll����
	AddFD(iFD);

	m_pstFDPool->SetListeningForExternalClient(iFD);

	LOGDEBUG("In CIOHandler::ReCreateOneListener Add Listener FD:%d IP:%u Port:%d OK\n", iFD, pstLotusListener->m_uiIP, pstLotusListener->m_ushPort);

	return 0;
}

int CIOHandler::CheckIO()
{
	m_stEpollWrapper.EpollWait();

	// ���Listen״̬
	for (int i = 0; i < (int)m_uiListenerNumber; i++)
	{
		CServerListener *pstLotusListener = &m_astLotusListener[i];
		if (LotusListener_UnListen == pstLotusListener->m_iState)
		{
			ReCreateOneListener(pstLotusListener);
		}
	}

	return 0;
}

int CIOHandler::CheckOffline()
{
	time_t tNow = time(0);

	//����ⲿ�����Ƿ����
	int iFD;
	int iExternalConnectionNumber = 0;
	int iResetNumber = 0;

	for (iFD = 0; iFD < (int)FD_SIZE; ++iFD)
	{
		//������ⲿ����
		if (!m_pstFDPool->IsConnectedByExternalClient(iFD))
		{
			continue;
		}

		TExternalClientSocket* pstExternalClientSocket = m_pstFDPool->GetExternalSocketByFD(iFD);
		if (pstExternalClientSocket == NULL)
		{
			continue;
		}

		++iExternalConnectionNumber;

		//��ʼ��Ƶ������
		pstExternalClientSocket->m_uiRecvPacketNumber = 0;

		//���δ�չ���̨����Ӧ������ʱ���ߵ�ʱ����̣���ֹ���⹥��
		if (pstExternalClientSocket->m_iSendFlag == ESCF_NO_PACKET_TO_CLIENT)
		{
			if (tNow - (int)pstExternalClientSocket->m_uiCreateTime >=
				CConfigManager::Instance()->GetIdleConnectionTimeout())
			{
				LOGDEBUG("In CIOHandler::CheckTimeOut, iFD(%d) is closed by not having Server rsp.\n", iFD);
				Reset(iFD, TIMEOUT_SVR_NOT_RSP);
				++iResetNumber;
			}
		}
		else
		{
			//����Ӧ�������жϿͻ�����һ�η���Ϣ��ʱ�䣬�ж��Ƿ������
			if (tNow - (int)pstExternalClientSocket->m_uiRecvClientTimeStamp >=
				CConfigManager::Instance()->GetIdleClientTimeout())
			{
				LOGDEBUG("In CIOHandler::CheckTimeOut, iFD(%d) is closed by not recving message too long.\n", iFD);
				Reset(iFD, TIMEOUT_IDLE_CLIENT);
				++iResetNumber;
			}
			//��ʱ��û�յ���Ӧ��
			else if (tNow - (int)pstExternalClientSocket->m_uiRecvSvrTimeStamp >=
				CConfigManager::Instance()->GetIdleServerTimeout())
			{
				LOGDEBUG("In CIOHandler::CheckTimeOut, iFD(%d) is closed by Server not rsp message too long.\n", iFD);
				Reset(iFD, TIMEOUT_IDLE_SVR);
				++iResetNumber;
			}
		}
	}

	LOGDEBUG("In CIOHandler::CheckOffline, Check %d External Connect, Reset %d.\n", iExternalConnectionNumber, iResetNumber);

	return 0;
}

//���ͨ��BUS
int CIOHandler::CheckBus()
{
	static GameProtocolMsg stMsg;
	SERVERBUSID stFromBusID;

	//���ղ�����Bus��Ϣ
	int iHandlerMsgNum = 0;
	int iRet = 0;
	while (true)
	{
		if (iHandlerMsgNum > 100)
		{
			//һ����ദ��100��
			break;
		}

		stFromBusID.Reset();
		stFromBusID.usServerID = GAME_SERVER_WORLD;

		m_iRecvLen = 0;
		iRet = CServerBusManager::Instance()->RecvOneMsg(m_szRecvBuff, sizeof(m_szRecvBuff) - 1, m_iRecvLen, stFromBusID);
		if (iRet || m_iRecvLen <= (int)sizeof(unsigned short))
		{
			//û����Ϣ
			break;
		}

		//������Ϣ
		stMsg.Clear();
		if (!stMsg.ParseFromArray(m_szRecvBuff + sizeof(unsigned short), m_iRecvLen - sizeof(unsigned short)))
		{
			LOGERROR("Failed to decode proto msg!\n");
			return -1;
		}

		//������Ϣ
		ProcessBusMsg(stMsg);

		//�Ͽ��ⲿ����
		TExternalClientSocket* pstExternalSocket = m_pstFDPool->GetExternalSocketBySession(stMsg.sthead().uisessionfd());
		if (pstExternalSocket)
		{
			Reset(pstExternalSocket->m_iSocketFD, 0);
		}

		//���Ӵ�����Ϣ����
		++iHandlerMsgNum;
	}

	return 0;
}

int CIOHandler::CheckTimeOut()
{
	time_t tNow = time(NULL);

	//��Сʱ����һ��
	if (tNow - m_tLastCheckTimeout >= 30 * 60)
	{
		m_tLastCheckTimeout = tNow;

		//�����������߼��
		CheckOffline();
	}

	//��Сʱ����һ��
	if (tNow - m_tLastSafeguradTimeout >= 30 * 60)
	{
		m_tLastSafeguradTimeout = tNow;

		PrintStat();

		m_stSafegurad.Timeout();
	}

	return 0;
}

int CIOHandler::OnError(int iFD)
{
	if (CIOHandler::Instance()->m_pstFDPool->IsListeningForExternalClient(iFD))
	{
		// ˵��listen����  ��ô���½�������
		// �����δ������½�������Listen
		// ֻ�ܷ�ѭ���������ˣ�
		CServerListener *pstLotusListener = CIOHandler::Instance()->GetListenerByFD(iFD);
		if (pstLotusListener)
		{
			pstLotusListener->m_iState = LotusListener_UnListen;
		}
	}

	return 0;
}

int CIOHandler::OnRead(int iFD)
{
	CFDPool* pFDPool = CIOHandler::Instance()->m_pstFDPool;

	int iRet = 0;
	if (pFDPool->IsListeningForExternalClient(iFD))
	{
		//���Լ����˿�
		iRet = CIOHandler::Instance()->Accept(iFD);
	}
	else if (pFDPool->IsConnectedByExternalClient(iFD))
	{
		//�����ⲿ
		iRet = CIOHandler::Instance()->OnReadFromExternal(iFD);
	}

	return iRet;
}

int CIOHandler::OnWrite(int iFD)
{
	LOGDEBUG("In CIOHandler::OnWrite, FD:%d OK\n", iFD);

	return 0;
}

int CIOHandler::AddFD(int iFD)
{
	m_stEpollWrapper.EpollAdd(iFD);
	m_stEpollWrapper.SetNonBlock(iFD);

	if (m_bNagelOff)
	{
		m_stEpollWrapper.SetNagleOff(iFD);
	}

	m_pstFDPool->SetFDActive(iFD);
	return 0;
}

int CIOHandler::PrintStat()
{
	LOGDEBUG("CurConnnectNum:%d, NewConnectNum:%d, CloseConnectNum:%d, C2SPacketNum:%d, S2CPacketNum:%d\n",
		m_stSafegurad.m_iCurExternalConnectNum, m_stSafegurad.m_iCurNewConnectNum,
		m_stSafegurad.m_iCurNewCloseConnectNum,
		m_stSafegurad.m_iCurUpPackCounter, m_stSafegurad.m_iCurDownPacketCounter);

	return 0;
}

int CIOHandler::Accept(int iListeningFD)
{
	unsigned int uiRemoteIP = 0;
	unsigned short ushRemotePort = 0;
	int iFD = -1;

	iFD = CSocketOperator::Accept(iListeningFD, uiRemoteIP, ushRemotePort);
	if (iFD < 0)
	{
		LOGERROR("In CIOHandler::Accept Accept Ret:%d\n", iFD);
		return -1;
	}

	if (!OnAccept(uiRemoteIP))
	{
		LOGERROR("In CIOHandler::Accept OnAccept Failed!\n");
		CSocketOperator::Close(iFD);
		return -2;
	}

	TExternalClientSocket* pstExternalClientSocket = m_pstFDPool->GetExternalSocketByFD(iFD);
	if (!pstExternalClientSocket)
	{
		CSocketOperator::Close(iFD);
		return -6;
	}

	//����ܷ����ر���
	if (!m_stSafegurad.IsAcceptConnect())
	{
		m_stSafegurad.IncreaseRefuseAcceptNum();
		CSocketOperator::Close(iFD);
		return -7;
	}

	m_stSafegurad.IncreaseNewConnectNum();

	pstExternalClientSocket->m_iSocketFD = iFD;
	pstExternalClientSocket->m_uiSrcIP = uiRemoteIP;
	pstExternalClientSocket->m_ushSrcPort = ushRemotePort;
	pstExternalClientSocket->m_uiCreateTime = time(NULL);
	pstExternalClientSocket->m_uiRecvClientTimeStamp = 0;
	pstExternalClientSocket->m_uiRecvSvrTimeStamp = 0;
	pstExternalClientSocket->m_iSendFlag = 0;
	pstExternalClientSocket->m_uiRecvEndOffset = 0;
	pstExternalClientSocket->m_uiRecvPacketNumber = 0;

	pstExternalClientSocket->m_uiSocketFD_NBO = htonl((unsigned int)pstExternalClientSocket->m_iSocketFD);
	pstExternalClientSocket->m_uiSrcIP_NBO = htonl(pstExternalClientSocket->m_uiSrcIP);
	pstExternalClientSocket->m_ushSrcPort_NBO = htons(pstExternalClientSocket->m_ushSrcPort);
	pstExternalClientSocket->m_uiCreateTime_NBO = htonl(pstExternalClientSocket->m_uiCreateTime);

	m_pstFDPool->SetConnectedByExternalClient(iFD);

	m_stSafegurad.IncreaseExternalConnectNum();

	//���÷��ͻ���ߴ�
	int iOptLength = sizeof(socklen_t);
	int iOptValue = CConfigManager::Instance()->GetExternalSNDBufferSize();

	if (setsockopt(iFD, SOL_SOCKET, SO_SNDBUF, (const void *)&iOptValue, iOptLength))
	{
		LOGERROR("Set Send Buffer Size to %d Failed!\n", iOptValue);
		return -4;
	}

	if (!getsockopt(iFD, SOL_SOCKET, SO_SNDBUF, (void *)&iOptValue, (socklen_t *)&iOptLength))
	{
		//TRACESVR(LOG_LEVEL_NOTICE, "Set Send Buffer of Socket:%d.\n", iOptValue);
	}

	iOptValue = CConfigManager::Instance()->GetExternalRCVBufferSize();

	if (setsockopt(iFD, SOL_SOCKET, SO_RCVBUF, (const void *)&iOptValue, iOptLength))
	{
		LOGERROR("Set Send Buffer Size to %d Failed!\n", iOptValue);
		return -5;
	}

	if (!getsockopt(iFD, SOL_SOCKET, SO_RCVBUF, (void *)&iOptValue, (socklen_t *)&iOptLength))
	{
		//TRACESVR(LOG_LEVEL_NOTICE, "Set Send Buffer of Socket:%d.\n", iOptValue);
	}

	AddFD(iFD);

	LOGDEBUG("In CIOHandler::Accept New Connection FD:%d\n", iFD);

	return 0;
}

int CIOHandler::OnReadFromExternal(int iFD)
{
	//��ȡ�ⲿ����
	TExternalClientSocket* pstExternalClientSocket = m_pstFDPool->GetExternalSocketByFD(iFD);
	if (!pstExternalClientSocket)
	{
		LOGERROR("In CIOHandler::OnReadFromExternal, pstExternalClientSocket=NULL\n");
		return -1;
	}

	struct in_addr stAddr;
	stAddr.s_addr = pstExternalClientSocket->m_uiSrcIP;

	//��ˢ
	if ((int)pstExternalClientSocket->m_uiRecvPacketNumber >= CConfigManager::Instance()->GetClientPacketMaxFrequencyNum())
	{
		//����ʧ�ܻ��߹ر�
		LOGERROR("In CIOHandler::OnReadFromExternal, RecvReqLimit Overload, Close Socket %s.\n", inet_ntoa(stAddr));
		Reset(iFD, ERROR_EXTERAL_RECV_REQ_OVERLOAD);
		return -2;
	}

	//����������
	if (!m_stSafegurad.IsAcceptReceive())
	{
		m_stSafegurad.IncreaseRefuseReciveNum();
		LOGERROR("In CIOHandler::OnReadFromExternal, AcceptReceive Overload, Close Socket\n");
		Reset(iFD, ERROR_EXTERAL_ACCEPT_RECV_OVERLOAD);
		return -3;
	}

	//���û�����
	pstExternalClientSocket->m_uiRecvEndOffset = 0;

	int iRet = CSocketOperator::Recv(iFD, sizeof(pstExternalClientSocket->m_szRecvBuffer), pstExternalClientSocket->m_szRecvBuffer);

	LOGDEBUG("In CIOHandler::OnReadFromExternal, On Recv FD:%d Recv Byte:%d\n", iFD, iRet);

	if (iRet < 0)
	{
		//����ʧ�ܻ��߹ر�
		if (iRet == -2)
		{
			LOGDEBUG("In CIOHandler::OnReadFromExternal, Client Close FD:%d, So Reset\n", iFD);
		}

		LOGDEBUG("Recv Err:line=%d,src=%s,fd=%d,createTime=%u,Errno=%d,ErrStr=%s,iRet=%d\n", __LINE__,
			inet_ntoa(stAddr), pstExternalClientSocket->m_iSocketFD, pstExternalClientSocket->m_uiCreateTime, errno, strerror(errno), iRet);
		Reset(iFD, ERROR_EXTERAL_RECV);
		return -4;
	}

	//ͳ��
	m_stSafegurad.IncreaseUpPacketNum();
	CServerStatistic::Instance()->AddInputPacket(iRet);

	//������������
	if (iRet > CConfigManager::Instance()->GetClientPacketMaxLength() || iRet == 0)
	{
		Reset(iFD, ERROR_EXTERAL_RECV_PACKET_TOO_MORE);
		LOGERROR("In CIOHandler::OnReadFromExternal, ClientPacketMaxLength:%d\n", iRet);
		return -5;
	}

	//�����յ�����ʱ���
	pstExternalClientSocket->m_uiRecvClientTimeStamp = time(NULL);
	pstExternalClientSocket->m_uiRecvPacketNumber++;
	pstExternalClientSocket->m_uiRecvEndOffset += iRet;

	//��β��Ϊ'\0'
	pstExternalClientSocket->m_szRecvBuffer[iRet] = '\0';
	const char* pszCodeBuff = pstExternalClientSocket->m_szRecvBuffer;

	//����ͳ��
	CServerStatistic::Instance()->AddInputCodeNumber(1);

	std::string strPostURI;
	GetRequestURI(pszCodeBuff, pstExternalClientSocket->m_uiRecvEndOffset, strPostURI);

	iRet = -1;
	if (strPostURI.compare("/recharge") == 0)
	{
		//��ͨ��ֵ
		//��ʱֻ������è���ֵ
		iRet = m_apstRechargeProxy[LOGIN_PLATFORM_YMN]->OnRechargeReq(strPostURI, pstExternalClientSocket->m_uiSessionFD, pszCodeBuff, pstExternalClientSocket->m_uiRecvEndOffset);
	}
	else if (strPostURI.compare("/wechatrecharge") == 0 || strPostURI.compare("/userinfo") == 0)
	{
		//΢�Ź��ں�
		iRet = m_apstRechargeProxy[LOGIN_PLATFORM_WECHAT]->OnRechargeReq(strPostURI, pstExternalClientSocket->m_uiSessionFD, pszCodeBuff, pstExternalClientSocket->m_uiRecvEndOffset);
	}
	else if (strPostURI.compare("/health") == 0)
	{
		//��ȡ����״̬
		((CWechatRechargeProxy*)m_apstRechargeProxy[LOGIN_PLATFORM_WECHAT])->GetServiceStatus(pstExternalClientSocket->m_uiSessionFD);

		//��������
		Reset(iFD, 0);

		return 0;
	}
	else if (strPostURI.compare("/schema.json") == 0)
	{
		//��ȡ����ӿ�
		((CWechatRechargeProxy*)m_apstRechargeProxy[LOGIN_PLATFORM_WECHAT])->GetServiceSchema(pstExternalClientSocket->m_uiSessionFD);

		//��������
		Reset(iFD, 0);

		return 0;
	}

	if (iRet)
	{
		Reset(iFD, iRet);
		LOGERROR("Failed to process recharge request, ret %d, FD %d, recv %s\n", iRet, iFD, pszCodeBuff);
		return -6;
	}

	return 0;
}

//������־����
int CIOHandler::LoadLogConfig(const char* pszConfigFile, const char* pszLogName)
{
	char szLogPath[64] = { 0 };
	char szLogName[64] = { 0 };
	int iLogLevel;

	if (!pszConfigFile)
	{
		// ʹ��Ĭ������
		SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
		SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
		iLogLevel = LOG_LEVEL_ANY;
	}
	else
	{
		// ��ȡ�����ļ�
		CSectionConfig stConfigFile;
		int iRet = stConfigFile.OpenFile(pszConfigFile);
		if (iRet == 0)
		{
			stConfigFile.GetItemValue("Param", "LogPath", szLogPath, sizeof(szLogPath), "../log/");
			stConfigFile.GetItemValue("Param", "LogName", szLogName, sizeof(szLogName), pszLogName);
			stConfigFile.GetItemValue("Param", "LogLevel", iLogLevel, LOG_LEVEL_WARNING);
			stConfigFile.CloseFile();
		}
		else
		{
			// ��ȡʧ��, ʹ��Ĭ������
			SAFE_STRCPY(szLogPath, "../log/", sizeof(szLogPath));
			SAFE_STRCPY(szLogName, pszLogName, sizeof(szLogName));
			iLogLevel = LOG_LEVEL_ANY;
		}
	}

	// server log
	TLogConfig stServerLogConfig;
	SAFE_STRCPY(stServerLogConfig.m_szPath, szLogPath, sizeof(stServerLogConfig.m_szPath) - 1);
	SAFE_STRCPY(stServerLogConfig.m_szBaseName, szLogName, sizeof(stServerLogConfig.m_szBaseName) - 1);
	SAFE_STRCPY(stServerLogConfig.m_szExtName, ".log", sizeof(stServerLogConfig.m_szExtName) - 1);
	stServerLogConfig.m_iAddDateSuffixType = EADST_DATE_YMD;
	stServerLogConfig.m_iLogLevel = iLogLevel;

	stServerLogConfig.m_shLogType = ELT_LOG_AND_FFLUSH;
	stServerLogConfig.m_iChkSizeSteps = DEFAULT_CHECK_SIZE_STEPS;
	stServerLogConfig.m_iMaxFileSize = DEFAULT_MAX_FILE_SIZE;
	stServerLogConfig.m_iMaxFileCount = DEFAULT_MAX_FILE_COUNT * 2;

	ServerLogSingleton::Instance()->ReloadLogConfig(stServerLogConfig);

	return 0;
}

//��ȡ�����URI
void CIOHandler::GetRequestURI(const char* pszCodeBuff, unsigned short usCodeLength, std::string& strPostURI)
{
	if (!pszCodeBuff || usCodeLength == 0)
	{
		return;
	}

	bool bIsInURI = false;
	for (unsigned i = 0; i < usCodeLength; ++i)
	{
		//�������ո�
		if (pszCodeBuff[i] == ' ')
		{
			if (bIsInURI)
			{
				//URI�������
				break;
			}
			else
			{
				//���濪ʼ����URI
				bIsInURI = true;
				continue;
			}
		}
		else if (bIsInURI)
		{
			//URI������
			strPostURI.push_back(pszCodeBuff[i]);
		}
	}

	return;
}

//����BUS��Ϣ
void CIOHandler::ProcessBusMsg(const GameProtocolMsg& stMsg)
{
	int iPlatformID = LOGIN_PLATFORM_INVALID;
	unsigned uin = 0;

	switch (stMsg.sthead().uimsgid())
	{
	case MSGID_WORLD_USERRECHARGE_RESPONSE:
	{
		//��ֵ�ķ���
		const World_UserRecharge_Response& stResp = stMsg.stbody().m_stworld_userrecharge_response();
		iPlatformID = stResp.iplatform();
		uin = stResp.uin();
	}
	break;

	case MSGID_WORLD_GETUSERINFO_RESPONSE:
	{
		//��ȡ�����Ϣ�ķ���
		const World_GetUserInfo_Response& stResp = stMsg.stbody().m_stworld_getuserinfo_response();
		iPlatformID = stResp.iplatformid();
		uin = stResp.uin();
	}
	break;

	default:
	{
		LOGERROR("Failed to process bus msg, invalid msg id %u\n", stMsg.sthead().uimsgid());
	}
	break;
	}

	if (iPlatformID <= LOGIN_PLATFORM_INVALID || iPlatformID >= LOGIN_PLATFORM_MAX)
	{
		LOGERROR("Failed to recv recharge response, invalid platform %d\n", iPlatformID);
		return;
	}

	//������
	int iRet = m_apstRechargeProxy[iPlatformID]->OnRechargeResp(stMsg);
	if (iRet)
	{
		LOGERROR("Failed to process recharge response, ret %d, uin %u\n", iRet, uin);
		return;
	}

	return;
}

void CIOHandler::CheckAppCmd()
{
	if (m_iAppCmd == APPCMD_STOP_SERVICE)
	{
		StopService();
	}
	if (m_iAppCmd == APPCMD_RELOAD_CONFIG)
	{
		ReloadConfig();
	}

	m_iAppCmd = 0;
}

void CIOHandler::SetAppCmd(const int iAppCmd)
{
	m_iAppCmd = iAppCmd;
}

void CIOHandler::StopService()
{
	exit(0);
}

void CIOHandler::ReloadConfig()
{
	//������־����
	LoadLogConfig(APP_CONFIG_FILE, "RechargeServer");

	//���ط�������
	int iRet = CConfigManager::Instance()->LoadAllConfig();
	if (iRet < 0)
	{
		LOGERROR("ReloadConfig Reload Failed\n");
		return;
	}

	m_stSafeguradStrategy.Initialize(&m_stSafegurad, APP_CONFIG_FILE);
}

int CIOHandler::Reset(int iFD, int iErrorNo, bool bNotSendOffline /* = false */)
{
	if (iFD < 0)
	{
		return -1;
	}

	m_stSafegurad.DecreaseExternalConnectNum();
	m_stSafegurad.IncreaseCloseNewConnectNum();

	TExternalClientSocket* pstExternalClientSocket = m_pstFDPool->GetExternalSocketByFD(iFD);
	if (!pstExternalClientSocket && iErrorNo!=0)
	{
		//������������ã���ӡ��־

		struct in_addr stInAddr;
		stInAddr.s_addr = pstExternalClientSocket->m_uiSrcIP;

		char szCurDateTime[32] = { 0 };
		CTimeUtility::ConvertUnixTimeToTimeString(CTimeUtility::GetNowTime(), szCurDateTime);

		LOGDEBUG("Reset:ErrorNo=%d,CurDateTime=%s,src=%s,fd=%d,createTime=%u,port=%d,Errno=%d,ErrStr=%s\n",
			iErrorNo, szCurDateTime, inet_ntoa(stInAddr), pstExternalClientSocket->m_iSocketFD,
			pstExternalClientSocket->m_uiCreateTime, pstExternalClientSocket->m_ushSrcPort, errno, strerror(errno));
	}

	if (iFD < (int)FD_SIZE)
	{
		m_pstFDPool->SetFDInactive(iFD);
		m_stEpollWrapper.EpollDelete(iFD);
	}

	return CSocketOperator::Close(iFD);
}

int CIOHandler::OnAccept(unsigned int uiIP)
{
	return true;
}

int CIOHandler::SendToExternalClient(unsigned uiSessionFD, const char* pszCodeBuffer, unsigned short ushCodeLength)
{
	LOGDEBUG("In CIOHandler::SendToExternalClient, SessionFD:%u CodeLength:%d\n", uiSessionFD, ushCodeLength);

	TExternalClientSocket* pstExternalClientSocket = m_pstFDPool->GetExternalSocketBySession(uiSessionFD);
	if (!pstExternalClientSocket)
	{
		LOGERROR("In CIOHandler::SendToExternalClient, pstExternalClientSocket=NULL\n");
		return -3;
	}

	pstExternalClientSocket->m_iSendFlag = ESCF_SENT_PACKET_TO_CLIENT;
	pstExternalClientSocket->m_uiRecvSvrTimeStamp = time(NULL);

	int iRet = CSocketOperator::Send(pstExternalClientSocket->m_iSocketFD, ushCodeLength, pszCodeBuffer);

	if (iRet < 0 || iRet != (int)(ushCodeLength))
	{
		struct in_addr stAddr;
		stAddr.s_addr = pstExternalClientSocket->m_uiSrcIP;

		LOGERROR("Send Err:line=%d,src=%s,fd=%d,createTime=%u,Errno=%d,ErrStr=%s,iRet=%d,ushCodeLength=%d\n", __LINE__,
			inet_ntoa(stAddr), pstExternalClientSocket->m_iSocketFD, pstExternalClientSocket->m_uiCreateTime, errno,
			strerror(errno), iRet, ushCodeLength);
		Reset(pstExternalClientSocket->m_iSocketFD, ERROR_EXTERAL_SEND_CLIENT_16);

		return 0;
	}

	LOGDEBUG("In CIOHandler::SendToExternalClient, Succ Send iLength:%d \n", ushCodeLength);

	CServerStatistic::Instance()->AddOutputPacket(ushCodeLength);
	m_stSafegurad.IncreaseDownPacketNum();

	LOGDEBUG("In CIOHandler::SendToExternalClient, FD:%d Send OK iRet:%d\n", pstExternalClientSocket->m_iSocketFD, iRet);

	return 0;
}
