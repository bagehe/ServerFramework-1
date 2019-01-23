#pragma once

#include "ServerBusConfigManager.h"
#include "BaseConfigManager.hpp"

typedef struct
{
    char m_szHostIP[24];
    unsigned short m_ushPort;
    unsigned short m_ushZoneID;
    char m_szZoneName[64];
} TZoneConf;

typedef struct
{
    unsigned short m_ushLen;

    // ������ͨ�͵���2��IP��Ϣ
    TZoneConf m_astZoneConf[MAX_ZONE_PER_WORLD*2];
} TZoneConfList;

class CConfigManager
{
public:
	static CConfigManager* Instance();
	~CConfigManager();

    int Initialize(bool bResume);

    int LoadZoneConf(const char* pszConfFile);
	const TZoneConfList& GetZoneConfList();

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

	//��ȡ�������ù�����
	BaseConfigManager& GetBaseCfgMgr();

	//��ȡ�������������
	int GetFullWorldNum();

	//��ȡ������æ�������
	int GetBusyWorldNum();

private:
	CConfigManager();

private:

	//�����б���Ϣ
    TZoneConfList m_stZoneConfList;
	
	//������ͨ��BUS����
	ServerBusConfigManager m_stBusConfigMgr;

	//��������������
	BaseConfigManager m_stBaseConfigManager;

	//������֧�ֵ��������
	int m_iFullWorldNum;

	//������æ֧�ֵ��������
	int m_iBusyWorldNum;
};
