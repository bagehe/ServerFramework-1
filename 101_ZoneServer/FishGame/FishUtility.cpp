
#include <stdint.h>

#include "GameProtocol.hpp"
#include "NowTime.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "FishpondObj.h"

#include "FishUtility.h"

using namespace ServerLib;

//���㸨��������

//����ΨһID
unsigned CFishUtility::uTableUniqID = 0;

//����ΨһID
unsigned CFishUtility::uFishUniqID = 0;

//�ӵ�ΨһID
unsigned CFishUtility::uBulletUniqID = 0;

//���ΨһID
unsigned CFishUtility::uReturnID = 0;

//����ΨһID
unsigned CFishUtility::uCycleUniqID = 0;

//��ȡ����ID
unsigned CFishUtility::GetTableUniqID()
{
	return ++uTableUniqID;
}

//��ȡ��ΨһID
unsigned CFishUtility::GetFishUniqID()
{
	return ++uFishUniqID;
}

//��ȡ�ӵ�ΨһID
unsigned CFishUtility::GetBulletUniqID()
{
	return ++uBulletUniqID;
}

//��ȡ���ΨһID
unsigned CFishUtility::GetReturnUniqID()
{
	return ++uReturnID;
}

//��ȡ����ΨһID
unsigned CFishUtility::GetCycleUniqID()
{
	return ++uCycleUniqID;
}

//��ȡ��ض���
CFishpondObj* CFishUtility::GetFishpondByID(unsigned uTableID)
{
	return GameTypeK32<CFishpondObj>::GetByKey(uTableID);
}

//�ж�ʱ���Ƿ�ͬһ��
bool CFishUtility::IsSameDay(long lTime1, long lTime2)
{
	if (abs(lTime1 - lTime2) >= 24 * 60 * 60 * 1000)
	{
		//�������24Сʱ
		return false;
	}

	struct tm *tm1;
	struct tm *tm2;
	time_t time1 = lTime1 / 1000;
	time_t time2 = lTime2 / 1000;
	tm1 = localtime(&time1);
	tm2 = localtime(&time2);

	return (tm1->tm_mday == tm2->tm_mday);
}
