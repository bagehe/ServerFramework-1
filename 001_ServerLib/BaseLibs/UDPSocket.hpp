/**
*@file UDPSocket.hpp
*@author jasonxiong
*@date 2010-01-26
*@version 1.0
*@brief ʵ��UDP�׽���
*
*
*/

#ifndef __UDP_SOCKET_HPP__
#define __UDP_SOCKET_HPP__

#include <time.h>
#include <sys/types.h>

#include "CommonDef.hpp"

namespace ServerLib
{

typedef struct tagUDPSocketConfig
{
    char m_szServerIP[MAX_IPV4_LENGTH]; //!<Զ��IP
    unsigned short m_ushServerPort; //!<Զ��Port
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<����IP��������Ҫ��m_szBindIP[0]='\0'
    unsigned short m_ushBindPort; //!<���ض˿�
    int m_iSysRecvBufSize; //!<ϵͳ���ջ����С
    int m_iSysSendBufSize; //!<ϵͳ���ͻ����С
} TUDPSocketConfig;

//!�û����������䷽ʽ
typedef enum enmUDPRecvBufAllocType
{
    EURT_ALLOC_BY_UDP_DEFAULT = 0, //!<Ĭ�����û�����
    EURT_ALLOC_BY_UDP_SOCKET = 1, //!<��CUDPSocket���з���
    EURT_ALLOC_BY_SHARED_MEMORY = 2, //!<�ɹ����ڴ����
} ENMUDPUSRBUFALLOCTYPE;

/**
*@brief UDP����״̬
*
*
*/
typedef enum enmUDPSocketStates
{
    EUSS_CLOSED = 0, //!<�����ѹر�
    EUSS_OPENED = 1, //!<���Ӵ򿪣�������
    EUSS_ERROR = 2,  //!<���ӳ��ִ���
} ENMUDPSOKCETSTATES;

const int MIN_UDP_SYS_RECV_BUFFER_SIZE = 16 * 1024; //!<���õ�UDPϵͳ���ջ���������С��С
const int DEFAULT_UDP_SYS_RECV_BUFFER_SIZE = 2 * 1024 * 1024; //!<Ĭ�ϵ�UDPϵͳ���ܻ�������С
const int MIN_UDP_SYS_SEND_BUFFER_SIZE = 16 * 1024; //!<���õ�UDPϵͳ���ͻ���������С��С
const int DEFAULT_UDP_SYS_SEND_BUFFER_SIZE = 2 * 1024 * 1024; //!<Ĭ�ϵ�UDPϵͳ���ͻ�������С

class CUDPSocket
{
public:
    ~CUDPSocket();

public:
    /**
    * ����Ĭ���������ݻ�����
    */
    CUDPSocket();

    /**
    *��̬�ڴ淽ʽ����CUDPSocket���û�������
    *@param[in] iUsrRecvBufSize �û����ջ�������С
    *@return 0 success
    */
    CUDPSocket(int iUsrRecvBufSize);

    /**
    *ͨ�������ڴ�������CUDPSocket��ע������������CUDPSocket�����ʼ��������ı����ڹ����ڴ��ֵ��
    *@param[in] pszKeyFileName �����ڴ�Attach���ļ���
    *@param[in] ucKeyPrjID �����ڴ��ProjectID
    *@param[in] iUsrRecvBufSize �û����ջ�������С
    *@return 0 success
    */
    static CUDPSocket* CreateBySharedMemory(const char* pszKeyFileName,
                                            const unsigned char ucKeyPrjID, int iUsrRecvBufSize);

public:
    /**
    *��ʼ��UDP�׽���
    *@param[in] pszServerIP Ҫ���ӵķ�����IP
    *@param[in] ushServerPort Ҫ���ӵķ������˿�
    *@param[in] pszBindIP Ҫ�󶨵ı���IP������ΪNULL����ʾ����
    *@param[in] ushBindPort Ҫ�󶨵ı��ض˿�
    *@return 0 success
    */
    int Initialize(const char* pszServerIP, unsigned short ushServerPort,
                   const char* pszBindIP, unsigned short ushBindPort);

    /**
    *��ʼ��UDP�׽��֣���ȫ���ã�
    *@param[in] rstUDPConfig UDP��������
    *@return 0 success
    */
    int Initialize(const TUDPSocketConfig& rstUDPConfig);

    //!�ر��׽���
    int CloseSocket();

    /**
    *�������׽�������������δ����ʱ������RecvData�������������ݵ��û����ջ�����
    *@param[out] pszSrcIP ���ݵķ��ͷ�IP
    *@param[out] pushSrcPort ���ݵķ��ͷ�Port
    *@return 0 success
    */
    int RecvData(char* pszSrcIP = NULL, unsigned short* pushSrcPort = NULL);

    /**
    *��֮ǰ��ʼ��ʱ��ServerIP��������
    *@param[in] pszCodeBuf ���͵�����Buf
    *@param[in] iCodeLength ���͵����ݳ���
    *@return 0 success
    */
    int SendData(const unsigned char* pszCodeBuf, int iCodeLength);

    /**
    *ָ��Ŀ�ĵص�����������������ݵ�Զ��FD
    *@param[in] pszCodeBuf ���͵�����Buf
    *@param[in] iCodeLength ���͵����ݳ���
    *@param[in] pszDstIP Ŀ��IP
    *@param[in] ushDstPort Ŀ�Ķ˿�
    *@return 0 success
    */
    int SendDataTo(const unsigned char* pszCodeBuf, int iCodeLength, const char* pszDstIP, unsigned short ushDstPort);

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

    //!��ȡ����
    const TUDPSocketConfig& GetConnectConfig() const
    {
        return m_stUDPSocketCfg;
    }

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!����Socket
    int CreateSocket();

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

    TUDPSocketConfig m_stUDPSocketCfg; //��������

    int m_iSysRecvBufSize; //!<ϵͳ���ջ����С
    int m_iSysSendBufSize; //!<ϵͳ���ͻ����С

    int m_iUsrBufAllocType; //!<�û����������䷽ʽ
    int m_iUsrRecvBufSize; //!<�û����̽��ջ����С
    int m_iReadBegin; //!<�û����̽��ջ���ͷָ��
    int m_iReadEnd; //!<�û����̽��ջ���ĩβָ��
    unsigned char* m_abyRecvBuffer; //!<�û����ջ�����

    static const int MAX_BUFFER_LENGTH = 40960;
    unsigned char m_szBufferContent[MAX_BUFFER_LENGTH];
};

}


#endif //__UDP_SOCKET_HPP__
///:~
