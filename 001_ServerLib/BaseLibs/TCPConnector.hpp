/**
*@file TCPConnector.hpp
*@author jasonxiong
*@date 2009-11-19
*@version 1.0
*@brief TCP�����࣬����TCP���ӣ�����TCP�շ���
*
*	��TCP���ӷ�Ϊ����
*	��1���������ӣ���Ҫ����connetȥ���ӶԷ�
*	��2���������ӣ�ͨ��������Accept��ȡ��FD����������
*/

#ifndef __TCP_CONNECTOR_HPP__
#define __TCP_CONNECTOR_HPP__

#include <time.h>
#include <sys/types.h>

#include "ErrorDef.hpp"
#include "CommonDef.hpp"

namespace ServerLib
{

const int MIN_TCP_USR_RECV_BUFFER_SIZE = 16 * 1024; //!<���õ�TCP�û����ջ���������С��С
const int MIN_TCP_USR_SEND_BUFFER_SIZE = 16 * 1024; //!<���õ�TCP�û����ͻ���������С��С
const int MIN_TCP_SYS_RECV_BUFFER_SIZE = 16 * 1024; //!<���õ�TCPϵͳ���ջ���������С��С
const int DEFAULT_TCP_SYS_RECV_BUFFER_SIZE = 2 * 1024 * 1024; //!<Ĭ�ϵ�TCPϵͳ���ܻ�������С
const int MIN_TCP_SYS_SEND_BUFFER_SIZE = 16 * 1024; //!<���õ�TCPϵͳ���ͻ���������С��С
const int DEFAULT_TCP_SYS_SEND_BUFFER_SIZE = 2 * 1024 * 1024; //!<Ĭ�ϵ�TCPϵͳ���ͻ�������С
const int DEFAULT_MAX_NOT_RECV_TIME = 300; //!<Ĭ�ϵ����δ�հ�ʱ�䣬������ʱ����Ҫ����
const int MIN_NB_CONNECT_TIMEOUT = 1000; //!<��С�ķ��������ӳ�ʱʱ�䣬1����
const int DEFAULT_NB_CONNECT_TIMEOUT = 1000000; //!<Ĭ�ϵķ��������ӳ�ʱʱ�䣬1��

typedef struct tagTCPConnectConfig
{
    int m_iSocketFD; //!<ָ��FD�����ڱ��������׽�������Ч
    int m_iType; //!<TCP�������ͣ�Ŀǰ0Ϊ���������׽��֣�1Ϊ���������׽���
    char m_szServerIP[MAX_IPV4_LENGTH]; //!<Զ��IP
    unsigned short m_ushServerPort; //!<Զ��Port
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<����IP��������Ҫ��m_szBindIP[0]='\0'
    unsigned short m_ushBindPort; //!<���ض˿�
    char m_cUseNBConnect; //!<ʹ�÷��������ӱ�־
    int m_iNBConnectTimeOut; //!<��������ʽ����ʱ��ʱʱ�䣬��λus������m_cUseNBConnect��Ϊ0ʱ��Ч
    int m_iSysRecvBufSize; //!<ϵͳ���ջ����С
    int m_iSysSendBufSize; //!<ϵͳ���ͻ����С
    int m_iMaxNotRecvTime; //!<�δ�հ�ʱ�䣬�������
} TTCPConnectCfg;

/**
*@brief TCP����״̬
*
*
*/
typedef enum eTCPConnStates
{
    tcs_closed = 0, //!<�����ѹر�
    tcs_opened = 1, //!<���Ӵ򿪣�������
    tcs_connected = 2, //!<������
    tcs_error = 3,  //!<���ӳ��ִ���
    tcs_code_mismatch = 4, //!<���ӳ��ֱ�����λ
} ETCPCONNSTATES;

//!�û����������䷽ʽ
typedef enum enmUsrBufAllocType
{
    EUBT_ALLOC_BY_TCP_DEFAULT = 0,  //!<Ĭ�����û�����
    EUBT_ALLOC_BY_TCP_CONNECTOR = 1, //!<��TCPConnector���з���
    EUBT_ALLOC_BY_SHARED_MEMORY = 2, //!<�ɹ����ڴ����
} ENMTCPUSRBUFALLOCTYPE;

typedef enum eRecvErrs
{
    ERR_RECV_NOSOCK = -1,
    ERR_RECV_NOBUFF = -2,
    ERR_RECV_REMOTE = -3,
    ERR_RECV_FALIED = -4
} ERECVERRS;

typedef enum eSendErrs
{
    ERR_SEND_NOSOCK = -1,
    ERR_SEND_NOBUFF = -2,
    ERR_SEND_FAILED = -3,
} ESENDERRS;

/**
*@brief TCP��������
*
*
*/
typedef enum enmTCPConnectorType
{
    ETCT_CONNECT = 0, //!<��������
    ETCT_ACCEPTED = 1, //!<��������
} ENMTCPCONNECTORTYPE;

class CTCPConnector
{
public:
    /**
    * ����Ĭ�����û�����
    */
    CTCPConnector();

    /**
    *��̬�ڴ淽ʽ����CTCPConnector���û�������
    *@param[in] iUsrRecvBufSize �û����ջ�������С
    *@param[in] iUsrSendBufSize �û����ͻ�������С
    *@return 0 success
    */
    CTCPConnector(int iUsrRecvBufSize, int iUsrSendBufSize);
    ~CTCPConnector();

