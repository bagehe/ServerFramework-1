#ifndef __FREQ_CTRL_HPP
#define __FREQ_CTRL_HPP

#include "TimeUtility.hpp"
#include "LogAdapter.hpp"

// ����Ƶ�ʿ�����, ��������ĳЩ������ƽ��Ƶ�ʺͷ�ֵƵ��. Ӧ�ó�����DB���ʵ�.

namespace ServerLib
{
template <int TYPE_ID>
class CFreqencyCtroller
{
public:

    // ���ÿ��Ʋ���
    // ƽ��Ƶ��: iMeanTimes / iMeanPeroid  (��/��)
    // ��ֵƵ��: iPeakFreq	(��/��)
    void SetParameters(time_t iMeanPeroid, int iMeanTimes, int iPeakFreq);

    // ����Ƿ����ִ��iTimes�β���
    // ����ֵ:
    //		  true: ����ִ�У����ۼ�iTimes�β���
    //		  false: ������ִ��
    bool CheckFrequency(int iTimes);

private:
    CFreqencyCtroller<TYPE_ID>() {};
public:
    static CFreqencyCtroller<TYPE_ID>* Instance();

private:
    static CFreqencyCtroller<TYPE_ID>* m_pInstance;

    // ���Ʋ���
    time_t m_iMeanPeroidCfg;
    int m_iMeanTimesCfg;
    int m_iPeakFreqCfg;

    // ���в��� - ƽ��Ƶ��
    time_t m_iMeanLastTime;
    int m_iMeanAccTimes;

    // ���в��� - ��ֵƵ��
    time_t m_iPeakLastTime;
    int m_iPeakAccTimes;
};

template <int TYPE_ID>
CFreqencyCtroller<TYPE_ID>* CFreqencyCtroller<TYPE_ID>::m_pInstance = NULL;

template <int TYPE_ID>
CFreqencyCtroller<TYPE_ID>* CFreqencyCtroller<TYPE_ID>::Instance()
{
    if (!m_pInstance)
    {
        m_pInstance = new CFreqencyCtroller();
    }

    return m_pInstance;
}

template <int TYPE_ID>
void CFreqencyCtroller<TYPE_ID>::SetParameters(time_t iMeanPeroid, int iMeanTimes, int iPeakFreq)
{
    m_iMeanPeroidCfg = iMeanPeroid;
    m_iMeanTimesCfg = iMeanTimes;
    m_iPeakFreqCfg = iPeakFreq;

    int iMeanFreqCfg = m_iMeanTimesCfg / m_iMeanPeroidCfg;
    if (m_iPeakFreqCfg > iMeanFreqCfg)
    {
        m_iPeakFreqCfg = iMeanFreqCfg;
    }

    m_iMeanLastTime = time(NULL);
    m_iMeanAccTimes = 0;

    m_iPeakLastTime = time(NULL);
    m_iPeakAccTimes = 0;
}

template <int TYPE_ID>
bool CFreqencyCtroller<TYPE_ID>::CheckFrequency(int iTimes)
{
    if (iTimes <= 0)
    {
        return false;
    }

    time_t iNowTime = CTimeUtility::m_uiTimeTick;

    // ���ƽ��Ƶ��
    time_t iMeanSlapTime = iNowTime - m_iMeanLastTime;
    if (iMeanSlapTime >= m_iMeanPeroidCfg)
    {
        m_iMeanLastTime = iNowTime;
        m_iMeanAccTimes = 0;
    }

    int iMeanAccTimes = m_iMeanAccTimes + iTimes;
    if (iMeanAccTimes > m_iMeanTimesCfg)
    {
        TRACESVR("MeanFreq Check Failed: Peroid = %ld, Times = %d, Last = %ld, Now = %ld, AccTimes = %d\n",
                 m_iMeanPeroidCfg, m_iMeanTimesCfg, m_iMeanLastTime, iNowTime, iMeanAccTimes);
        return false;
    }

    // ����ֵƵ��
    time_t iPeakSlapTime = iNowTime - m_iPeakLastTime;
    if (iPeakSlapTime >= 1)
    {
        m_iPeakLastTime = iNowTime;
        m_iPeakAccTimes = 0;
    }

    int iPeakAccTimes = m_iPeakAccTimes + iTimes;
    if (iPeakAccTimes > m_iPeakFreqCfg)
    {
        TRACESVR("PeakFreq Check Failed: Freq = %d, Last = %ld, Now = %ld, PeakTimes = %d\n",
                 m_iPeakFreqCfg, m_iPeakLastTime, iNowTime, iPeakAccTimes);
        return false;
    }

    // ����Ƶ��ֵ
    m_iMeanAccTimes = iMeanAccTimes;
    m_iPeakAccTimes = iPeakAccTimes;

    return true;
}
}

#endif

