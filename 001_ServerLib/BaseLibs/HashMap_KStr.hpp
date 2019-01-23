#ifndef __HASH_MAP_KSTR_HPP__
#define __HASH_MAP_KSTR_HPP__

#include <new>
#include <LogAdapter.hpp>
#include "ErrorDef.hpp"
#include "StringUtility.hpp"

namespace ServerLib
{
typedef enum enmHashNodeStatus_str
{
    EHNS_KSTR_FREE = 0, //!<Hash�ڵ�δʹ��
    EHNS_KSTR_USED = 1, //!<Hash�ڵ���ʹ��
} ENMHASHNODESTATUS_STR;

template <int MAX_KEY_LEN>
class  CKeyString
{
public:
    char m_szKeyString[MAX_KEY_LEN];

    CKeyString()
    {
        m_szKeyString[0] = '\0';
    }

    CKeyString(const char* pszString)
    {
        SAFE_STRCPY(m_szKeyString, pszString, sizeof(m_szKeyString) - 1);
        m_szKeyString[MAX_KEY_LEN - 1] = '\0';
    }
};

template <typename VALUE_TYPE, int KEYSIZE>
class CHashMap_KStr
{
    typedef struct tagHashMapNode_KSTR
    {
        CKeyString<KEYSIZE> m_stPriKey; //!<char[]���͵Ľڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        VALUE_TYPE m_iValue; //!<�������
        int m_iIsNodeUsed; //!<�ڵ��Ƿ�ʹ�� 1-ʹ�� 0-δʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode_KSTR;

public:

    static CHashMap_KStr* CreateHashMap(char* pszMemoryAddress,
                                        const unsigned int uiFreeMemorySize,
                                        const int iNodeNumber);

    bool CompareKey(const CKeyString<KEYSIZE>& key1,const CKeyString<KEYSIZE>& key2);

    static size_t CountSize(const int iNodeNumber);

private:
    CHashMap_KStr();

public:
    CHashMap_KStr(int iNodeNumber);

    ~CHashMap_KStr();

public:

    //������Ӧ��ָ��
    int AttachHashMap(char* pszMemoryAddress);

    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndex(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey);

    //��������Assist���ߵĸ�������
    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndexAssist(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey);


    //!ָ��Keyֵ����ȡ���ݣ�����ֵ0��ʾ�ɹ�
    int GetValueByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue);

    VALUE_TYPE* GetValuePtrByKey(const CKeyString<KEYSIZE> stPriKey);

    //!�����ӦKey�Ľڵ�
    int DeleteByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int InsertValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int UpdateValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int ReplaceValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue);

    //!��ȡ���ýڵ����
    int GetUsedNodeNumber() const
    {
        return m_iUsedNodeNumber;
    }

    //!��ȡ���ýڵ����
    int GetFreeNodeNumber() const
    {
        return m_iNodeNumber - m_iUsedNodeNumber;
    }

    //!��ȡ�ܹ��Ľڵ����
    int GetNodeSize() const
    {
        return m_iNodeNumber;
    }

    //!������нڵ�
    int EraseAll();

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!������Keyֵͨ��Hashת��������
    int HashKeyToIndex(const CKeyString<KEYSIZE> stPriKey) const;

    int BKDRHash(const char* szStr,int iStrLength) const;

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iNodeNumber;

    int m_iErrorNO; //!������
    int m_iUsedNodeNumber; //!<�Ѿ�ʹ�õĽڵ����
    int m_iFirstFreeIndex; //!<��������ͷ�ڵ�
    THashMapNode_KSTR* m_pastHashNode; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndex; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

    //��HashMap�ĸ���ָ��
    THashMapNode_KSTR* m_pastHashNodeAssist; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndexAssist; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

};



template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>* CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CreateHashMap(char* pszMemoryAddress,
        const unsigned int uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    return new(pszMemoryAddress) CHashMap_KStr(iNodeNumber);
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::AttachHashMap(char* pszMemoryAddress)
{
    __ASSERT_AND_LOG(m_iNodeNumber >= 0);
    m_pastHashNodeAssist = (THashMapNode_KSTR*)((char*)(pszMemoryAddress) + sizeof(CHashMap_KStr));
    m_paiHashFirstIndexAssist = (int*)((char*)(m_pastHashNodeAssist) + sizeof(THashMapNode_KSTR) * m_iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);

    m_pastHashNode = m_pastHashNodeAssist;
    m_paiHashFirstIndex = m_paiHashFirstIndexAssist;

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
size_t CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CountSize(const int iNodeNumber)
{
    return sizeof(CHashMap_KStr) + (sizeof(THashMapNode_KSTR) + sizeof(int)) * iNodeNumber;
}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CHashMap_KStr()
{


}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CHashMap_KStr(int iNodeNumber)
{
    __ASSERT_AND_LOG(iNodeNumber > 0);

    m_iErrorNO = 0;

    m_iNodeNumber = iNodeNumber;

    m_pastHashNode = (THashMapNode_KSTR*)((char*)(this) + sizeof(CHashMap_KStr));
    m_paiHashFirstIndex = (int*)((char*)(m_pastHashNode) + sizeof(THashMapNode_KSTR) * iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);

    EraseAll();
}

template <typename VALUE_TYPE, int KEYSIZE>
CHashMap_KStr<VALUE_TYPE, KEYSIZE>::~CHashMap_KStr()
{
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::EraseAll()
{
    m_iUsedNodeNumber = 0;
    m_iFirstFreeIndex = 0;

    int i;
    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_pastHashNode[i].m_iHashNext = i + 1;
        m_pastHashNode[i].m_iIsNodeUsed = EHNS_KSTR_FREE;
    }

    m_pastHashNode[m_iNodeNumber-1].m_iHashNext = -1;

    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_paiHashFirstIndex[i] = -1;
    }

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);
    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue)
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

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);
    return 0;
}


