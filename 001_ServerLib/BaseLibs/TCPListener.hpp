/**
*@file TCPListener.hpp
*@author jasonxiong
*@date 2009-11-19
*@version 1.0
*@brief TCP���Ӽ�����
*
*
*/

#ifndef __TCP_LISTENER_HPP__
#define __TCP_LISTENER_HPP__

#include "ErrorDef.hpp"
#include "CommonDef.hpp"

namespace ServerLib
{

typedef struct tagTCPListenerConfig
{
    char m_szBindIP[MAX_IPV4_LENGTH]; //!<����IP
    unsigned short m_ushBindPort; //!<���ض˿�
    int m_iBacklogFDNum; //!<�ڼ��������л��۵�FD����
} TTCPListenerCfg;

const int MIN_BACKLOG_FD_NUMBER = 1; //!<�ڼ���ʱ������۵�FD��С��Ŀ
const int DEFAULT_BACKLOG_FD_NUMBER = 10; //!<Ĭ�ϼ���ʱ������۵�FD��Ŀ
const int MAX_BACKLOG_FD_NUMBER = 1024; //!<�ڼ���ʱ������۵�FD�����Ŀ

typedef enum eTCPListenerStates
{
    tls_closed = 0, //!<�ѹر�
    tls_opened = 1, //!<�򿪼���
    tls_error = 2,  //!<���ִ���
} ETCPLISTENSTATES;

class CTCPListener
{
public:
    CTCPListener();
    ~CTCPListener();

public:
    /**
    *�򵥳�ʼ��
    *@param[in] pszBindIP ����Ҫ�󶨵�IP�����Դ�NULL
    *@param[in] ushBindPort ����Ҫ�󶨵Ķ˿�
    *@return 0 success
    */
    int Initialize(const char* pszBindIP, const unsigned short ushBindPort);

    //!��ʼ����������
    int Initialize(const TTCPListenerCfg& rstListenerConfig);

    //!�ر�����
    int CloseSocket();

    //!��ȡ�����׽���
    int GetSocketFD() const
    {
        return m_iSocketFD;
    }

    /**
    *����һ������
    *@param[out] riAcceptFD ���ܵ�����FD
    *@param[out] ruiIP ���ܵ�����IP
    *@param[out] rushPort ���ܵ����Ӷ˿�
    *@return 0 success
    */
    int Accept(int& riAcceptFD, unsigned int& ruiIP, unsigned short& rushPort);

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

private:
    int m_iSocketFD;
    int m_iStatus; //!<����״̬
    TTCPListenerCfg m_stTCPListenConfig;
    int m_iErrorNO; //!������

};

}

#endif //__TCP_LISTENER_HPP__
///:~
