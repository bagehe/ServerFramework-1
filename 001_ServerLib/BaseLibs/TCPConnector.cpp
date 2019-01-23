/**
*@file TCPConnector.cpp
*@author jasonxiong
*@date 2009-11-19
*@version 1.0
*@brief
*
*
*/

//������ӱ�׼��ͷ�ļ�
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

//�������ServerLibͷ�ļ�
#include "TCPConnector.hpp"
#include "SocketUtility.hpp"
#include "NowTime.hpp"
#include "SharedMemory.hpp"
#include "LogAdapter.hpp"
#include "StringUtility.hpp"
using namespace ServerLib;

CTCPConnector::CTCPConnector()
{
    // Ĭ�����û�����

    m_iUsrBufAllocType = EUBT_ALLOC_BY_TCP_DEFAULT;
    m_iUsrRecvBufSize = MAX_BUFFER_LENGTH;
    m_iUsrSendBufSize = MAX_BUFFER_LENGTH;
    m_abyRecvBuffer = m_szRecvBufferContent;
    m_abyPostBuffer = m_szPostBufferContent;

    m_iSysRecvBufSize = 0;
    m_iSysSendBufSize = 0;
    memset(&m_stConnectCfg, 0, sizeof(m_stConnectCfg));

    m_iReadBegin = m_iReadEnd = 0;
    m_iPostBegin = m_iPostEnd = 0;
    m_iSocketFD = -1;
    m_iStatus = tcs_closed;
    m_iErrorNO = 0;
    m_tLastRecvPacketTime = NowTimeSingleton::Instance()->GetNowTime();

}

CTCPConnector::CTCPConnector(int iUsrRecvBufSize, int iUsrSendBufSize)
{
    __ASSERT_AND_LOG(iUsrRecvBufSize > 0 && iUsrSendBufSize > 0);

    //����û����������õ�����TCPConnector����

    m_iUsrBufAllocType = EUBT_ALLOC_BY_TCP_CONNECTOR;
    m_iUsrRecvBufSize = iUsrRecvBufSize;
    m_iUsrSendBufSize = iUsrSendBufSize;
    m_abyRecvBuffer = new unsigned char[m_iUsrRecvBufSize];
    m_abyPostBuffer = new unsigned char[m_iUsrSendBufSize];
    __ASSERT_AND_LOG(m_abyRecvBuffer && m_abyPostBuffer);
    m_iSysRecvBufSize = 0;
    m_iSysSendBufSize = 0;
    memset(&m_stConnectCfg, 0, sizeof(m_stConnectCfg));

    m_iReadBegin = m_iReadEnd = 0;
    m_iPostBegin = m_iPostEnd = 0;
    m_iSocketFD = -1;
    m_iStatus = tcs_closed;
    m_iErrorNO = 0;
    m_tLastRecvPacketTime = NowTimeSingleton::Instance()->GetNowTime();
}

CTCPConnector::~CTCPConnector()
{
    CloseSocket();

    if(m_iUsrBufAllocType == EUBT_ALLOC_BY_TCP_CONNECTOR)
    {
        FreeUsrBuffer();
    }
}

CTCPConnector* CTCPConnector::CreateBySharedMemory(const char* pszKeyFileName,
        const unsigned char ucKeyPrjID,
        int iUsrRecvBufSize, int iUsrSendBufSize)
{
    if(pszKeyFileName == NULL || iUsrRecvBufSize <= 0 || iUsrSendBufSize <= 0)
    {
        return NULL;
    }

    CSharedMemory stSharedMemory;
    unsigned int uiSharedMemorySize = sizeof(CTCPConnector) + iUsrRecvBufSize +
                                      iUsrSendBufSize;

    int iRet = stSharedMemory.CreateShmSegment(pszKeyFileName, ucKeyPrjID,
               (int)uiSharedMemorySize);

    if(iRet)
    {
        return NULL;
    }

    //�ڹ����ڴ�ĵ�ַ�Ϸ���CodeQueue
    CTCPConnector* pstTCPConnector = (CTCPConnector*)stSharedMemory.GetFreeMemoryAddress();

    if(!pstTCPConnector)
    {
        return NULL;
    }

    pstTCPConnector->m_iUsrRecvBufSize = iUsrRecvBufSize;
    pstTCPConnector->m_iUsrSendBufSize = iUsrSendBufSize;
    pstTCPConnector->m_iUsrBufAllocType = EUBT_ALLOC_BY_SHARED_MEMORY;
    pstTCPConnector->m_abyRecvBuffer = (unsigned char*)stSharedMemory.GetFreeMemoryAddress() + sizeof(CTCPConnector);
    pstTCPConnector->m_abyPostBuffer = (unsigned char*)stSharedMemory.GetFreeMemoryAddress() +
                                       sizeof(CTCPConnector) + iUsrRecvBufSize;

    return pstTCPConnector;
}

