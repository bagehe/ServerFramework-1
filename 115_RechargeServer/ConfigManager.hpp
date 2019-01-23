#pragma once

#include "AppDefine.hpp"
#include "ServerBusConfigManager.h"

class CConfigManager
{
public:
	static CConfigManager* Instance();
	~CConfigManager();

public:

	//��������
	int LoadAllConfig();

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

    int GetIdleConnectionTimeout();
    int GetIdleClientTimeout();
    int GetIdleServerTimeout();
    int GetClientPacketMaxFrequencyNum();
	int GetExternalRCVBufferSize();
	int GetExternalSNDBufferSize();
	int GetClientPacketMaxLength();
    const TAddressGroup& GetExternalListenerAddress();

private:
	CConfigManager();

	//���ط�������
	int LoadServerConfig();

private:

    // ���ջ�������С
	int m_iExternalSocketRCVBufferSize;

    // ���ͻ�������С
	int m_iExternalSocketSNDBufferSize;

    //�������б��ĳ�������
	 int m_iClientPacketMaxLength;

    //��鳬ʱ��ʱ��
    int m_iIdleConnectionTimeout; //���ӳ�ʱʱ��
    int m_iIdleClientTimeout; //û���յ��ͻ��˰���ʱʱ��
    int m_iIdleServerTimeout; //û���յ�Svr�ذ��ĳ�ʱʱ��

    //��������Ƶ������
    int m_iClientPacketMaxFrequency;

    //�ⲿ������ַ����
    TAddressGroup m_stClientListenerAddress;

	//BUS���ù�����
	ServerBusConfigManager m_stBusConfigManager;
};
