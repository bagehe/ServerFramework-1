#ifndef __ZMQ_WRAPPER_HPP__
#define __ZMQ_WRAPPER_HPP__

#include <stdint.h>

#include "zmq.h"
#include "zmq_utils.h"

//����ZMQ�ķ�װ������Ϸ�������У����еķ������ڲ�ͨ�Ŷ�ʹ��ZMQ

enum enZmqSocketType
{
    EN_ZMQ_SOCKET_PAIR = 1,             //ZMQ_PAIR
    EN_ZMQ_SOCKET_PUB = 2,              //ZMQ_PUB
    EN_ZMQ_SOCKET_SUB = 3,              //ZMQ_SUB
    EN_ZMQ_SOCKET_REQ = 4,              //ZMQ_REQ
    EN_ZMQ_SOCKET_REP = 5,              //ZMQ_REP
};

enum enZmqProcType
{
    EN_ZMQ_PROC_INPROC = 1,             //������ͨ��
    EN_ZMQ_PROC_IPC = 2,                //���̼�IPCͨ��
    EN_ZMQ_PROC_TCP = 3,                //TCP����ͨ��
};

enum enZmqServiceType
{
    EN_ZMQ_SERVICE_SERVER = 1,          //�������ˣ�����bind
    EN_ZMQ_SERVICE_CLIENT = 2,          //�ͻ��ˣ�����connect
};

class ZmqBus
{
public:
    ZmqBus();
    ~ZmqBus();

    int ZmqInit(uint64_t ullBusID, const char* pszAddress, enZmqSocketType eSocketType, enZmqProcType eProcType, bool bIsServerEnd);

    int ZmqReinit();

    int ZmqSend(const char* pszBuff, int iMsgLen, int iFlags);

    int ZmqRecv(char* pszBuff, int iMaxLen, int& iMsgLen, int iFlags);

    void ZmqFin();

	uint64_t GetBusID();

private:
    int GetSocketType(enZmqSocketType eSocketType);

    int GetRealSockAddr(enZmqProcType eProcType, const char* pszAddress);

private:
    //zmq context���̰߳�ȫ�ģ������̹߳���һ��
    static void* m_spZmqCtx;

    void* m_pZmqSocket;

    //�󶨵ĵ�ַ
    char m_szSocketAddr[128];

    //ʵ�ʰ󶨵ĵ�ַ��������ProcType�ĵ�ַ
    char m_szRealSockAddr[256];

    //���ӵ�����
    int m_iSocketType;

    //ͨ�ŵĽ���ģ������
    int m_iProcType;

    //�������ӵ����ͣ��Ƿ��������ǿͻ���
    int m_iServiceType;

	//ZMQ Bus��BusID
	uint64_t m_ullServerBusID;
};

#endif
