/**
*@file TimeValue.hpp
*@author jasonxiong
*@date 2009-11-24
*@version 1.0
*@brief ���ڷ������timevalֵ����
*
*	ֱ��ʹ����ѧ���������ʱ����м���
*/

#ifndef __TIME_VALUE_HPP__
#define __TIME_VALUE_HPP__

#include <time.h>
#include <sys/types.h>

namespace ServerLib
{

	class CTimeValue
	{
	public:
		CTimeValue();
		CTimeValue(timeval tvTimeVal);
		CTimeValue(int iSec, int iUsec);
		~CTimeValue();

	public:
		//!�������ص����������timeval�ļ���
		CTimeValue& operator=(const CTimeValue& rstTime);
		CTimeValue operator+(const CTimeValue& rstTime);
		CTimeValue operator-(const CTimeValue& rstTime);
		int operator>(const CTimeValue& rstTime);
		int operator<(const CTimeValue& rstTime);
		int operator>=(const CTimeValue& rstTime);
		int operator<=(const CTimeValue& rstTime);
		int operator==(const CTimeValue& rstTime);
		int operator!=(const CTimeValue& rstTime);

		void SetTimeValue(timeval stTimeValue)
		{
			m_stTimeval = stTimeValue;
		}
		timeval GetTimeValue() const
		{
			return m_stTimeval;
		}

		int64_t GetMilliSec() const
		{
			return (int64_t)m_stTimeval.tv_sec * 1000 + m_stTimeval.tv_usec / 1000;
		}
		void RefreshTime();


	private:
		timeval m_stTimeval; //!<��ȷʱ�̣���Ҫ���ڼ���ʱ���

	};

}


#endif //__TIME_VALUE_HPP__
