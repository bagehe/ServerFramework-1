#pragma once

#include "Kernel/HandlerFactory.hpp"
#include "LogAdapter.hpp"
#include "GMPrivConfigManager.hpp"
#include "BaseConfigManager.hpp"
#include "ServerBusConfigManager.h"

using namespace ServerLib;

// ��Ϣ���CDʱ�� 2000ms
const int MAX_MSG_INTERVAL_TIME = 2000;

// Ĭ�ϲ�����CD
const int DEFAULT_MSG_INTERVAL = 0;

//ģ�鿪��
enum MODULE_ID
{
	MODULE_FISH_GAME = 0,		//����ģ�鿪��
	MODULE_MAX_ID,
};

class CConfigManager
{
public:
	static CConfigManager* Instance();
	~CConfigManager();

    int Initialize(bool bResumeMode);

    //GMȨ������
	CGMPrivConfigManager& GetGMPrivConfigManager();

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

	//��ȡ�������ù�����
	BaseConfigManager& GetBaseCfgMgr();
	
	int GetMsgInterval(unsigned int uiMsgID);

	//ģ���Ƿ�ر�
	bool IsModuleSwitchOn(MODULE_ID eModuleID);

	//�Ƿ���SessionKey
	bool IsSSKeyCheckEnabled();

	// �Ƿ���Player��־
	bool IsPlayerLogEnabled();

private:

	//���ط���������
	int LoadZoneConfig();

	int LoadMsgInterval();
	int LoadMsgDiableList();

    int PrintConfigMemoryStatics();

private:
	CConfigManager();

private:

    //GM����Ȩ�����ù�����
    CGMPrivConfigManager m_stGMPrivConfigManager;

	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigManager;

	//Excel���ù�����
	BaseConfigManager m_stBaseConfigManager;

	//����ģ�鿪��
	int m_aiModuleSwitch[MODULE_MAX_ID];

	//�Ƿ���SessionKey
	int m_iCheckSSKeyEnabled;
	
	//�Ƿ��ӡPlayer��־
	int m_iIsEnablePlayerLog;
};
