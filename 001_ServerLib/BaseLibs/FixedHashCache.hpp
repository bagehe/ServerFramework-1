#ifndef __FIXED_HASH_CACHE_HPP__
#define __FIXED_HASH_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_K32.hpp"
//#include "LogAdapter.hpp"

#define HASH_NODE_MULTIPLE    2 // hash������Ϊ�����������2��

using namespace ServerLib;

// �������̶���cache�������������hash��ʽ����
// ����cache����������ɣ�����Σ�����idx�����obj���飩������hash����Ρ�
// hash�����һ��Ϊ�������������ɱ�
template <typename TConcreteObj>
class CFixedHashCache
{
private:
    static CObjAllocator* m_pAllocator; // ����Σ�����idx��obj��idx����obj���ķ��䡢���չ���
    static CHashMap_K32* m_pHashMap;    // �������ε�hash�����ڿ��ٷ���

private:
    static int m_iObjNodeNumber;   // �������ܸ���������ռ�ʱʹ��
    static int m_iHashNodeNumber;  // hash����ܸ���������ռ�ʱʹ��
    static int m_iSingleObjSize;   // ��������Ĵ�С

    // ˽�еĸ�������
private:
    static void SetSingleObjSize();
    static int GetSingleObjSize();

    static void SetObjNodeNumber(const int iNodeNumber);
    static int GetObjNodeNumber();

    // Ϊ���ٳ�ͻ��hash���ռ�һ��Ϊ������ռ�����ɱ���iNodeNumber��
    // �����������iMultiple�Ǳ�����ȡֵΪ1,2,3...
    static void SetHashNodeNumber(const int iNodeNumber, const int iMultiple);
    static int GetHashNodeNumber();

    // ���ý�����
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();
    static int CaculateHashSegSize();

    // ���仺��ʱ�ȵ���CaculateSize������Ҫ����Ĵ�С��Ȼ�����AllocateFromShmʵʩ����
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

    // ������ɾ�������ʵ���������Ľӿ�
public:
    static TConcreteObj* CreateByUin(const unsigned int uiUin);
    static int DeleteByUin(const unsigned int uiUin);
    static TConcreteObj* GetByUin(const unsigned int uiUin);
    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();

public:
    static void ClearCache(time_t& stTime); // �����ڲ���stTime֮ǰ��<=stTime��������cache���
};

// ��̬���ݳ�Ա�ĳ�ʼ��
template <typename TConcreteObj>
CObjAllocator* CFixedHashCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
CHashMap_K32* CFixedHashCache<TConcreteObj>::m_pHashMap = NULL;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iHashNodeNumber = 0;

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::m_iSingleObjSize = 0;

// ��Ա����
template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetHashNodeNumber(const int iNodeNumber,
        const int iMultiple)
{
    m_iHashNodeNumber = iNodeNumber * iMultiple;
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetHashNodeNumber()
{
    return m_iHashNodeNumber;
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
    SetHashNodeNumber(iNodeNumber, HASH_NODE_MULTIPLE);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateHashSegSize()
{
    return CHashMap_K32::CountSize(GetHashNodeNumber());
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();
    int iHashSegSize = CaculateHashSegSize();

    return (iObjSegSize + iHashSegSize);
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
{
    if (!bResume)
    {
        m_pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_K32::CreateHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());
    }
    else
    {
        m_pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
        ASSERT_AND_LOG_RTN_VOID(m_pAllocator);
        shm.UseShmBlock(CaculateObjSegSize());

        m_pHashMap = CHashMap_K32::ResumeHashMap(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetHashNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pHashMap);
        shm.UseShmBlock(CaculateHashSegSize());
    }

    if (bResume)
    {
        // �ָ�ʹ���е�obj
        int iUsedIdx = m_pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = m_pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = m_pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::CreateByUin(const unsigned int uiUin)
{
    // �Ӷ����������һ��δʹ�õĽ�㣬���û�п��ý���ˣ��򷵻�NULL
    int iObjID = m_pAllocator->CreateObject();
    if (iObjID < 0)
    {
        TRACESVR("Create object failed\n");
        return NULL;
    }

    // ���븨��hash��Ĺ���
    int iRet = m_pHashMap->InsertValueByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Insert object to hash table failed\n");
        return NULL;
    }

    //TRACESVR("create ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::DeleteByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // ���븨��hash��Ĺ���
    int iRet = m_pHashMap->DeleteByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from hash table failed\n");
        return -1;
    }

    // �����黹��������г�Ϊδʹ�ý��
    iRet = m_pAllocator->DestroyObject(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -3;
    }

    TRACESVR("delete ok, obj id: %d\n", iObjID);

    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CFixedHashCache<TConcreteObj>::GetByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // ͨ������hash��ʵ�ֿ��ٷ���
    int iRet = m_pHashMap->GetValueByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Get object from hash table failed, uiUin:%u\n", uiUin);
        return NULL;
    }

    //TRACESVR("get ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CFixedHashCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

template <typename TConcreteObj>
void CFixedHashCache<TConcreteObj>::ClearCache(time_t& stTime)
{
    TConcreteObj* pObj = NULL;
    CIdx *pIdx = NULL;
    time_t* pstCreatedTime = NULL;
    int iNextIdx;

    int iObjID = m_pAllocator->GetUsedHead();
    while (iObjID != -1)
    {
        pObj = (TConcreteObj*)m_pAllocator->GetObj(iObjID);
        ASSERT_AND_LOG_RTN_VOID(pObj);
        // TRACESVR("obj id: %d, uin: %u\n", iObjID, pObj->GetUin());

        pIdx = m_pAllocator->GetIdx(iObjID);
        ASSERT_AND_LOG_RTN_VOID(pIdx);
        iNextIdx = pIdx->GetNextIdx();

        pstCreatedTime = pObj->GetCreatedTime();
        if (*pstCreatedTime <= stTime)
        {
            TRACESVR("clear obj cache, id: %d\n", iObjID);
            DeleteByUin(pObj->GetUin());
        }
        
        iObjID = iNextIdx;
    }
}

#endif // __FIXED_HASH_CACHE_HPP__