int CTCPConnector::Initialize(const char* pszServerIP, unsigned short ushServerPort,
                              const char* pszBindIP, unsigned short ushBindPort)
{
    if(pszServerIP == NULL || ushServerPort <= 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__NULL_POINTER);

        return -1;
    }

    TTCPConnectCfg stConnectCfg;
    memset(&stConnectCfg, 0, sizeof(stConnectCfg));
    stConnectCfg.m_iType = ETCT_CONNECT;
    SAFE_STRCPY(stConnectCfg.m_szServerIP, pszServerIP, sizeof(stConnectCfg.m_szServerIP)-1);
    stConnectCfg.m_ushServerPort = ushServerPort;

    if(pszBindIP != NULL)
    {
        SAFE_STRCPY(stConnectCfg.m_szBindIP, pszBindIP, sizeof(stConnectCfg.m_szBindIP)-1);
        stConnectCfg.m_ushBindPort = ushBindPort;
    }

    stConnectCfg.m_iSysRecvBufSize = DEFAULT_TCP_SYS_RECV_BUFFER_SIZE;
    stConnectCfg.m_iSysSendBufSize = DEFAULT_TCP_SYS_SEND_BUFFER_SIZE;
    stConnectCfg.m_iMaxNotRecvTime = DEFAULT_MAX_NOT_RECV_TIME;
    stConnectCfg.m_cUseNBConnect = 1;
    stConnectCfg.m_iNBConnectTimeOut = DEFAULT_NB_CONNECT_TIMEOUT;

    return Initialize(stConnectCfg);
}

int CTCPConnector::Initialize(int iFD, const char* pszRemoteIP, unsigned short ushRemotePort)
{
    if(pszRemoteIP == NULL || ushRemotePort <= 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__NULL_POINTER);

        return -1;
    }

    TTCPConnectCfg stConnectCfg;
    memset(&stConnectCfg, 0, sizeof(stConnectCfg));
    stConnectCfg.m_iSocketFD = iFD;
    stConnectCfg.m_iType = ETCT_ACCEPTED;
    SAFE_STRCPY(stConnectCfg.m_szServerIP, pszRemoteIP, sizeof(stConnectCfg.m_szServerIP)-1);
    stConnectCfg.m_ushServerPort = ushRemotePort;
    stConnectCfg.m_iSysRecvBufSize = DEFAULT_TCP_SYS_RECV_BUFFER_SIZE;
    stConnectCfg.m_iSysSendBufSize = DEFAULT_TCP_SYS_SEND_BUFFER_SIZE;
    stConnectCfg.m_iMaxNotRecvTime = DEFAULT_MAX_NOT_RECV_TIME;
    stConnectCfg.m_cUseNBConnect = 1;
    stConnectCfg.m_iNBConnectTimeOut = DEFAULT_NB_CONNECT_TIMEOUT;

    return Initialize(stConnectCfg);
}

int CTCPConnector::Initialize(const TTCPConnectCfg& rstTCPConfig)
{
    m_stConnectCfg = rstTCPConfig;

    if(rstTCPConfig.m_iType == ETCT_CONNECT)
    {
        int iRet = CreateConnector();

        if(iRet)
        {
            return -1;
        }
    }
    //�����������ӣ�ֱ�Ӹ�ֵFD
    else if(rstTCPConfig.m_iType == ETCT_ACCEPTED)
    {
        int iRet = CreateAcceptor();

        if(iRet)
        {
            return -2;
        }
    }

    if(m_iSocketFD < 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__INVALID_FD);

        return -3;
    }

    //���������Ѿ��������ˣ���Ҫ��Щ��������
    CSocketUtility::SetNBlock(m_iSocketFD);

    m_iSysRecvBufSize = rstTCPConfig.m_iSysRecvBufSize;
    CSocketUtility::SetSockRecvBufLen(m_iSocketFD, m_iSysRecvBufSize);
    m_iSysSendBufSize = rstTCPConfig.m_iSysSendBufSize;
    CSocketUtility::SetSockSendBufLen(m_iSocketFD, m_iSysSendBufSize);

    m_iReadBegin = m_iReadEnd = 0;
    m_iPostBegin = m_iPostEnd = 0;
    m_iStatus = tcs_connected;

    m_tLastRecvPacketTime = NowTimeSingleton::Instance()->GetNowTime();

    return 0;
}