    /**
    *ͨ�������ڴ�������CTempMemoryMng��ע������������CTempMemoryMng�����ʼ��������ı����ڹ����ڴ��ֵ��
    *@param[in] pszKeyFileName �����ڴ�Attach���ļ���
    *@param[in] ucKeyPrjID �����ڴ��ProjectID
    *@param[in] iUsrRecvBufSize �û����ջ�������С
    *@param[in] iUsrSendBufSize �û����ͻ�������С
    *@return 0 success
    */
    static CTCPConnector* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iUsrRecvBufSize, int iUsrSendBufSize);

public:
    /**
    *��������ʱ��ʼ��
    *@param[in] pszServerIP Ҫ���ӵķ�����IP
    *@param[in] ushServerPort Ҫ���ӵķ������˿�
    *@param[in] pszBindIP Ҫ�󶨵ı���IP������ΪNULL����ʾ����
    *@param[in] ushBindPort Ҫ�󶨵ı��ض˿�
    *@return 0 success
    */
    int Initialize(const char* pszServerIP, unsigned short ushServerPort,
                   const char* pszBindIP, unsigned short ushBindPort);

    /**
    *��������ʱ��ʼ��
    *@param[in] iFD accept��ȡ��FD
    *@param[in] pszRemoteIP �������ӵ�Զ��IP
    *@param[in] ushRemotePort �������ӵ�Զ�˶˿�
    *@return 0 success
    */
    int Initialize(int iFD, const char* pszRemoteIP, unsigned short ushRemotePort);

    /**
    *���ӳ�ʼ������ȫ���ã�
    *@param[in] rstTCPConfig TCP��������
    *@return 0 success
    */
    int Initialize(const TTCPConnectCfg& rstTCPConfig);

    //!��������Ƿ�����
    int CheckActive();

    //!���½�������
    int ReConnect();

    //!�ر�����
    int CloseSocket();

    /**
    *�������׽�������������δ����ʱ������RecvData�������������ݵ��û����ջ�����
    *@return 0 success
    */
    int RecvData();

    /**
    *�����׽��ֵ�����������������ݵ�Զ��FD
    *@param[in] pszCodeBuf ���͵�����Buf
    *@param[in] iCodeLength ���͵����ݳ���
    *@return 0 success
    */
    int SendData(const unsigned char* pszCodeBuf, int iCodeLength);

    /**
    *��ȡ����ָ������ݳ���
    *@param[out] rpcDataBeg ��ȡ���ݿ�ʼλ��
    *@param[out] riDataLenth ���յ������ݳ��ȣ�Ϊ0��ʾû������
    *@return 0 success
    */
    int GetData(unsigned char*& rpcDataBeg, int& riDataLenth);

    /**
    *��ս��ջ���������ͷ��ʼ���ָ����С
    *@param[in] iFlushLength ��Ҫ��յĻ�������С��Ϊ-1ʱ��ʾȫ�����
    *@return 0 success
    */
    int FlushRecvBuf(int iFlushLength);

    //!��ȡSocketFD
    int GetSocketFD() const
    {
        return m_iSocketFD;
    }

    //!��ȡ��ǰ����״̬
    int GetStatus() const
    {
        return m_iStatus;
    }
    //���ӱ��ر�
    void SetClosed()
    {
        m_iStatus = tcs_closed;
    }
    //�ж��Ƿ�����
    bool IsConnected() const
    {
        return m_iStatus == tcs_connected;
    }

    //!��ȡϵͳ���ջ�������С
    int GetSysRecvBufSize() const
    {
        return m_iSysRecvBufSize;
    }

    //!��ȡϵͳ���ͻ�������С
    int GetSysSendBufSize() const
    {
        return m_iSysSendBufSize;
    }

    //!��ȡ�û����ջ�������С
    int GetUsrRecvBufSize() const
    {
        return m_iUsrRecvBufSize;
    }

    //!��ȡ�û����ͻ�������С
    int GetUsrSendBufSize() const
    {
        return m_iUsrSendBufSize;
    }

    //!��ȡ����
    const TTCPConnectCfg& GetConnectConfig() const
    {
        return m_stConnectCfg;
    }

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!����Socket
    int CreateSocket();

    //!������������
    int CreateConnector();

    //!���ӵ�������
    int ConnectToServer();

    //!������������
    int CreateAcceptor();

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!�����û�������
    int FreeUsrBuffer();

private:
    int m_iSocketFD; //!<���ӵ�SocketFD
    int m_iStatus; //!<����״̬
    int m_iErrorNO; //!������

#ifdef _POSIX_MT_
    pthread_mutex_t m_stMutex;			//�����Ļ������
#endif

    TTCPConnectCfg m_stConnectCfg; //��������

    int m_iSysRecvBufSize; //!<ϵͳ���ջ����С
    int m_iSysSendBufSize; //!<ϵͳ���ͻ����С

    int m_iUsrBufAllocType; //!<�û����������䷽ʽ
    int m_iUsrRecvBufSize; //!<�û����̽��ջ����С
    int m_iReadBegin; //!<�û����̽��ջ���ͷָ��
    int m_iReadEnd; //!<�û����̽��ջ���ĩβָ��
    unsigned char* m_abyRecvBuffer; //!<�û����ջ�����
    int m_iUsrSendBufSize; //!<�û����̷��ͻ����С
    int m_iPostBegin; //!<�û����̷��ͻ���ͷָ��
    int m_iPostEnd; //!<�û����̷��ͻ���ĩβָ��
    unsigned char* m_abyPostBuffer; //!<�û����ͻ�����

    time_t m_tLastRecvPacketTime; //!<��һ�ν��յ����ݵ�ʱ��

    static const int MAX_BUFFER_LENGTH = 4096000;
    unsigned char m_szRecvBufferContent[MAX_BUFFER_LENGTH];
    unsigned char m_szPostBufferContent[MAX_BUFFER_LENGTH];
};

}

#endif //__TCP_CONNECTOR_HPP__
///:~
