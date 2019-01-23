#ifndef __CONFIG_MGR_H__
#define __CONFIG_MGR_H__

#include "ServerBusConfigManager.h"

struct ServerInfoConfig
{
    int iServerID;
    char szServerHostInfo[32];

    ServerInfoConfig()
    {
        memset(this, 0, sizeof(*this));
    };
};

class CConfigManager
{
public:

	static CConfigManager* Instance();
	~CConfigManager();

    int LoadAllConf();
public:
    const ServerInfoConfig* GetServerInfo(int iServerID);

	//��ȡͨ��BUS������
	ServerBusConfigManager& GetBusConfigMgr();

private:
	CConfigManager();

private:
    //��Ϸ�������б�
    int m_iServerInfoNum;
    ServerInfoConfig m_astServerList[32];

	//ͨ��BUS���ù�����
	ServerBusConfigManager m_stBusConfigMgr;
};

#endif // __CONFIG_MGR_H__
