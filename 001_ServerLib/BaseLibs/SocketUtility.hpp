/**
*@file SocketUtility.hpp
*@author jasonxiong
*@date 2009-11-05
*@version 1.0
*@brief �����׽�����ز���
*
*
*/

#ifndef __SOCKET_UTIL_HPP__
#define __SOCKET_UTIL_HPP__

//������ӱ�׼��ͷ�ļ�
#include <netinet/in.h>

namespace ServerLib
{
class CSocketUtility
{
private:
    CSocketUtility() {}
    ~CSocketUtility() {}

public:
    /**
    *�����׽���Ϊ������
    *@param[in] iSocketFD ��Ҫ���õ��׽���
    *@return 0 success
    */
    static int SetNBlock(int iSocketFD);

    /**
    *�����׽���Nagle�㷨�ر�
    *@param[in] iSocketFD ��Ҫ���õ��׽���
    *@return 0 success
    */
    static int SetNagleOff(int iSocketFD);

    /**
    *���õ�ַ����
    *@param[in] iSocketFD ��Ҫ���õ�ַ���õ�FD
    *@return 0 success
    */
    static int SetReuseAddr(int iSocketFD);

    /**
    *����TCP����
    *@param[in] iSocketFD ��Ҫ���ñ����FD
    *@return 0 success
    */
    static int SetKeepalive(int iSocketFD);

    /**
    *��ֹ�ӳٹر�
    *@param[in] iSocketFD ��Ҫ���ñ����FD
    *@return 0 success
    */
    static int SetLingerOff(int iSocketFD);

    /**
    *��Socket��IP
    *@param[in] iSocketFD
    *@param[in] pszBindIP Ҫ�󶨵�IP��ַ
    *@param[in] ushBindPort Ҫ�󶨵Ķ˿ڵ�ַ
    *@return 0 success
    */
    static int BindSocket(int iSocketFD, const char* pszBindIP, unsigned short ushBindPort);

    /**
    *���ӵ�Զ��IP
    *@param[in] iSocketFD
    *@param[in] pszBindIP Ҫ���ӵ�IP��ַ
    *@param[in] ushBindPort Ҫ���ӵĶ˿ڵ�ַ
    *@return 0 success
    */
    static int Connect(int iSocketFD, const char* pszServerIP, unsigned short ushServerPort);

    /**
    *�����׽�����ռ��ϵͳ���ջ�������С
    *@param[in] iSocketFD ��Ҫ���õ��׽���
    *@param[in] riRecvBufLen Ҫ���õ�ϵͳ���ջ�������С
    *@param[out] riRecvBufLen ���ú��ϵͳ���ջ�������С
    *@return 0 success
    */
    static int SetSockRecvBufLen(int iSocketFD, int& riRecvBufLen);

    /**
    *�����׽�����ռ��ϵͳ���ͻ�������С
    *@param[in] iSocketFD ��Ҫ���õ��׽���
    *@param[in] riSendBufLen Ҫ���õ�ϵͳ���ͻ�������С
    *@param[out] riSendBufLen ���ú��ϵͳ���ͻ�������С
    *@return 0 success
    */
    static int SetSockSendBufLen(int iSocketFD, int& riSendBufLen);

    /**
    *����sockaddr_in��ʾ��IP��ַת��Ϊ�ַ���
    *@param[in] pstSockAddr Ҫת���ĵ�ַ
    *@param[out] szAddr ���ַ�����ʾ��IPv4:port��ַ
    *@return 0 success
    */
    static int SockAddrToString(sockaddr_in *pstSockAddr, char *szAddr);

    /**
    *����int32��ʾ��IP��ַת��Ϊ�ַ���
    *@param[in] iIPAddr int32��ʾ��IP��ַ
    *@param[in] szAddr ���ʮ���Ʊ�ʾ��IP��ַ
    *@return 0 success
    */
    static int IPInt32ToString(int iIPAddr, char* szAddr);
};
}

#endif //__SOCKET_UTIL_HPP__
///:~
