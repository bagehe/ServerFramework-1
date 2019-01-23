#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "CommDefine.h"
#include "StringUtility.hpp"

using namespace ServerLib;

//������ͨ��BUS���ù�����

//ͨ��BUSID
// Ĭ��instance��1��ʼ
// ZoneIDΪ0, ��ʾ��������world�ķ�����
struct SERVERBUSID
{
	unsigned short usWorldID;
	unsigned short usServerID;
	unsigned short usInstanceID;
	unsigned short usZoneID;

	SERVERBUSID()
	{
		Reset();
	}

	void Reset()
	{
		usWorldID = 0;
		usServerID = 0;
		usInstanceID = 1;
		usZoneID = 0;
	}
};

// ���ɵ�ServerID: world:16.zone:16.function:16.instance:16
// Ĭ��instance��1��ʼ
// ZoneIDΪ0, ��ʾ��������world�ķ�����
inline uint64_t GenerateBusID(short iWorldID, EGameServerID enServerID, short iInstance = 1, short iZoneID = 0)
{
	uint64_t ullBusID = ((uint64_t)iWorldID) << 48;
	ullBusID += ((uint64_t)enServerID) << 32;
	ullBusID += ((uint64_t)iInstance) << 16;
	ullBusID += iZoneID;

	return ullBusID;
}

// ���ɵ�ServerID: world:16.zone:16.function:16.instance:16
// Ĭ��instance��1��ʼ
// ZoneIDΪ0, ��ʾ��������world�ķ�����
inline std::string GenerateBusString(short iWorldID, EGameServerID enServerID, short iInstance = 1, short iZoneID = 0)
{
	char szServerBusID[SERVER_BUSID_LEN] = { 0 };
	SAFE_SPRINTF(szServerBusID, sizeof(szServerBusID)-1, "%hu.%hu.%hu.%hu", iWorldID, enServerID, iInstance, iZoneID);

	return std::string(szServerBusID);
}

//����BusString��ȡBusID
inline uint64_t GetBusID(const std::string& strBusString)
{
	//����ͨ��BusID
	unsigned short usWorldID = 0;
	unsigned short usServerID = 0;
	unsigned short usInstanceID = 0;
	unsigned short usZoneID = 0;
	sscanf(strBusString.c_str(), "%hu.%hu.%hu.%hu", &usWorldID, &usServerID, &usInstanceID, &usZoneID);

	return GenerateBusID(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);
}

//����BusID��ȡBusString
inline std::string GetBusString(uint64_t ullBusID)
{
	//����ͨ��BusID
	unsigned short usWorldID = ullBusID>>48;
	unsigned short usServerID = ullBusID >> 32;
	unsigned short usInstanceID = ullBusID >> 16;
	unsigned short usZoneID = ullBusID;

	return GenerateBusString(usWorldID, (EGameServerID)usServerID, usInstanceID, usZoneID);
}

//����BusID��ȡBUSID�ṹ��
inline void GetBusStructID(uint64_t ullBusID, SERVERBUSID& stBusID)
{
	//����ͨ��BusID
	stBusID.usWorldID = ullBusID >> 48;
	stBusID.usServerID = ullBusID >> 32;
	stBusID.usInstanceID = ullBusID >> 16;
	stBusID.usZoneID = ullBusID;

	return;
}

//������ͨ��BUS����
//�� conf/GameServer.tcm �ļ��ж�ȡ������ͨ��BUS������
struct ServerBusConfig
{
	bool bIsServerEnd;							//ZMQ Server�˻���Client��
	uint64_t ullServerBusID;					//ZMQ ͨ��BUSID
	char szBusAddr[MAX_SERVER_BUS_ADDR_LEN];    //ZMQͨ��ͨ����ʵ�ʵ�ַ����ʽΪ ip:port

	ServerBusConfig()
	{
		Reset();
	}

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

class ServerBusConfigManager
{
public:
	ServerBusConfigManager();
	~ServerBusConfigManager();

	void Reset();

	//����ͨ��BUS������
	int LoadServerBusConfig();

	//��ȡͨ��Bus����,��ʽΪ XX.XX.XX.XX
	//ServerBusID: world:16.zone:16.function:16.instance:16
	const ServerBusConfig* GetServerBus(const char* szServerBusID);

	//��ȡ����ͨ��BUS������
	const std::vector<ServerBusConfig>& GetAllServerBus();

private:
	//����ͨ��BUS����
	std::vector<ServerBusConfig> m_vServerBusConfig;
};