int CTCPConnector::FreeUsrBuffer()
{
    if(m_abyRecvBuffer)
    {
        delete []m_abyRecvBuffer;
        m_abyRecvBuffer = NULL;
    }

    if(m_abyPostBuffer)
    {
        delete []m_abyPostBuffer;
        m_abyPostBuffer = NULL;
    }

    m_iUsrRecvBufSize = 0;
    m_iUsrSendBufSize = 0;
    m_iReadBegin = m_iReadEnd = 0;
    m_iPostBegin = m_iPostEnd = 0;

    return 0;
}

int CTCPConnector::CloseSocket()
{
    if(m_iSocketFD > 0)
    {
        close(m_iSocketFD);
    }

    m_iSocketFD = -1;
    m_iStatus = tcs_closed;

    return 0;
}

int CTCPConnector::ConnectToServer()
{
    //���ʹ�÷�������ʽ����
    if(m_stConnectCfg.m_cUseNBConnect)
    {
        //�����÷������׽���
        CSocketUtility::SetNBlock(m_iSocketFD);

        //�ڳ�ʼ��ʱ������Ӳ��õ����������ӣ���������ϵ������÷���������
        int iRet = CSocketUtility::Connect(m_iSocketFD, m_stConnectCfg.m_szServerIP, m_stConnectCfg.m_ushServerPort);

        //һ����������ʱ��Żᷢ��
        if(iRet == 0)
        {
            return 0;
        }
        //����������ڴ����У��򷵻ش���
        else if(errno != EINPROGRESS)
        {
            SetErrorNO(EEN_TCP_CONNECTOR__NB_CONNECT_FAILED);

            return -1;
        }

        fd_set rset, wset;
        FD_ZERO(&rset);
        FD_SET(m_iSocketFD, &rset);
        wset = rset;

        if(m_stConnectCfg.m_iNBConnectTimeOut < MIN_NB_CONNECT_TIMEOUT)
        {
            m_stConnectCfg.m_iNBConnectTimeOut = DEFAULT_NB_CONNECT_TIMEOUT;
        }

        struct timeval stTimeout;
        stTimeout.tv_sec = m_stConnectCfg.m_iNBConnectTimeOut / 1000000;
        stTimeout.tv_usec = m_stConnectCfg.m_iNBConnectTimeOut % 1000000;

        iRet = select(m_iSocketFD+1, &rset, &wset, NULL, &stTimeout);

        if(iRet <= 0)
        {
            SetErrorNO(EEN_TCP_CONNECTOR__NB_CONNECT_TIMEOUT);

            return -2;
        }

        if(FD_ISSET(m_iSocketFD, &rset) || FD_ISSET(m_iSocketFD, &wset))
        {
            int iError = 0;
            socklen_t iLen = sizeof(iError);
            iRet = getsockopt(m_iSocketFD, SOL_SOCKET, SO_ERROR, &iError, &iLen);

            if(iRet < 0 || iError != 0)
            {
                SetErrorNO(EEN_TCP_CONNECTOR__NB_CONNECT_REFUSED);

                return -3;
            }
        }
        else
        {
            SetErrorNO(EEN_TCP_CONNECTOR__NB_CONNECT_FAILED);

            return -4;
        }
    }
    else
    {
        //�ڳ�ʼ��ʱ������Ӳ��õ����������ӣ���������ϵ������÷���������
        int iRet = CSocketUtility::Connect(m_iSocketFD, m_stConnectCfg.m_szServerIP, m_stConnectCfg.m_ushServerPort);

        if(iRet)
        {
            SetErrorNO(EEN_TCP_CONNECTOR__CONNECT_FAILED);

            return -3;
        }
    }

    return 0;
}