//!ָ������ֵ����ȡHash�ڵ㣬һ�����ڱ����У�����ֵ0��ʾ�ɹ�
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetKeyByIndex(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey)
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

    memcpy(&riHashMapKey,&(m_pastHashNode[iNodeIndex].m_stPriKey),sizeof(riHashMapKey) );

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);

    return 0;
}

//////////////����Assist����////////////////////////

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNodeAssist[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    riValue = m_pastHashNodeAssist[iNodeIndex].m_iValue;

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);
    return 0;
}


//!ָ������ֵ����ȡHash�ڵ㣬һ�����ڱ����У�����ֵ0��ʾ�ɹ�
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetKeyByIndexAssist(const int iNodeIndex, CKeyString<KEYSIZE>& riHashMapKey)
{
    if(iNodeIndex < 0 || iNodeIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    if(!m_pastHashNodeAssist[iNodeIndex].m_iIsNodeUsed)
    {
        return -2;
    }

    memcpy(&riHashMapKey,&(m_pastHashNodeAssist[iNodeIndex].m_stPriKey),sizeof(riHashMapKey) );

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);

    return 0;
}

///////////////////////////////////////


template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValueByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    while(iCurrentIndex != -1)
    {
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
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

template <typename VALUE_TYPE, int KEYSIZE>
VALUE_TYPE* CHashMap_KStr<VALUE_TYPE, KEYSIZE>::GetValuePtrByKey(const CKeyString<KEYSIZE> stPriKey)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];
    VALUE_TYPE* piValue = NULL;

    while(iCurrentIndex != -1)
    {
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
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

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::DeleteByKey(const CKeyString<KEYSIZE> stPriKey, VALUE_TYPE& riValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

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
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
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

        m_pastHashNode[iCurrentIndex].m_iIsNodeUsed = EHNS_KSTR_FREE;
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

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::InsertValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

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
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
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
    m_pastHashNode[iNowAssignIdx].m_stPriKey = stPriKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTR_USED;
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

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::UpdateValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    VALUE_TYPE* piValue = GetValuePtrByKey(stPriKey);
    if(!piValue)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);
        return -1;
    }

    *piValue = iValue;

    return 0;
}

template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::ReplaceValueByKey(const CKeyString<KEYSIZE> stPriKey, const VALUE_TYPE iValue)
{
    int iHashIndex = HashKeyToIndex(stPriKey);

    if(iHashIndex < 0 || iHashIndex >= m_iNodeNumber)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    /*assert(m_paiHashFirstIndex);
    assert(m_pastHashNode);*/
    int iCurrentIndex = m_paiHashFirstIndex[iHashIndex];

    //while(iCurrentIndex != -1)
    while(iCurrentIndex >= 0)
    {
        //�Ѿ����ڸ�Key��ֱ�Ӹ���
        //if(stPriKey == m_pastHashNode[iCurrentIndex].m_stPriKey)
        if(CompareKey(stPriKey,m_pastHashNode[iCurrentIndex].m_stPriKey))
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
    m_pastHashNode[iNowAssignIdx].m_stPriKey = stPriKey;
    m_pastHashNode[iNowAssignIdx].m_iValue = iValue;
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTR_USED;
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


const unsigned int STR_SEED = 131;
const unsigned int STR_MASK = 0x7FFFFFFF;

// BKDR Hash Function
template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::BKDRHash(const char* szStr,int iStrLength) const
{
    //unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    unsigned int *piTemp = (unsigned int*)szStr;

    //��szStr��ÿ���ֽڽ��в���
    unsigned int i;
    for( i = 0; i < iStrLength / sizeof(int); ++i)
    {
        //hash += piTemp[i];
        hash = hash * STR_SEED + (piTemp[i]);
    }

    if(iStrLength % sizeof(int) > 0)
    {
        unsigned char* pByte = (unsigned char*)szStr;
        pByte += (iStrLength - (iStrLength % sizeof(int)));
        unsigned int uiTemp = 0; // ֮ǰδ��ʼ�������¶���ͬ��key���ɵ�hash location����ͬ
        memcpy((void *)&uiTemp, (const void *)pByte, iStrLength%sizeof(int));
        hash += uiTemp;
    }

    return (hash & STR_MASK);
}


template <typename VALUE_TYPE, int KEYSIZE>
int CHashMap_KStr<VALUE_TYPE, KEYSIZE>::HashKeyToIndex(const CKeyString<KEYSIZE> stPriKey) const
{
    int iRealLength = strnlen(stPriKey.m_szKeyString,sizeof(stPriKey.m_szKeyString));
    int hash_index=BKDRHash(stPriKey.m_szKeyString,iRealLength);

    hash_index=hash_index % m_iNodeNumber;
    //TRACESVR("HashKeyToIndex.Key=%s, hash_index=%d ,m_iNodeNumber=%d  \n",(char*)stPriKey.KeyString, hash_index,m_iNodeNumber);
    return hash_index;
}

template <typename VALUE_TYPE, int KEYSIZE>
bool CHashMap_KStr<VALUE_TYPE, KEYSIZE>::CompareKey(const CKeyString<KEYSIZE>& key1,const CKeyString<KEYSIZE>& key2)
{
    //if(memcmp((const void*)&key1, (const void*)&key2, sizeof(CKeyString<KEYSIZE>)) == 0)
    //if(strcmp((const char*)key1.KeyString,(const char*)key2.KeyString) == 0)
    if(strncmp((const char*)key1.m_szKeyString,(const char*)key2.m_szKeyString,sizeof(key1.m_szKeyString)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
}

#endif //__HASH_MAP_KSTR_HPP__
