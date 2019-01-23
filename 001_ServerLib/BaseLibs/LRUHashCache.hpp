#ifndef __LRU_HASH_CACHE_HPP__
#define __LRU_HASH_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"
#include "HashMap_K32.hpp"
#include "BiDirQueue.hpp"
//#include "LogAdapter.hpp"

#define HASH_NODE_MULTIPLE    2 // hash������Ϊ�����������2��

using namespace ServerLib;

// ��Ҫ�Զ����յ�cache�������������hash��ʽ��˫��̬����ʽ����
// ����cache����������ɣ�����Σ�����idx�����obj���飩������hash����Σ�����queue�Ρ�
// ���ʹ�õĶ��������queue��β���������ڴ����ͷ���ĳ��������ʱ����Ҫ�����ƶ�
// ��queue��β����ʹ��queue��ͷ����������δʹ�õĽ�㡣
// hash�����һ��Ϊ�������������ɱ���queue��������ڶ�������
template <typename TConcreteObj>
class CLRUHashCache
{
private:
    static CObjAllocator* m_pAllocator; // ����Σ�����idx��obj��idx����obj���ķ��䡢���չ���
    static CHashMap_K32* m_pHashMap;    // �������ε�hash�����ڿ��ٷ���
    static CBiDirQueue* m_pBiDirQueue;  // �������ε�˫����У����ڻ���λ�ڶ���ͷ����������δʹ�õĶ�����

private:
    static int m_iObjNodeNumber;   // �������ܸ���������ռ�ʱʹ��
    static int m_iHashNodeNumber;  // hash����ܸ���������ռ�ʱʹ��
    static int m_iQueueNodeNumber; // queue����ܸ���������ռ�ʱʹ��
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

    static void SetQueueNodeNumber(const int iNodeNumber);
    static int GetQueueNodeNumber();

    // ���ý�����
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();
    static int CaculateHashSegSize();
    static int CaculateQueueSegSize();

    // ���仺��ʱ�ȵ���CaculateSize������Ҫ����Ĵ�С��Ȼ�����AllocateFromShmʵʩ����
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

private:
    // �ͷ�˫�����ͷ����iNodeNumber�����
    static void SwapOut(const int iNodeNumber);

    // ������ɾ�������ʵ���������Ľӿ�
public:
    static TConcreteObj* CreateByUin(const unsigned int uiUin);
    static int DeleteByUin(const unsigned int uiUin);
    static TConcreteObj* GetByUin(const unsigned int uiUin);
    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();
};

// ��̬���ݳ�Ա�ĳ�ʼ��
template <typename TConcreteObj>
CObjAllocator* CLRUHashCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
CHashMap_K32* CLRUHashCache<TConcreteObj>::m_pHashMap = NULL;

template <typename TConcreteObj>
CBiDirQueue* CLRUHashCache<TConcreteObj>::m_pBiDirQueue = NULL;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iHashNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iQueueNodeNumber = 0;

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::m_iSingleObjSize = 0;

// ��Ա����
template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetHashNodeNumber(const int iNodeNumber,
        const int iMultiple)
{
    m_iHashNodeNumber = iNodeNumber * iMultiple;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetHashNodeNumber()
{
    return m_iHashNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetQueueNodeNumber(const int iNodeNumber)
{
    m_iQueueNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetQueueNodeNumber()
{
    return m_iQueueNodeNumber;
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
    SetHashNodeNumber(iNodeNumber, HASH_NODE_MULTIPLE);
    SetQueueNodeNumber(iNodeNumber);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateHashSegSize()
{
    return CHashMap_K32::CountSize(GetHashNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateQueueSegSize()
{
    return CBiDirQueue::CountSize(GetQueueNodeNumber());
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();
    int iHashSegSize = CaculateHashSegSize();
    int iQueueSegSize = CaculateQueueSegSize();

    return (iObjSegSize + iHashSegSize + iQueueSegSize);
}

template <typename TConcreteObj>
void CLRUHashCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
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

        // ����cache����ʱ���˫�����
        m_pBiDirQueue = CBiDirQueue::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetQueueNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pBiDirQueue);
        shm.UseShmBlock(CaculateQueueSegSize());
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

        // ����cache����ʱ���˫�����
        m_pBiDirQueue = CBiDirQueue::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetQueueNodeNumber());
        ASSERT_AND_LOG_RTN_VOID(m_pBiDirQueue);
        shm.UseShmBlock(CaculateQueueSegSize());
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
void CLRUHashCache<TConcreteObj>::SwapOut(const int iNodeNumber)
{
    int iRemoveObjID = -1;
    int i = 0;

    while (i < iNodeNumber)
    {
        iRemoveObjID = m_pBiDirQueue->GetHeadItem();
        TConcreteObj* pObj = (TConcreteObj*)m_pAllocator->GetObj(iRemoveObjID);

        ASSERT_AND_LOG_RTN_VOID(pObj);
        unsigned int uiUin = pObj->GetUin();
        //TRACESVR("swap out, uin: %u, obj id: %d\n", uiUin, iRemoveObjID);

        DeleteByUin(uiUin);
        i++;
    }
}

template <typename TConcreteObj>
TConcreteObj* CLRUHashCache<TConcreteObj>::CreateByUin(const unsigned int uiUin)
{
    // ���û�п��ý���ˣ����ͷ�������δʹ�õ�10�����
    if (GetFreeNodeNumber() < 1)
    {
        SwapOut(10);
    }

    // �Ӷ����������һ��δʹ�õĽ��
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

    // �����µĽ��ҵ�queue��β�������븨��queue�Ĺ���
    iRet = m_pBiDirQueue->AppendItemToTail(iObjID);
    if (iRet != 0)
    {
        TRACESVR("Append object to tail of LRU queue failed\n");
        return NULL;
    }

    //TRACESVR("create ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::DeleteByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // ���븨��hash��Ĺ���
    int iRet = m_pHashMap->DeleteByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from hash table failed\n");
        return -1;
    }

    // ���븨��queue�Ĺ���
    iRet = m_pBiDirQueue->DeleteItem(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Delete object from LRU queue failed\n");
        return -2;
    }

    // �����黹��������г�Ϊδʹ�ý��
    iRet = m_pAllocator->DestroyObject(iObjID);
    if (iRet < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -3;
    }

    //TRACESVR("delete ok, obj id: %d\n", iObjID);

    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CLRUHashCache<TConcreteObj>::GetByUin(const unsigned int uiUin)
{
    int iObjID = 0;

    // ͨ������hash��ʵ�ֿ��ٷ���
    int iRet = m_pHashMap->GetValueByKey(uiUin, iObjID);
    if (iRet < 0)
    {
        TRACESVR("Get object from hash table failed\n");
        return NULL;
    }

    // ���ý���ƶ���queue��β��
    iRet = m_pBiDirQueue->AppendItemToTail(iObjID);
    if (iRet != 0)
    {
        TRACESVR("Appent object to tail of LRU queue failed\n");
        return NULL;
    }

    //TRACESVR("get ok, uin: %u, obj id: %d\n", uiUin, iObjID);

    return (TConcreteObj*)m_pAllocator->GetObj(iObjID);
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CLRUHashCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

#endif // __LRU_HASH_CACHE_HPP__