int CTCPConnector::ReConnect()
{
    if(m_stConnectCfg.m_iType == ETCT_CONNECT)
    {
        int iRet = CreateConnector();

        if(iRet)
        {
            return -1;
        }

        //���������Ѿ��������ˣ���Ҫ��Щ��������
        CSocketUtility::SetNBlock(m_iSocketFD);
        CSocketUtility::SetSockRecvBufLen(m_iSocketFD, m_iSysRecvBufSize);
        CSocketUtility::SetSockSendBufLen(m_iSocketFD, m_iSysSendBufSize);

        m_iReadBegin = m_iReadEnd = 0;
        m_iPostBegin = m_iPostEnd = 0;
        m_iStatus = tcs_connected;
        m_tLastRecvPacketTime = NowTimeSingleton::Instance()->GetNowTime();
    }

    return 0;
}

int CTCPConnector::CreateConnector()
{
    //��ʼ���������ӣ�����
    //(1)����Socket
    //(2)Bind
    //(3)connnect

    int iRet = CreateSocket();

    if(iRet)
    {
        return -1;
    }

    if(m_stConnectCfg.m_szBindIP[0] != '\0')
    {
        iRet = CSocketUtility::BindSocket(m_iSocketFD, m_stConnectCfg.m_szBindIP, m_stConnectCfg.m_ushBindPort);

        if(iRet)
        {
            CloseSocket();
            SetErrorNO(EEN_TCP_CONNECTOR__BIND_SOCKET_FAILED);

            return -2;
        }
    }

    iRet = ConnectToServer();

    if(iRet)
    {
        CloseSocket();

        return -3;
    }

    return 0;
}

int CTCPConnector::CreateAcceptor()
{
    if(m_stConnectCfg.m_iSocketFD < 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__INVALID_FD);

        return -1;
    }

    m_iSocketFD = m_stConnectCfg.m_iSocketFD;

    return 0;
}

int CTCPConnector::CreateSocket()
{
    if(m_iStatus != tcs_closed && m_iSocketFD > 0)
    {
        CloseSocket();
    }

    m_iSocketFD = socket(AF_INET, SOCK_STREAM, 0);

    if(m_iSocketFD < 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__SOCKET_FAILED);

        m_iStatus = tcs_closed;
        m_iSocketFD = -1;

        return -1;
    }

    return 0;
}

int CTCPConnector::CheckActive()
{
    time_t tNowTime = NowTimeSingleton::Instance()->GetNowTime();

    //ֻ���������ӷ�����Ҫ�ж�����״̬������
    if(m_stConnectCfg.m_iType == ETCT_CONNECT)
    {
        if(m_stConnectCfg.m_iMaxNotRecvTime <= 0)
        {
            m_stConnectCfg.m_iMaxNotRecvTime = DEFAULT_MAX_NOT_RECV_TIME;
        }

        if(m_iStatus != tcs_connected || m_iSocketFD < 0 ||
                tNowTime - m_tLastRecvPacketTime >= m_stConnectCfg.m_iMaxNotRecvTime)
        {
            //�����Ӳ����ڻ��߳־�û���յ���Ӧʱ�����½�������
            int iRet = ReConnect();

            if(iRet)
            {
                return -1;
            }
        }
    }

    return 0;
}

