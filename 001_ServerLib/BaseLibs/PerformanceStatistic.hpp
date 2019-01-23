/**
*@file PerformanceStatistic.hpp
*@author jasonxiong
*@date 2009-12-21
*@version 1.0
*@brief ����ͳ����
*
*
*/

#ifndef __PERFORMANCE_STATISTIC_HPP__
#define __PERFORMANCE_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

const int MAX_PERFORMANCE_STAT_OP_NUMBER = 128; //!<���ͳ�Ƶ�OP��������

/**
*@brief OP������Ҫͳ�Ƶ�ͳ��������
*
*
*/
typedef enum enmStatOPItemIndex
{
    ESOI_OP_CALL_TIMES = 0, //!<OP���ô���
    ESOI_SUM_PROCESS_TIME = 1, //!<OP�ܴ���ʱ��
    ESOI_AVA_PROCESS_TIME = 2, //!<OPƽ������ʱ��
    ESOI_MAX_PROCESS_TIME = 3, //!<OP�����ʱ��
    ESOI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMSTATOPITEMINDEX;

extern const char* g_apszOPItemName[ESOI_MAX_ITEM_NUMBER];

class CPerformanceStatistic
{
public:
    CPerformanceStatistic();
    ~CPerformanceStatistic();

public:
    /**
    *��ʼ�����ڳ�ʼ��ʱ������ڴ��CStatistic���е�Section
    *@param[in] pszStatPath ͳ���ļ�·����Ĭ����../stat/
    *@param[in] pszStatFileName ͳ���ļ�����Ĭ����s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    /**
    *��Ӳ���ͳ����Ϣ
    *@param[in] pszOPName �������֣�һ���Ǻ�������
    *@param[in] tvProcessTime ���ú�ʱ
    *@return 0 success
    */
    int AddOPStat(const char* pszOPName, timeval tvProcessTime);

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    void Print();
    void Reset();

private:
    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    CStatistic m_stStatistic;
    int m_iErrorNO; //!������
};

//!һ����˵ֻ���õ�һ��CPerformanceStatistic�࣬����ʵ��һ����������ʹ��
typedef CSingleton<CPerformanceStatistic> PerformanceStatisticSingleton;

}

#endif //__PERFORMANCE_STATISTIC_HPP__
///:~
