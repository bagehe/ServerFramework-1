
#include "HashMap_K32.hpp"

#include "LogAdapter.hpp"

using namespace ServerLib;



CHashMap_K32* CHashMap_K32::CreateHashMap(char* pszMemoryAddress,
        const unsigned int uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    return new(pszMemoryAddress) CHashMap_K32(iNodeNumber);
}

CHashMap_K32* CHashMap_K32::ResumeHashMap(char* pszMemoryAddress,
        const unsigned int uiFreeMemorySize, const int iNodeNumber)
{
    if((NULL == pszMemoryAddress) || (CountSize(iNodeNumber) > uiFreeMemorySize))
    {
        return NULL;
    }

    CHashMap_K32* pHashMap = (CHashMap_K32*) pszMemoryAddress;

    if (pHashMap->GetNodeSize() != iNodeNumber)
    {
        return NULL;
    }

    pHashMap->m_pastHashNode = (THashMapNode_K32*) ((char*) pHashMap + sizeof(CHashMap_K32));
    pHashMap->m_paiHashFirstIndex = (int*) ((char*) (pHashMap->m_pastHashNode) + sizeof(THashMapNode_K32) * iNodeNumber);

    return pHashMap;
}

size_t CHashMap_K32::CountSize(const int iNodeNumber)
{
    return sizeof(CHashMap_K32) + (sizeof(THashMapNode_K32) + sizeof(int)) * iNodeNumber;
}

CHashMap_K32::CHashMap_K32()
{
}

CHashMap_K32::CHashMap_K32(int iNodeNumber)
{
    __ASSERT_AND_LOG(iNodeNumber > 0);

    m_iErrorNO = 0;

    m_iNodeNumber = iNodeNumber;

    m_pastHashNode = (THashMapNode_K32*)((char*)(this) + sizeof(CHashMap_K32));
    m_paiHashFirstIndex = (int*)((char*)(m_pastHashNode) + sizeof(THashMapNode_K32) * iNodeNumber);

    EraseAll();
}

CHashMap_K32::~CHashMap_K32()
{
}

int CHashMap_K32::EraseAll()
{
    m_iUsedNodeNumber = 0;
    m_iFirstFreeIndex = 0;

    int i;
    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_pastHashNode[i].m_iHashNext = i + 1;
        m_pastHashNode[i].m_iIsNodeUsed = EHNS_K32_FREE;
    }

    m_pastHashNode[m_iNodeNumber-1].m_iHashNext = -1;

    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_paiHashFirstIndex[i] = -1;
    }

    return 0;
}

int CHashMap_K32::GetValueByIndex(const int iNodeIndex, int& riValue)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNode[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    riValue = m_pastHashNode[iNodeIndex].m_iValue;

    return 0;
}