int CTCPConnector::RecvData()
{
    int iRecvedBytes = 0;

#ifdef _POSIX_MT_
    pthread_mutex_lock( &m_stMutex );
#endif

    if(m_iSocketFD < 0 || m_iStatus != tcs_connected)
    {
#ifdef _POSIX_MT_
        pthread_mutex_unlock( &m_stMutex );
#endif

        SetErrorNO(EEN_TCP_CONNECTOR__INVALID_FD);

        return ERR_RECV_NOSOCK;
    }

    if(m_iUsrRecvBufSize <= 0)
    {
#ifdef _POSIX_MT_
        pthread_mutex_unlock( &m_stMutex );
#endif

        SetErrorNO(EEN_TCP_CONNECTOR__CONFIG_USR_BUF_FAILED);

        return ERR_RECV_NOBUFF;
    }

    //�������Ѿ���գ���λ
    if( m_iReadEnd == m_iReadBegin )
    {
        m_iReadBegin = 0;
        m_iReadEnd = 0;
    }

    do
    {
        /* ��ʼ��������: ����   ����: 2003-8-14*/
        if( m_iReadEnd == m_iUsrRecvBufSize )
        {
            /************************************************************************/
            /*//���ָʾ���ݿ�ʼ��ָ��(m_iReadBegin)���ڻ���������㣬��ͨ���ƶ�����*/
            /*���е����ݣ������Լ�����������										*/
            /************************************************************************/
            if (m_iReadBegin > 0)
            {
                memmove(&m_abyRecvBuffer[0], &m_abyRecvBuffer[m_iReadBegin], m_iReadEnd - m_iReadBegin);
                m_iReadEnd -= m_iReadBegin;
                m_iReadBegin = 0;
            }
            //�û��Ľ��ջ������Ѿ���
            else
            {
                SetErrorNO(EEN_TCP_CONNECTOR__USR_RECV_BUF_FULL);

#ifdef _POSIX_MT_
                pthread_mutex_unlock( &m_stMutex );
#endif

                return ERR_RECV_NOBUFF;
            }
        }
        /* ��ֹ: ������: ����   ����: 2003-8-14*/

        iRecvedBytes = recv(m_iSocketFD, &m_abyRecvBuffer[m_iReadEnd],
                            m_iUsrRecvBufSize-m_iReadEnd, 0);

        if(iRecvedBytes > 0)
        {
            m_iReadEnd += iRecvedBytes;
            m_tLastRecvPacketTime = NowTimeSingleton::Instance()->GetNowTime();
        }
        //Զ�˹ر�����
        else if( iRecvedBytes == 0 )
        {
            CloseSocket();

            SetErrorNO(EEN_TCP_CONNECTOR__REMOTE_DISCONNECT);

#ifdef _POSIX_MT_
            pthread_mutex_unlock( &m_stMutex );
#endif

            return ERR_RECV_REMOTE;
        }
        //���շ�������
        else if(errno != EAGAIN)
        {
            CloseSocket();

            SetErrorNO(EEN_TCP_CONNECTOR__RECV_ERROR);

#ifdef _POSIX_MT_
            pthread_mutex_unlock( &m_stMutex );
#endif

            return ERR_RECV_FALIED;
        }
        //else
        //{
        //    //// ����������£�û�յ����ݣ�ֱ���˳�
        //    if (1 == m_stConnectCfg.m_cUseNBConnect)
        //    {
        //        break;
        //    }
        //}
    }
    while( iRecvedBytes > 0 );

#ifdef _POSIX_MT_
    pthread_mutex_unlock( &m_stMutex );
#endif

    return 0;
}

int CTCPConnector::GetData(unsigned char*& rpcDataBeg, int& riDataLenth)
{
    if(m_abyRecvBuffer == NULL || m_iUsrRecvBufSize <= 0)
    {
        riDataLenth = 0;
        SetErrorNO(EEN_TCP_CONNECTOR__CONFIG_USR_BUF_FAILED);

        return -1;
    }

    rpcDataBeg = &m_abyRecvBuffer[m_iReadBegin];
    riDataLenth = m_iReadEnd - m_iReadBegin;

    return 0;
}

int CTCPConnector::FlushRecvBuf(int iFlushLength)
{
    if(m_abyRecvBuffer == NULL || m_iUsrRecvBufSize <= 0)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__CONFIG_USR_BUF_FAILED);

        return -1;
    }

    if(iFlushLength > m_iReadEnd - m_iReadBegin || iFlushLength == -1)
    {
        iFlushLength = m_iReadEnd - m_iReadBegin;
    }

    m_iReadBegin += iFlushLength;

    if( m_iReadBegin == m_iReadEnd )
    {
        m_iReadBegin = m_iReadEnd = 0;
    }

    return 0;
}

