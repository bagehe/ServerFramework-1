/**
*@file NowTIme.hpp
*@author jasonxiong
*@date 2009-11-23
*@version 1.0
*@brief ��ǰʱ��Ķ���
*
*	Ĭ������²�ʹ�á���ʱ�䡱������GetNowTime��������time(NULL)��ȡʱ��
*	��ʹ�á���ʱ�䡱����ҪӦ�����е���RefreshNowTimeˢ��ʱ�䣬GetNowTime��ȡ������һ��ˢ��ʱ��ʱ��
*/

#ifndef __NOW_TIME_HPP__
#define __NOW_TIME_HPP__

#include "UnixTime.hpp"
#include "TimeValue.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

class CNowTime
{
public:
    CNowTime()
    {
        m_bUsePseudoTime = false;
    }
    ~CNowTime() {}

public:
    //!��ȡ��ǰUnixTimeʱ�䣨�뼶��
    time_t GetNowTime() const;
    //!ʹ�ü�UnixTimeʱ�䣬��ǰʱ��ˢ����Ӧ�õ���RefreshNowTime����
    void EnablePseudoTime()
    {
        m_bUsePseudoTime = true;
    }
    //!��ʹ�ü�UnixTimeʱ�䣬��ǰʱ����ϵͳ��������
    void DisablePseudoTime()
    {
        m_bUsePseudoTime = false;
    }
    //!ˢ�µ�ǰUnixTimeʱ��
    void RefreshNowTime()
    {
        m_stPseudoTime.RefreshTime();
    }


    //!��ȡ��ǰtimevalʱ�䣨΢�뼶��
    timeval GetNowTimeVal() const;
    //!ʹ�ü�timevalʱ�䣬��ǰʱ��ˢ����Ӧ�õ���RefreshNowTimeVal����
    void EnablePseudoTimeVal()
    {
        m_bUsePseudoTime = true;
    }
    //!��ʹ�ü�timevalʱ�䣬��ǰʱ����ϵͳ��������
    void DisablePseudoTimeVal()
    {
        m_bUsePseudoTime = false;
    }
    //!ˢ�µ�ǰ��TimeVal
    void RefreshNowTimeVal()
    {
        m_stPseudoTimeValue.RefreshTime();
    }

private:
    bool m_bUsePseudoTime; //!<�Ƿ�ʹ�ü�ʱ�䣬��Ӧ�����е���RefreshNowTime��ˢ�µ�ǰʱ��
    CUnixTime m_stPseudoTime; //!<����time_t����
    CTimeValue m_stPseudoTimeValue; //!<����timeval����
};

typedef CSingleton<CNowTime> NowTimeSingleton;

}

#endif //__NOW_TIME_HPP__
///:~
