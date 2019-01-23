#pragma once

#include "EpollWrapper.hpp"
#include "ServerListener.hpp"
#include "FDPool.hpp"
#include "ConfigManager.hpp"
#include "OverloadSafeguard.hpp"
#include "RechargeProxy.h"
#include "YmnRechargeProxy.h"
#include "WechatRechargeProxy.h"

enum ENU_RESETERROR
{
    TIMEOUT_SVR_NOT_RSP = 1,                   // �������ӳ�ʱ����Server��δ��������ӻع���(closed by not having Server rsp)
    TIMEOUT_IDLE_CLIENT = 2,                   // ����client��ʱ(closed by not recving message too long)
    TIMEOUT_IDLE_SVR    = 3,                   // ����server��ʱ(closed by Server not rsp message too long)
    ERROR_EXTERAL_RECV_REQ_OVERLOAD = 8,       // OnReadFromExternal, RecvReqLimit Overload, Close Socket
    ERROR_EXTERAL_ACCEPT_RECV_OVERLOAD = 9,    // OnReadFromExternal, AcceptReceive Overload, Close Socket
    ERROR_EXTERAL_RECV = 10,                   // ����ʧ�ܻ�����ҹر�
    ERROR_EXTERAL_RECV_PACKET_TOO_MORE = 11,   // ����������������
    ERROR_EXTERAL_SEND_CLIENT_16 = 34,         // SendToExternalClient, Error Send
};

class CIOHandler
{
public:
	static CIOHandler* Instance();
	~CIOHandler();

    //��ʼ�����ü��ϡ�
    int Initialize(bool bNagelOff);

    //��������IO�����������汾ֻ֧��Epoll
    int CreateEpoll();

    //�������м�����
    int CreateAllListener();

    //����һö������
    int CreateOneListener(unsigned int uiIP, unsigned short ushPort, unsigned int uiSendBufferSize);

    CServerListener* GetListenerByFD(int iListeningSocket);
    int ReCreateOneListener(CServerListener *pstLotusListener);

    //�������IO
    int CheckIO();

	//���ͨ��BUS
	int CheckBus();

    //��鳬ʱ���
    int CheckTimeOut();
    int CheckOffline(); //������

    int SendToExternalClient(unsigned uiSessionFD, const char* pszCodeBuffer, unsigned short ushCodeLength);

    int PrintStat();

public:
    //��Ӧ����IO������
    static int OnError(int iFD);

    //��Ӧ����IO��
    static int OnRead(int iFD);

    //��Ӧ����IOд
    static int OnWrite(int iFD);

    int AddFD(int iFD);

    //��������
    int Reset(int iFD, int iErrorNo, bool bNotSendOffline = false);

    //����Ӧ�����ֹͣ������������
    static void SetAppCmd(const int iAppCmd);

    //���Ӧ�����ֹͣ������������
    void CheckAppCmd();

    //ֹͣ����
    void StopService();

    //��������
    void ReloadConfig();

private:
    //���캯��
	CIOHandler();

    //������������
    int Accept(int iListeningFD);

    //������������ʱ��
    int OnAccept(unsigned int uiIP);

    //��Ӧ����IO���������ⲿ����
    int OnReadFromExternal(int iFD);

	//������־����
	int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);

	//��ȡ�����URI
	void GetRequestURI(const char* pszCodeBuff, unsigned short usCodeLength, std::string& strPostURI);

	//����BUS��Ϣ
	void ProcessBusMsg(const GameProtocolMsg& stMsg);

private:

    //FD�ؾ��
    CFDPool* m_pstFDPool;

    //����IO�����������汾ֻ֧��Epollʵ��
    CEpollWrapper m_stEpollWrapper;

    //������ʵ��
    unsigned int m_uiListenerNumber;
    CServerListener m_astLotusListener[MAX_LISTENER_NUMBER];

    //Ӧ������
    static int m_iAppCmd;

    time_t m_tLastCheckTimeout; //��һ�μ�鳬ʱ��ʱ��

    //���ر���
    time_t m_tLastSafeguradTimeout;
    COverloadSafeguard m_stSafegurad; //���ر�����
    COverloadSafeguardStrategy m_stSafeguradStrategy; //���ر���������

    bool m_bNagelOff; // �շ��׽����Ƿ�ر�nagel�㷨

	//��������
	int m_iRecvLen;
	char m_szRecvBuff[DEFAULT_RECVBUFFER_SIZE];

	//ƽ̨��ֵ����
	CRechargeProxy* m_apstRechargeProxy[LOGIN_PLATFORM_MAX];
};