int CTCPConnector::SendData(const unsigned char* pszCodeBuf, int iCodeLength)
{
    int iBytesSent = 0;
    int iBytesLeft = iCodeLength;
    const unsigned char *pbyTemp = NULL;
    int iTempRet = 0;

    if(!pszCodeBuf)
    {
        SetErrorNO(EEN_TCP_CONNECTOR__NULL_POINTER);

        return ERR_SEND_NOSOCK;
    }

#ifdef _POSIX_MT_
    pthread_mutex_lock( &m_stMutex );
#endif

    /* ��ʼ��������: ����   ����: 2003-8-19*/
    if( m_iSocketFD < 0 || m_iStatus != tcs_connected )
    {
#ifdef _POSIX_MT_
        pthread_mutex_unlock( &m_stMutex );
#endif
        SetErrorNO(EEN_TCP_CONNECTOR__INVALID_FD);

        return ERR_SEND_NOSOCK;
    }
    /* ��ֹ: ������: ����   ����: 2003-8-19*/

    if(m_iUsrSendBufSize <= 0)
    {
#ifdef _POSIX_MT_
        pthread_mutex_unlock( &m_stMutex );
#endif

        SetErrorNO(EEN_TCP_CONNECTOR__CONFIG_USR_BUF_FAILED);

        return ERR_SEND_NOBUFF;
    }

    /* ��ʼ��������: ����   ����: 2003-8-19*/
    //���ȼ���Ƿ�����������
    iBytesLeft = m_iPostEnd - m_iPostBegin;
    pbyTemp = &(m_abyPostBuffer[m_iPostBegin]);
    while( iBytesLeft > 0 )
    {
        iBytesSent = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);

        if( iBytesSent > 0 )
        {
            m_iPostBegin += iBytesSent;
            pbyTemp += iBytesSent;
            iBytesLeft -= iBytesSent;
        }

#ifdef WIN32
        if( iBytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK )
#else
        if( iBytesSent < 0 && errno != EAGAIN )
#endif
        {
            m_iStatus = tcs_error;
            iTempRet = ERR_SEND_FAILED;
            SetErrorNO(EEN_TCP_CONNECTOR__SEND_ERROR);

            break;
        }
        else if( iBytesSent < 0 )
        {
            iTempRet = ERR_SEND_NOBUFF;
            SetErrorNO(EEN_TCP_CONNECTOR__SYS_SEND_BUF_FULL);

            break;
        }
    }

    if( iBytesLeft == 0 )
    {
        //�������ݷ��ͳɹ�����������ͱ����ύ������
        m_iPostBegin = m_iPostEnd = 0;
    }
    else
    {
#ifdef _POSIX_MT_
        pthread_mutex_unlock( &m_stMutex );
#endif
        if(iCodeLength > m_iUsrSendBufSize - iBytesLeft)
        {
            iTempRet = ERR_SEND_NOBUFF;
            SetErrorNO(EEN_TCP_CONNECTOR__USR_SEND_BUF_FULL);
        }
        else
        {
            //���ʣ�೤�Ȳ������ƶ���Mem
            if(iCodeLength < m_iUsrSendBufSize - m_iPostEnd)
            {
                memmove(&m_abyPostBuffer[0], &m_abyPostBuffer[m_iPostBegin], m_iPostEnd - m_iPostBegin);
                m_iPostEnd -= m_iPostBegin;
                m_iPostBegin = 0;
            }

            //�����Ҫ���͵ı�������
            memcpy((void *)&(m_abyPostBuffer[m_iPostEnd]), (const void *)pszCodeBuf, iCodeLength);
            m_iPostEnd += iCodeLength;
            iTempRet = 0;
        }

        return iTempRet;
    }

    //���ͱ����ύ������
    iBytesLeft = iCodeLength;
    pbyTemp = (unsigned char*)pszCodeBuf;
    /* ��ֹ: ������: ����   ����: 2003-8-19*/

    while( iBytesLeft > 0 )
    {
        iBytesSent = send(m_iSocketFD, (const char *)pbyTemp, iBytesLeft, 0);

        if( iBytesSent > 0 )
        {
            pbyTemp += iBytesSent;
            iBytesLeft -= iBytesSent;
        }

#ifdef WIN32
        if( iBytesSent < 0 && WSAGetLastError() != WSAEWOULDBLOCK )
#else
        if( iBytesSent < 0 && errno != EAGAIN )
#endif
        {
            m_iStatus = tcs_error;
            iTempRet = ERR_SEND_FAILED;
            SetErrorNO(EEN_TCP_CONNECTOR__SEND_ERROR);

            break;
        }
        /* ��ʼ��������: ����   ����: 2003-8-19*/
        else if( iBytesSent < 0 )
        {
            if(iBytesSent > m_iUsrSendBufSize - m_iPostEnd)
            {
                iTempRet = ERR_SEND_NOBUFF;
                SetErrorNO(EEN_TCP_CONNECTOR__USR_SEND_BUF_FULL);
            }
            else
            {
                //��δ������ı�������
                memcpy((void *)&(m_abyPostBuffer[m_iPostEnd]), (const void *)pbyTemp, iBytesLeft);
                m_iPostEnd += iBytesLeft;
                iTempRet = 0;
            }

            break;
        }
        /* ��ֹ: ������: ����   ����: 2003-8-19*/
    }

#ifdef _POSIX_MT_
    pthread_mutex_unlock( &m_stMutex );
#endif

    return iTempRet;
}