int CHashMap_K32::GetValueByKey(const unsigned int uiKey, int& riValue)
{
    int iHashIndex = HashKeyToIndex(uiKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        if(uiKey == m_pastHashNode[iCurrentIndex].m_uiKey)
        {
            riValue = m_pastHashNode[iCurrentIndex].m_iValue;
            break;
        }

        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    if(iCurrentIndex == -1)
    {
        return -2;
    }

    return 0;
}

int* CHashMap_K32::GetValuePtrByKey(const unsigned int uiKey)
{
    int iHashIndex = HashKeyToIndex(uiKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];
    int* piValue = NULL;

    while(iCurrentIndex != -1)
    {
        if(uiKey == m_pastHashNode[iCurrentIndex].m_uiKey)
        {
            piValue = &m_pastHashNode[iCurrentIndex].m_iValue;
            break;
        }

        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    if(iCurrentIndex == -1)
    {
        return NULL;
    }

    return piValue;
}

int CHashMap_K32::DeleteByKey(const unsigned int uiKey, int& riValue)
{
    int iHashIndex = HashKeyToIndex(uiKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //�Ѿ����ڸ�Key�򷵻�ʧ��
        if(uiKey == m_pastHashNode[iCurrentIndex].m_uiKey)
        {
            break;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    //�ҵ���Ҫɾ���Ľڵ�
    if(iCurrentIndex != -1)
    {
        //�ǳ�ͻ����ͷ�ڵ�
        if(m_paiHashFirstIndex[iHashIndex] == iCurrentIndex)
        {
            m_paiHashFirstIndex[iHashIndex] = m_pastHashNode[iCurrentIndex].m_iHashNext;
        }
        //����һ���ڵ��Next�������ڵ�ǰҪɾ���ڵ��Next����
        else
        {
            m_pastHashNode[iPreIndex].m_iHashNext = m_pastHashNode[iCurrentIndex].m_iHashNext;
        }

        riValue = m_pastHashNode[iCurrentIndex].m_iValue;

        m_pastHashNode[iCurrentIndex].m_iIsNodeUsed = EHNS_K32_FREE;
        m_pastHashNode[iCurrentIndex].m_iHashNext = m_iFirstFreeIndex;
        m_iFirstFreeIndex = iCurrentIndex;
        --m_iUsedNodeNumber;
    }
    else
    {
        // δ�ҵ�������ʧ��
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);
        return -2;
    }

    return 0;
}

int CHashMap_K32::InsertValueByKey(const unsigned int uiKey, const int iValue)
{
    int iHashIndex = HashKeyToIndex(uiKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    //�Ѿ�û�п��õĽڵ���
    if(m_iFirstFreeIndex < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iPreIndex = -1;
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //�Ѿ����ڸ�Key�򷵻�ʧ��
        if(uiKey == m_pastHashNode[iCurrentIndex].m_uiKey)
        {
            SetErrorNO(EEN_HASH_MAP__INSERT_FAILED_FOR_KEY_DUPLICATE);

            return -3;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    int iNowAssignIdx = m_iFirstFreeIndex;
    m_iFirstFreeIndex = m_pastHashNode[m_iFirstFreeIndex].m_iHashNext;
    ++m_iUsedNodeNumber;
    m_pastHashNode[iNowAssignIdx].m_uiKey = uiKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_K32_USED;
    m_pastHashNode[iNowAssignIdx].m_iHashNext = -1;

    //�ǳ�ͻ����ĵ�һ���ڵ�
    if(m_paiHashFirstIndex[iHashIndex] == -1)
    {
        m_paiHashFirstIndex[iHashIndex] = iNowAssignIdx;
    }
    else
    {
        m_pastHashNode[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

int CHashMap_K32::UpdateValueByKey(const unsigned int uiKey, const int iValue)
{
    int* piValue = GetValuePtrByKey(uiKey);
    if(!piValue)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);
        return -1;
    }

    *piValue = iValue;

    return 0;
}

int CHashMap_K32::ReplaceValueByKey(const unsigned int uiKey, const int iValue)
{
    int iHashIndex = HashKeyToIndex(uiKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //�Ѿ����ڸ�Key��ֱ�Ӹ���
        if(uiKey == m_pastHashNode[iCurrentIndex].m_uiKey)
        {
            m_pastHashNode[iCurrentIndex].m_iValue = iValue;
            return 0;
        }

        iPreIndex = iCurrentIndex;
        iCurrentIndex = m_pastHashNode[iCurrentIndex].m_iHashNext;
    }

    //�Ѿ�û�п��õĽڵ���
    if(m_iFirstFreeIndex < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iNowAssignIdx = m_iFirstFreeIndex;
    m_iFirstFreeIndex = m_pastHashNode[m_iFirstFreeIndex].m_iHashNext;
    ++m_iUsedNodeNumber;
    m_pastHashNode[iNowAssignIdx].m_uiKey = uiKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_K32_USED;
    m_pastHashNode[iNowAssignIdx].m_iHashNext = -1;

    //�ǳ�ͻ����ĵ�һ���ڵ�
    if(m_paiHashFirstIndex[iHashIndex] == -1)
    {
        m_paiHashFirstIndex[iHashIndex] = iNowAssignIdx;
    }
    else
    {
        m_pastHashNode[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

int CHashMap_K32::HashKeyToIndex(const unsigned int uiKey) const
{
    return (int)(((uiKey>>16) + ((uiKey<<16) >> 16)) % m_iNodeNumber);
}
