#ifndef __FIXED_CACHE_HPP__
#define __FIXED_CACHE_HPP__

#include "SharedMemory.hpp"
#include "ObjAllocator.hpp"

using namespace ServerLib;

// �������̶���cache�����������
// ����cache����һ������Σ�����idx�����obj���飩��ɡ�
template <typename TConcreteObj>
class CFixedCache
{
private:
    static CObjAllocator* m_pAllocator; // ����Σ�����idx��obj��idx����obj���ķ��䡢���չ���

private:
    static int m_iObjNodeNumber;   // �������ܸ���������ռ�ʱʹ��
    static int m_iSingleObjSize;   // ��������Ĵ�С

    // ˽�еĸ�������
private:
    static void SetSingleObjSize();
    static int GetSingleObjSize();

    static void SetObjNodeNumber(const int iNodeNumber);
    static int GetObjNodeNumber();

    // ���ý�����
    static void SetNodeNumber(const int iNodeNumber);

    static int CaculateObjSegSize();

    // ���仺��ʱ�ȵ���CaculateSize������Ҫ����Ĵ�С��Ȼ�����AllocateFromShmʵʩ����
public:
    static int CaculateSize(const int iNodeNumber);
    static void AllocateFromShm(CSharedMemory& shm, bool bResume);

    // ������ɾ�������ʵ���������Ľӿ�
public:
    // �Ӷ����������һ��δʹ�õĽ�㣬���û�п��ý���ˣ��򷵻�һ����ֵ
    // �ɹ�ʱ�����½��Ķ������ID
    static int Create();

    static int Delete(const int iObjID);

    // ���ݶ������ID��ȡ�ö����㣬����ֵΪNULL��ʾʧ��
    static TConcreteObj* GetByID(const int iObjID);

    static int GetUsedNodeNumber();
    static int GetFreeNodeNumber();
};

// ��̬���ݳ�Ա�ĳ�ʼ��
template <typename TConcreteObj>
CObjAllocator* CFixedCache<TConcreteObj>::m_pAllocator = NULL;

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::m_iObjNodeNumber = 0;

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::m_iSingleObjSize = 0;

// ��Ա����
template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetSingleObjSize()
{
    m_iSingleObjSize = sizeof(TConcreteObj);
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetSingleObjSize()
{
    return m_iSingleObjSize;
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetObjNodeNumber(const int iNodeNumber)
{
    m_iObjNodeNumber = iNodeNumber;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetObjNodeNumber()
{
    return m_iObjNodeNumber;
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::SetNodeNumber(const int iNodeNumber)
{
    SetObjNodeNumber(iNodeNumber);
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::CaculateObjSegSize()
{
    return CObjAllocator::CountSize(GetSingleObjSize(), GetObjNodeNumber());
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::CaculateSize(const int iNodeNumber)
{
    SetSingleObjSize();
    SetNodeNumber(iNodeNumber);

    int iObjSegSize = CaculateObjSegSize();

    return (iObjSegSize);
}

template <typename TConcreteObj>
void CFixedCache<TConcreteObj>::AllocateFromShm(CSharedMemory& shm, bool bResume)
{
    if (!bResume)
    {
        m_pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
    }
    else
    {
        m_pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)shm.GetFreeMemoryAddress(), shm.GetFreeMemorySize(),
            GetSingleObjSize(), GetObjNodeNumber(),
            TConcreteObj::CreateObject);
    }

    ASSERT_AND_LOG_RTN_VOID(m_pAllocator);

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

    shm.UseShmBlock(CaculateObjSegSize());
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::Create()
{
    int iObjID = m_pAllocator->CreateObject();
    if (iObjID < 0)
    {
        TRACESVR("Create object failed\n");
        return -1;
    }

    //TRACESVR("create ok, obj id: %d\n", iObjID);
    return iObjID;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::Delete(const int iObjID)
{
    // �����黹��������г�Ϊδʹ�ý��
    int iRes = m_pAllocator->DestroyObject(iObjID);
    if (iRes < 0)
    {
        TRACESVR("Destroy object failed\n");
        return -1;
    }

    //TRACESVR("delete ok, obj id: %d\n", iObjID);
    return 0;
}

template <typename TConcreteObj>
TConcreteObj* CFixedCache<TConcreteObj>::GetByID(const int iObjID)
{
    TConcreteObj* pObj = (TConcreteObj*)m_pAllocator->GetObj(iObjID);

    //TRACESVR("get ok,obj id: %d\n", iObjID);
    return pObj;
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetUsedNodeNumber()
{
    return m_pAllocator->GetUsedCount();
}

template <typename TConcreteObj>
int CFixedCache<TConcreteObj>::GetFreeNodeNumber()
{
    return m_pAllocator->GetFreeCount();
}

#endif // __FIXED_CACHE_HPP__
