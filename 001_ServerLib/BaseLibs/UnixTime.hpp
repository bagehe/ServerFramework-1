/**
*@file UnixTime.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief ��time_t���з�װ�������
*
*	ʹ��RefreshTime��ˢ��ʱ��
*	���ø���GetXXX��������ȡ��Ҫ��ʱ���ʽ
*/

#ifndef __UNIX_TIME_HPP__
#define __UNIX_TIME_HPP__

#include <time.h>

#include "SingletonTemplate.hpp"

namespace ServerLib
{

class CUnixTime
{
public:
    CUnixTime();
    CUnixTime(time_t tTime);
    ~CUnixTime();

public:
    //!����ǰ���е�ʱ����"%Y-%m-%d %H-%M-%S"��ʾ
    int GetTimeString(char* szTimeString);
    //!����ǰ���е�ʱ����"%Y-%m-%d"��ʾ
    int GetDateString(char* szDateString);
    //!����ǰ���е�ʱ����"%Y-%m"��ʾ
    int GetMonthString(char* szMonthString);
    //!����ǰ���е�ʱ����ĳһ��֮���ĳһ��
    int GetDaysAfterYear(int iYear, unsigned short& rushDays);
    //!ͨ����ǰ���е�ʱ���ȡ���
    int GetYear(int& iYear);
    //!ֱ�ӷ�����ݣ�ʧ�ܷ���-1
    int GetYear();
    //!ͨ����ǰ���е�ʱ���ȡ�·�
    int GetMonth(int& iMonth);
    //!ֱ�ӷ����·ݣ�ʧ�ܷ���-1
    int GetMonth();
    //!ͨ����ǰ���е�ʱ���ȡ����
    int GetDate(int& iDate);
    //!ֱ�ӷ������ڣ�ʧ�ܷ���-1
    int GetDate();
    //!ͨ����ǰ���е�ʱ���ȡСʱ
    int GetHour(int& riHour);
    //!ֱ�ӷ���Сʱ��ʧ�ܷ���-1
    int GetHour();
    //!ͨ����ǰ���е�ʱ���ȡ����
    int GetMinute(int& riMinute);
    //!ֱ�ӷ��ط��ӣ�ʧ�ܷ���-1
    int GetMinute();

    void SetTime(time_t tTime)
    {
        m_tTime = tTime;
    }
    time_t GetTime() const
    {
        return m_tTime;
    }

    //!ˢ��Unixʱ��Ϊ��ǰʱ��
    void RefreshTime();

private:
    time_t m_tTime; //!<UnixTime����Ҫ����ʱ���ʾ
};

}

#endif //__UNIX_TIME_HPP__
///:~
