/**
*@file TimeUtility.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief Time��غ�����װ��
*
*
*/

#ifndef __TIME_UTIL_HPP__
#define __TIME_UTIL_HPP__

//������ӱ�׼��ͷ�ļ�
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include "TimeValue.hpp"

const int HOURS_IN_ONE_DAY = 24;

namespace ServerLib
{

typedef struct
{
    char m_cHoursNum;
    char m_acHour[HOURS_IN_ONE_DAY];
} MultiHours;

class CTimeUtility
{
public:
    CTimeUtility() {}
    ~CTimeUtility() {}

public:
    //!��ȡ��ǰʱ��
    static time_t GetNowTime()
    {
        return time(NULL);
    }

	//��ȡ��ǰmsʱ��
	static int64_t GetMSTime();

    //!����tTime��ʾ��ʱ��ת������"%Y-%m-%d %H:%M:%S"��ʾ
	//���bSimpleFmt true, ʱ���ʽΪ "%Y%m%d%H%M%S"
    static int ConvertUnixTimeToTimeString(time_t tTime, char* szTimeString, bool bSimpleFmt = false);
    //!����tTime��ʾ��ʱ��ת������"%Y-%m-%d"��ʾ
    static int ConvertUnixTimeToDateString(time_t tTime, char* szDateString);
    //!����tTime��ʾ��ʱ��ת������"%Y-%m"��ʾ
    static int ConvertUinxTimeToMonthString(time_t tTime, char* szMonthString);
    //!����"%Y-%m-%d %H-%M-%S"��ʾ��ʱ��ת����UnixTime
    static int ConvertTimeStringToUnixTime(char* szTimeString, time_t& rtTime);
    //!����"%Y-%m-%d %H-%M-%S"��ʾ��ʱ��ת����tm��ʾ�Ľṹ
    static int ConvertTimeStringToTmStruct(char* szTimeString, struct tm& rstTM);
    //!����tTime��ʾ��ʱ��ת����ĳһ��֮���ĳһ��
    static int ConvertUnixTimeToDaysAfterYear(time_t tTime, int iYear, unsigned short& rushDays);
    //!tvResult = tvA + tvB
    static int TimeValPlus(const timeval& tvA, const timeval& tvB, timeval& tvResult);
    //!tvResult = tvA - tvB
    static int TimeValMinus(const timeval& tvA, const timeval& tvB, timeval& tvResult);

    // ��ȡtTime��һ�ܵĵڼ���
    // ����ֵ: ��һ(1) - ����(7)
    static int GetWeekDay(time_t tTime);

    // ��ȡ����ָ����ʱ���
    // tTime: ��ǰʱ���
    // iTimeSeconds: ��0�㿪ʼ������, Ĭ��Ϊ0, ��0��
    static time_t GetTodayTime(time_t tTime, int iTimeSeconds = 0);

    // ��ȡ����ָ����ʱ���
    // tTime: ��ǰʱ���
    // iWeekDay: ��һ(1) - ����(7)
    // iDaySeconds: iWeekDay�Ĵ�0�㿪ʼ������
    static time_t GetWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // ���ָ��iHour����һ��ʱ���
    // tTime: ��ǰʱ���
    // �����ǰʱ��С��iHour �򷵻�ǰһ���iHour��Ӧ��time
    // ���򷵻ص��� iHour��Ӧ��time
    static int GetLastNearestHourTime(time_t tTime, int iHour);

    // ���ָ��iHour����һ��ʱ���
    // tTime: ��ǰʱ���
    // �����ǰʱ��С�� iHour �򷵻ص����iHour��Ӧ��time
    // ���򷵻���һ�� iHour��Ӧ��time
    static int GetNextNearestHourTime(time_t tTime, int iHour);

    //���ָ��iHour����һ��ʱ���
    // tTime: ��ǰʱ���
    // �����ǰʱ��С�� iSec �򷵻ص����iSec��Ӧ��time
    // ���򷵻���һ�� iSec��Ӧ��time
    static int GetNextNearestSecTime(time_t tTime, int iSec);

    // ���ָ��iHour���¼���ʱ���
    // tTime: ��ǰʱ���
    // �����ǰʱ��С�� iHour �򷵻ص����iHour��Ӧ��time
    // ���򷵻���һ�� iHour��Ӧ��time
    static int GetNextNearestMutiHourTimes(time_t tTime, const MultiHours* pstMultiHours);

    // ���ָ��ʱ�����һ��ʱ���
    // tTime: ��ǰʱ���
    // iWeekDay: ��һ(1) - ����(7)
    // iDaySeconds: iWeekDay�Ĵ�0�㿪ʼ������
    static int GetLastNearestWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // ���ָ��ʱ�����һ��ʱ���
    // tTime: ��ǰʱ���
    // iWeekDay: ��һ(1) - ����(7)
    // iDaySeconds: iWeekDay�Ĵ�0�㿪ʼ������
    static int GetNextNearestWeekTime(time_t tTime, int iWeekDay, int iDaySeconds);

    // ���������գ�ʱ���룬����
    static int GetTimeOfDay(time_t tTime,
                            int* iYear = NULL,
                            int* iMon = NULL,
                            int* iDay = NULL,
                            int* iHour = NULL,
                            int* iMin = NULL,
                            int* iSec = NULL,
                            int* iWeekDay = NULL);

    /*
     *���tTime > year..sec return 1
     *tTime < year..sec return -1
     *tTime == year..sec return 0
     *
     *ע��iHour[0-23]
     */
    static int CompareDateTime(time_t tTime, int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec);

    /*
    *���tTime > year..sec return 1
    *tTime < year..sec return -1
    *tTime == year..sec return 0
    *
    *ע��iHour[0-23]
    */
    static int CompareTime(time_t tTime, int iHour, int iMin, int iSec);

    /* �Ƿ���ͬһ���� */
    static bool IsInSameMonth(time_t tTime1, time_t tTime2);

    /* �Ƿ���ͬһ�� */
    static bool IsInSameYear(time_t tTime1, time_t tTime2);

    static time_t MakeTime(int iYear, int iMonth, int iDay, int iHour, int iMin, int iSec);

    // �Ƿ���ͬһ��
    static bool IsInSameDay(time_t tTime1, time_t tTime2);

    //�Ƿ���ͬһ��
    static bool IsInSameWeek(time_t tTime1, time_t tTime2);

    //��ȡ��һ��0���ʱ��
    static time_t GetNextDayBeginTime();
public:
    //����tick��ʼ��timevalue
    static CTimeValue m_stTimeValueTick;

    //����tick��ʼ��time
    static time_t m_uiTimeTick;

};
}

#endif //__TIME_UTIL_HPP__
///:~
