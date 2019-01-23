/**
*@file BiDirQueue.cpp
*@author jasonxiong
*@date 2009-12-14
*@version 1.0
*@brief CBiDirQueue��ʵ���ļ�
*
*
*/

#include <assert.h>
#include <stdio.h>

#include "BiDirQueue.hpp"
#include "ErrorDef.hpp"
#include "SharedMemory.hpp"
#include "LogAdapter.hpp"

using namespace ServerLib;

CBiDirQueue* CBiDirQueue::CreateByGivenMemory(char* pszMemoryAddress,
        const size_t uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    CBiDirQueue* pstBiDirQueue = (CBiDirQueue*)pszMemoryAddress;

    if(!pstBiDirQueue)
    {
        return NULL;
    }

    pstBiDirQueue->m_iMaxItemCount = iNodeNumber;
    pstBiDirQueue->m_shQueueAllocType = EBDT_ALLOC_BY_SHARED_MEMORY;
    pstBiDirQueue->m_astQueueItems = (TBiDirQueueItem*)
                                     ((unsigned char*)pszMemoryAddress + sizeof(CBiDirQueue));

    pstBiDirQueue->Initialize();

    return pstBiDirQueue;
}

CBiDirQueue* CBiDirQueue::ResumeByGivenMemory(char* pszMemoryAddress,
        const size_t uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    CBiDirQueue* pstBiDirQueue = (CBiDirQueue*)pszMemoryAddress;

    if(!pstBiDirQueue)
    {
        return NULL;
    }

    pstBiDirQueue->m_iMaxItemCount = iNodeNumber;
    pstBiDirQueue->m_shQueueAllocType = EBDT_ALLOC_BY_SHARED_MEMORY;
    pstBiDirQueue->m_astQueueItems = (TBiDirQueueItem*)
                                     ((unsigned char*)pszMemoryAddress + sizeof(CBiDirQueue));

    return pstBiDirQueue;
}

size_t CBiDirQueue::CountSize(const int iNodeNumber)
{
    size_t uiSize = sizeof(CBiDirQueue) +
                    (unsigned int)iNodeNumber * sizeof(TBiDirQueueItem);

    return uiSize;
}

CBiDirQueue::CBiDirQueue()
{

}

CBiDirQueue::CBiDirQueue(int iMaxItemCount)
{
    __ASSERT_AND_LOG(iMaxItemCount > 0);

    m_astQueueItems = new TBiDirQueueItem[iMaxItemCount];

    __ASSERT_AND_LOG(m_astQueueItems);

    m_shQueueAllocType = EBDT_ALLOC_BY_SELF;
    m_iMaxItemCount = iMaxItemCount;

    Initialize();
}

CBiDirQueue::~CBiDirQueue()
{
    if(m_shQueueAllocType == EBDT_ALLOC_BY_SELF)
    {
        if(m_astQueueItems)
        {
            delete []m_astQueueItems;
            m_astQueueItems = NULL;
        }
    }
}

CBiDirQueue* CBiDirQueue::CreateBySharedMemory(const char* pszKeyFileName,
        const unsigned char ucKeyPrjID,
        int iMaxItemCount)
{
    if(pszKeyFileName == NULL || iMaxItemCount <= 0)
    {
        return NULL;
    }

    CSharedMemory stSharedMemory;
    unsigned int uiSharedMemorySize = sizeof(CBiDirQueue) +
                                      (unsigned int)iMaxItemCount * sizeof(TBiDirQueueItem);

    int iRet = stSharedMemory.CreateShmSegment(pszKeyFileName, ucKeyPrjID,
               (int)uiSharedMemorySize);

    if(iRet)
    {
        return NULL;
    }

    //�ڹ����ڴ�ĵ�ַ�Ϸ���CodeQueue
    CBiDirQueue* pstBiDirQueue = (CBiDirQueue*)stSharedMemory.GetFreeMemoryAddress();

    if(!pstBiDirQueue)
    {
        return NULL;
    }

    pstBiDirQueue->m_iMaxItemCount = iMaxItemCount;
    pstBiDirQueue->m_shQueueAllocType = EBDT_ALLOC_BY_SHARED_MEMORY;
    pstBiDirQueue->m_astQueueItems = (TBiDirQueueItem*)((unsigned char*)stSharedMemory.GetFreeMemoryAddress() +
                                     sizeof(CBiDirQueue));

    return pstBiDirQueue;
}

int CBiDirQueue::Initialize()
{
    m_iErrorNO = 0;
    m_iHeadIdx = -1;
    m_iTailIdx = -1;
    m_iCurItemCount = 0;

    int i;
    for(i = 0; i < m_iMaxItemCount; ++i)
    {
        m_astQueueItems[i].m_iNextItem = -1;
        m_astQueueItems[i].m_iPreItem = -1;
        m_astQueueItems[i].m_cUseFlag = EINF_NOT_USED;
    }

    return 0;
}

