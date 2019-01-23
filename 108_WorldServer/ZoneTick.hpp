#pragma once

#include "GameProtocol.hpp"
#include "CommDefine.h"

// Zone���߳�ʱʱ�� (10����)
const int UNACTIVE_ZONE_TIMEOUT     = 10 * 60;

// ����� (5��)
const int ZONE_TICK_INTERVAL        = 5;

typedef struct tagZoneStatus
{
	int m_iZoneID;              // ZoneID
	int m_iLastActiveTime;      // �ϴλʱ��
	int m_iOnlineNumber;        // ��������
	unsigned char m_ucActive;   // �Ƿ��Ծ
	unsigned char m_ucState;	// ��æ״̬
}TZoneStatus;

class CZoneTick
{
public:
    int Initialize(bool bResumeMode);

    int OnTick();
    
public:
    void ActiveRefresh(int iZoneID, int iOnlineNumber = -2);

public:
    const TZoneStatus& GetZoneStatus(int iZoneID);
	int GetActiveZoneNum()
	{
		return m_iActiveZoneNum;
	}
	int GetWorldOnlineNum()
	{
		return m_iWorldOnlineNum;
	}

	// ����������ٵ�ZoneID
	int GetLowestZoneID();

public:
    bool IsZoneActive(int iZoneID);
private:
    void OnZoneDead(int iZoneID);

private:
    TZoneStatus m_astZoneStatus[MAX_ZONE_PER_WORLD];
    int m_iLastTickTime;
	int m_iActiveZoneNum;
	int m_iWorldOnlineNum;

    GameProtocolMsg m_stWorldMsg;
};
