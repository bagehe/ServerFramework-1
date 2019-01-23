#pragma once

#include "ZmqBus.hpp"
#include "CodeQueueManager.hpp"
#include "CommDefine.h"
#include "ServerBusConfigManager.h"

using namespace ServerLib;

const int MAX_SERVER_BUS_NUM = 8;

class CServerBusManager
{
public:
	static CServerBusManager* Instance();
	~CServerBusManager();

public:
	//��ʼ��ͨ��BUS
	int Init(const char* pszServerName, ServerBusConfigManager& stBusConfigMgr);

	//������Ϣ
	int SendOneMsg(const char* pszMsg, int iMsgLength, const SERVERBUSID& stToBusID);

	//������Ϣ
	int RecvOneMsg(char* pszMsg, int iMaxOutMsgLen, int& riMsgLength, SERVERBUSID& stFromBusID);

private:
	CServerBusManager();

	//��ȡZMQ BUS
	ZmqBus* GetZmqBus(uint64_t ullBusID);

private:
	//��ѯ����Ϣ����ǰ����Ϣ��BusIndex
	int m_iRecvBusIndex;

	//��Lotus��CodeQueueͨ��
	CCodeQueueManager m_stCodeQueueMgr;

	//������ZMQBUSͨ��
	int m_iBusNum;
	ZmqBus m_astServerBus[MAX_SERVER_BUS_NUM];
};