int CBiDirQueue::GetNextItem(int iItemIdx, int& iNextItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�ýڵ�δ��ʹ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_NOT_USED)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__ITEM_NOT_BE_USED);

        return -3;
    }

    iNextItemIdx = m_astQueueItems[iItemIdx].m_iNextItem;

    return 0;
}

int CBiDirQueue::GetPrevItem(int iItemIdx, int& iPrevItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�ýڵ�δ��ʹ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_NOT_USED)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__ITEM_NOT_BE_USED);

        return -3;
    }

    iPrevItemIdx = m_astQueueItems[iItemIdx].m_iPreItem;

    return 0;
}

int CBiDirQueue::SetNextItem(int iItemIdx, int iNextItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    m_astQueueItems[iItemIdx].m_iNextItem = iNextItemIdx;

    return 0;
}

int CBiDirQueue::SetPrevItem(int iItemIdx, int iPrevItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    m_astQueueItems[iItemIdx].m_iPreItem = iPrevItemIdx;

    return 0;
}

int CBiDirQueue::DeleteItem(int iItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�ýڵ�δ��ʹ����ֱ�ӷ��سɹ�
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_NOT_USED)
    {
        m_astQueueItems[iItemIdx].m_iPreItem = -1;
        m_astQueueItems[iItemIdx].m_iNextItem = -1;

        return 0;
    }

    int iPrevItemIdx = m_astQueueItems[iItemIdx].m_iPreItem;
    int iNextItemIdx = m_astQueueItems[iItemIdx].m_iNextItem;

    if(iPrevItemIdx == -1)
    {
        //ǰ�ӽڵ�Ϊ�գ���ɾ���ڵ��Ϊ�׽ڵ�
        if(iItemIdx != m_iHeadIdx)
        {
            SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

            return -3;
        }

        m_iHeadIdx = iNextItemIdx;
    }
    else
    {
        SetNextItem(iPrevItemIdx, iNextItemIdx);
    }

    if(iNextItemIdx == -1)
    {
        //�����ڵ�Ϊ�գ���ɾ���ڵ��Ϊβ�ڵ�
        if(iItemIdx != m_iTailIdx)
        {
            SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

            return -4;
        }

        m_iTailIdx = iPrevItemIdx;
    }
    else
    {
        SetPrevItem(iNextItemIdx, iPrevItemIdx);
    }

    m_astQueueItems[iItemIdx].m_iPreItem = -1;
    m_astQueueItems[iItemIdx].m_iNextItem = -1;
    m_astQueueItems[iItemIdx].m_cUseFlag = EINF_NOT_USED;

    --m_iCurItemCount;

    return 0;
}

int CBiDirQueue::AppendItemToTail(int iItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�Ѿ������һ���ڵ��ˣ���ֱ�ӷ���
    if(m_iTailIdx == iItemIdx)
    {
        return 0;
    }

    //����ýڵ��Ѿ��ڶ����У�����Ҫ�ȴӶ�����ɾ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_USED)
    {
        DeleteItem(iItemIdx);
    }

    m_astQueueItems[iItemIdx].m_iNextItem = -1;
    m_astQueueItems[iItemIdx].m_iPreItem = m_iTailIdx;
    m_astQueueItems[iItemIdx].m_cUseFlag = EINF_USED;
    ++m_iCurItemCount;

    if(m_iTailIdx == -1)
    {
        m_iTailIdx = m_iHeadIdx = iItemIdx;
    }
    else
    {
        SetNextItem(m_iTailIdx, iItemIdx);

        m_iTailIdx = iItemIdx;
    }

    return 0;
}

int CBiDirQueue::PopHeadItem()
{
    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -1;
    }

    int iTempItem = m_iHeadIdx;

    if(m_iHeadIdx != -1)
    {
        DeleteItem(m_iHeadIdx);
    }

    return iTempItem;
}

int CBiDirQueue::PopTailItem()
{
    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -1;
    }

    int iTempItem = m_iHeadIdx;

    if(m_iTailIdx != -1)
    {
        DeleteItem(m_iTailIdx);
    }

    return iTempItem;
}

int CBiDirQueue::InsertItemToHead(int iItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�Ѿ��ǵ�һ���ڵ��ˣ���ֱ�ӷ���
    if(m_iHeadIdx == iItemIdx)
    {
        return 0;
    }

    //����ýڵ��Ѿ��ڶ����У�����Ҫ�ȴӶ�����ɾ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_USED)
    {
        DeleteItem(iItemIdx);
    }

    m_astQueueItems[iItemIdx].m_iPreItem = -1;
    m_astQueueItems[iItemIdx].m_iNextItem = m_iHeadIdx;
    m_astQueueItems[iItemIdx].m_cUseFlag = EINF_USED;
    ++m_iCurItemCount;

    if(m_iHeadIdx == -1)
    {
        m_iTailIdx = m_iHeadIdx = iItemIdx;
    }
    else
    {
        SetPrevItem(m_iHeadIdx, iItemIdx);

        m_iHeadIdx = iItemIdx;
    }

    return 0;
}

