/**
*@file CachStatistic.hpp
*@author jasonxiong
*@date 2009-11-21
*@version 1.0
*@brief ������״̬ͳ����
*
*	ʹ���������ͳ�ƻ�������״̬��Ϣ���������û��桢���û��棬�����ʵ�
*/

#ifndef __CACHE_STATISTIC_HPP__
#define __CACHE_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{

const int MAX_CACHE_TYPE_NUMBER = 32; //!<���֧�ֵ�CacheID������������ͳ�Ƶ�Cache�������

extern const char* DEFAULT_CACHE_STAT_DIRECTORY_PATH; //!<Cacheͳ���ļ�������ļ���
extern const char* DEFAULT_CACHE_STAT_FILE_NAME; //!<Cacheͳ���ļ���

typedef enum enmCacheStatusItemIndex
{
    ECSI_USED_COUNT = 0, //!<Cacheʹ����
    ECSI_FREE_COUNT, //!<Cache������
    ECSI_RECYCLE_COUNT, //!<Cache�ڻ��ն����еĸ���
    ECSI_SWAP_TIMES, //!<Cache�������Ĵ���
    ECSI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMCACHESTATUSITEMINDEX;

extern const char* g_apszCacheStatusItemName[ECSI_MAX_ITEM_NUMBER];

typedef enum enmReadCacheItemIndex
{
    ERCI_HIT_CACHE_COUNT = 0, //!<��Cache���д���
    ERCI_MISS_CACHE_COUNT, //!<��Cacheδ���д���
    ERCI_HIT_RATE, //!<��Cache������
    ERCI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMREADCACHEITEMINDEX;

extern const char* g_apszReadCacheItemName[ERCI_MAX_ITEM_NUMBER];

typedef enum enmWriteCacheItemIndex
{
    EWCI_HIT_CACHE_COUNT = 0, //!<дCache���д���
    EWCI_MISS_CACHE_COUNT, //!<дCacheδ���д���
    EWCI_HIT_RATE, //!<дCache������
    EWCI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMWRITECACHEITEMINDEX;

extern const char* g_apszWriteCacheItemName[EWCI_MAX_ITEM_NUMBER];

//!����ͨ��CacheID���㶨λ����CStatistic�е�Section����
typedef struct tagStatCacheInfo
{
    int m_iCacheType; //!<Cache���ͣ�ÿһ��Cache���뱣֤���ֵΨһ
    int m_iCacheStatusIndex; //!<CacheStatus��CStaitistic�е�Section����
    int m_iReadCacheIndex; //!<ReadCache��CStatistic�е�Section����
    int m_iWriteCacheIndex; //!<WriteCache��CStatistic�е�Section����
} TStatCacheInfo;

class CCacheStatistic
{
public:
    CCacheStatistic();
    ~CCacheStatistic();

public:
    /**
    *��ʼ�����ڳ�ʼ��ʱ������ڴ��CStatistic���е�Section
    *@param[in] pszStatPath ͳ���ļ�·����Ĭ����../stat/
    *@param[in] pszStatFileName ͳ���ļ�����Ĭ����s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    /**
    *����Cache������ͳ�ƣ�����Ҫĳһ��CacheǰҪ����������������Cache�����������ȥ
    *@param[in] iCacheType Cache���ͣ�ÿһ��Cache���뱣֤���ֵΨһ���������ж�Cache��ͳ����Ҫָ�����ID
    *@param[in] pszCacheName Cache��
    *@return 0 success
    */
    int AddCacheType(int iCacheType, const char* pszCacheName);

    /**
    *����Cacheͳ����Ϣ
    *@param[in] iCacheType Cache���ͣ�ͨ�����ֵ��λ��Section����
    *@param[in] iUsedCount Cache���ø���
    *@param[in] iFreeCount Cache���и���
    *@param[in] iRecycleCount Cache�ڻ��ն����еĸ���
    *@param[in] iSwapTimes Cache�������Ĵ���
    *@return 0 success
    */
    int UpdateCacheStatus(int iCacheType, int iUsedCount, int iFreeCount,
                          int iRecycleCount, int iSwapTimes);

    /**
    *���Ӷ�Cacheʱ��ͳ����Ϣ
    *@param[in] iCacheType Cache���ͣ�ͨ�����ֵ��λ��Section����
    *@param[in] iAddHitCount ��Ҫ���ӵĶ����д���
    *@param[in] iAddMissCount ��Ҫ���ӵĶ���ʧ����
    *@return 0 success
    */
    int AddReadCacheStat(int iCacheType, int iAddHitCount, int iAddMissCount);

    /**
    *����дCacheʱ��ͳ����Ϣ
    *@param[in] iCacheType Cache���ͣ�ͨ�����ֵ��λ��Section����
    *@param[in] iAddHitCount ��Ҫ���ӵ�д���д���
    *@param[in] iAddMissCount ��Ҫ���ӵ�д��ʧ����
    *@return 0 success
    */
    int AddWriteCacheStat(int iCacheType, int iAddHitCount, int iAddMissCount);

    //!��ӡͳ����Ϣ
    void Print();

    //!���ͳ����Ϣ
    void Reset();

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!����CacheStatusͳ�ƶ�
    int AddCacheStatusSection(const char* pszCacheName, int& riSectionIdx);

    //!����ReadCacheͳ�ƶ�
    int AddReachCacheSection(const char* pszCacheName, int& riSectionIdx);

    //!����WriteCacheͳ�ƶ�
    int AddWriteCacheSection(const char* pszCacheName, int& riSectionIdx);

    //!����Cache���ͻ�ȡָ����Cache��Ϣ�ṹ
    TStatCacheInfo* GetCacheInfo(int iCacheType);

private:
    CStatistic m_stStatistic;
    int m_iErrorNO; //!������

    short m_shCacheTypeNum;
    TStatCacheInfo m_astCacheInfo[MAX_CACHE_TYPE_NUMBER];

};

//!һ����˵ֻ���õ�һ��CMsgStatistic�࣬����ʵ��һ����������ʹ��
typedef CSingleton<CCacheStatistic> CacheStatisticSingleton;

}

#endif //__CACHE_STATISTIC_HPP__
///:~