int CBiDirQueue::InsertItemAfter(int iItemIdx, int iPrevItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount ||
            iPrevItemIdx < 0 || iPrevItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�����ڵ�������ͬ��
    if(iItemIdx == iPrevItemIdx)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INSERT_SAME_INDEX);

        return -3;
    }

    //�Ѿ�������ȷ��λ�ã�ֱ�ӷ���
    if(m_astQueueItems[iPrevItemIdx].m_iNextItem == iItemIdx)
    {
        return 0;
    }

    //����ýڵ��Ѿ��ڶ����У�����Ҫ�ȴӶ�����ɾ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_USED)
    {
        DeleteItem(iItemIdx);
    }

    int iNextItemIdx = m_astQueueItems[iPrevItemIdx].m_iNextItem;

    if(iNextItemIdx == -1)
    {
        //������λ��û�к����ڵ㣬���Ϊβ�ڵ�
        if(iPrevItemIdx != m_iTailIdx)
        {
            SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

            return -4;
        }

        return AppendItemToTail(iItemIdx);
    }

    //����ڵ�ĺ����ڵ㼴iPrevItemIdx�ĺ����ڵ�
    m_astQueueItems[iItemIdx].m_iNextItem = iNextItemIdx;
    //����ڵ��ǰ�ӽڵ㼴iPrevItemIdx
    m_astQueueItems[iItemIdx].m_iPreItem = iPrevItemIdx;

    //ǰ�ӽڵ�ĺ����ڵ��޸�Ϊ����ڵ�
    m_astQueueItems[iPrevItemIdx].m_iNextItem = iItemIdx;
    //�����ڵ��ǰ�ӽڵ��޸�Ϊ����ڵ�
    m_astQueueItems[iNextItemIdx].m_iPreItem = iItemIdx;

    m_astQueueItems[iItemIdx].m_cUseFlag = EINF_USED;

    ++m_iCurItemCount;

    return 0;
}

int CBiDirQueue::InsertItemBefore(int iItemIdx, int iNextItemIdx)
{
    if(iItemIdx < 0 || iItemIdx >= m_iMaxItemCount ||
            iNextItemIdx < 0 || iNextItemIdx >= m_iMaxItemCount)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

        return -1;
    }

    if(m_astQueueItems == NULL)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__NULL_POINTER);

        return -2;
    }

    //�����ڵ�������ͬ��
    if(iItemIdx == iNextItemIdx)
    {
        SetErrorNO(EEN_BI_DIR_QUEUE__INSERT_SAME_INDEX);

        return -3;
    }

    //�Ѿ�������ȷ��λ�ã�ֱ�ӷ���
    if(m_astQueueItems[iNextItemIdx].m_iPreItem == iItemIdx)
    {
        return 0;
    }

    //����ýڵ��Ѿ��ڶ����У�����Ҫ�ȴӶ�����ɾ��
    if(m_astQueueItems[iItemIdx].m_cUseFlag == EINF_USED)
    {
        DeleteItem(iItemIdx);
    }

    int iPrevItemIdx = m_astQueueItems[iNextItemIdx].m_iPreItem;

    //������λ����ǰһ���ڵ㣬���Ϊͷ�ڵ�
    if(iPrevItemIdx == -1)
    {
        if(iNextItemIdx != m_iHeadIdx)
        {
            SetErrorNO(EEN_BI_DIR_QUEUE__INVALID_ITEM_IDX);

            return -4;
        }

        return InsertItemToHead(iItemIdx);
    }

    //����ڵ����һ���ڵ㼴iNextItemIdx
    m_astQueueItems[iItemIdx].m_iNextItem = iNextItemIdx;
    //����ڵ��ǰһ���ڵ㼴iNextItemIdx��ǰһ���ڵ�
    m_astQueueItems[iItemIdx].m_iPreItem = iPrevItemIdx;

    //iNextItemIdx�ڵ��ǰһ���ڵ��޸�Ϊ����ڵ㣬��һ���ڵ㱣�ֲ���
    m_astQueueItems[iNextItemIdx].m_iPreItem = iItemIdx;
    //iPrevItemIdx�ڵ�ĺ�һ���ڵ��޸�Ϊ����ڵ㣬ǰһ���ڵ㱣�ֲ���
    m_astQueueItems[iPrevItemIdx].m_iNextItem = iItemIdx;

    m_astQueueItems[iItemIdx].m_cUseFlag = EINF_USED;

    ++m_iCurItemCount;

    return 0;
}
