/**
*@file HashMap_KString.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_KSTRING_HPP__
#define __HASH_MAP_KSTRING_HPP__



#include <new>

#include "LogAdapter.hpp"

#include "ErrorDef.hpp"

#include "StringUtility.hpp"


namespace ServerLib
{
#define MAX_KEY_SIZE 64
#define MAX_VALUE_SIZE 64


typedef enum enmHashNodeStatus
{
    EHNS_KSTRING_FREE = 0, //!<Hash�ڵ�δʹ��
    EHNS_KSTRING_USED = 1, //!<Hash�ڵ���ʹ��
} ENMHASHNODESTATUS;

typedef struct tagDataString
{
    char KeyString[MAX_KEY_SIZE];

    tagDataString()
    {
        KeyString[0] = '\0';
    }

    tagDataString(const char* pszString)
    {
        SAFE_STRCPY(KeyString, pszString, sizeof(KeyString) - 1);
        KeyString[MAX_KEY_SIZE - 1] = '\0';
    }

    tagDataString(const tagDataString& stDataString)
    {
        SAFE_STRCPY(KeyString, stDataString.KeyString, sizeof(KeyString) - 1);
        KeyString[MAX_KEY_SIZE - 1] = '\0';
    }

    tagDataString& operator=(const tagDataString& stDataString)
    {
        SAFE_STRCPY(KeyString, stDataString.KeyString, sizeof(KeyString)-1);
        KeyString[MAX_KEY_SIZE - 1] = '\0';

        return *this;
    }
} TDataString;
//
//const int HASHMAP_KSTRING_MAX_NODE_NUMBER = 200000;

template <typename VALUE_TYPE>
class CHashMap_KString
{


    typedef struct tagHashMapNode_KSTRING
    {
        TDataString m_stPriKey; //!<char[]���͵Ľڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        VALUE_TYPE m_iValue; //!<�������
        int m_iIsNodeUsed; //!<�ڵ��Ƿ�ʹ�� 1-ʹ�� 0-δʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode_KSTRING;

public:

    static CHashMap_KString* CreateHashMap(char* pszMemoryAddress,
                                           const unsigned int uiFreeMemorySize,
                                           const int iNodeNumber);




    bool CompareKey(const TDataString& key1,const TDataString& key2);

    static size_t CountSize(const int iNodeNumber);

private:
    CHashMap_KString();

public:
    CHashMap_KString(int iNodeNumber);

    ~CHashMap_KString();

public:

    //������Ӧ��ָ��
    int AttachHashMap(char* pszMemoryAddress);

    ////����m_iUseFlag1
    //int SetUseFlagNW(const int iUseFlag);

    ////��ȡm_iUseFlag1
    //int& GetUseFlagNW();

    ////����m_iUseFlag2
    //int SetUseFlagNR(const int iUseFlag);

    ////��ȡm_iUseFlag2
    //int& GetUseFlagNR();

    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndex(const int iNodeIndex, TDataString& riHashMapKey);

    //��������Assist���ߵĸ�������
    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndexAssist(const int iNodeIndex, TDataString& riHashMapKey);




    //!ָ��Keyֵ����ȡ���ݣ�����ֵ0��ʾ�ɹ�
    int GetValueByKey(const TDataString stPriKey, VALUE_TYPE& riValue);

    VALUE_TYPE* GetValuePtrByKey(const TDataString stPriKey);

    //!�����ӦKey�Ľڵ�
    int DeleteByKey(const TDataString stPriKey, VALUE_TYPE& riValue);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int InsertValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int UpdateValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int ReplaceValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue);

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
    int HashKeyToIndex(const TDataString stPriKey) const;

    int BKDRHash(const char* szStr,int iStrLength) const;

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iNodeNumber;

    //int m_iUseFlagNW;//��Ǹ�HashMap�Ƿ񱻱��ݹ���ռ���У���ֵ�μ�ENMHASHUSEFLAG���ñ��NameServerֻ��д
    //int m_iUseFlagNR;//��Ǹ�HashMap�Ƿ񱻱��ݹ���ռ���У���ֵ�μ�ENMHASHUSEFLAG���ñ��NameServerֻ�ܶ�

    int m_iErrorNO; //!������
    int m_iUsedNodeNumber; //!<�Ѿ�ʹ�õĽڵ����
    int m_iFirstFreeIndex; //!<��������ͷ�ڵ�
    THashMapNode_KSTRING* m_pastHashNode; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndex; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

    //��HashMap�ĸ���ָ��
    THashMapNode_KSTRING* m_pastHashNodeAssist; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndexAssist; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

};



template <typename VALUE_TYPE>
CHashMap_KString<VALUE_TYPE>* CHashMap_KString<VALUE_TYPE>::CreateHashMap(char* pszMemoryAddress,
        const unsigned int uiFreeMemorySize,
        const int iNodeNumber)
{
    if(CountSize(iNodeNumber) > uiFreeMemorySize)
    {
        return NULL;
    }

    return new(pszMemoryAddress) CHashMap_KString(iNodeNumber);
}

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::AttachHashMap(char* pszMemoryAddress)
{
    __ASSERT_AND_LOG(m_iNodeNumber >= 0);
    m_pastHashNodeAssist = (THashMapNode_KSTRING*)((char*)(pszMemoryAddress) + sizeof(CHashMap_KString));
    m_paiHashFirstIndexAssist = (int*)((char*)(m_pastHashNodeAssist) + sizeof(THashMapNode_KSTRING) * m_iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNodeAssist,(unsigned int)m_paiHashFirstIndexAssist);

    m_pastHashNode = m_pastHashNodeAssist;
    m_paiHashFirstIndex = m_paiHashFirstIndexAssist;

    return 0;
}

////����m_iUseFlagNW
//template <typename VALUE_TYPE>
//int CHashMap_KString<VALUE_TYPE>::SetUseFlagNW(const int iUseFlag)
//{
//	m_iUseFlagNW = iUseFlag;
//	return 0;
//}

////��ȡm_iUseFlagNW
//template <typename VALUE_TYPE>
//int& CHashMap_KString<VALUE_TYPE>::GetUseFlagNW()
//{
//	return m_iUseFlagNW;
//}

////����m_iUseFlagNR
//template <typename VALUE_TYPE>
//int CHashMap_KString<VALUE_TYPE>::SetUseFlagNR(const int iUseFlag)
//{
//	m_iUseFlagNR = iUseFlag;
//	return 0;
//}

////��ȡm_iUseFlagNR
//template <typename VALUE_TYPE>
//int& CHashMap_KString<VALUE_TYPE>::GetUseFlagNR()
//{
//	return m_iUseFlagNR;
//}

template <typename VALUE_TYPE>
size_t CHashMap_KString<VALUE_TYPE>::CountSize(const int iNodeNumber)
{
    return sizeof(CHashMap_KString) + (sizeof(THashMapNode_KSTRING) + sizeof(int)) * iNodeNumber;
}

template <typename VALUE_TYPE>
CHashMap_KString<VALUE_TYPE>::CHashMap_KString()
{


}

template <typename VALUE_TYPE>
CHashMap_KString<VALUE_TYPE>::CHashMap_KString(int iNodeNumber)
{
    __ASSERT_AND_LOG(iNodeNumber > 0);

    //m_iUseFlagNR=EHNS_IDLE;// ��HashMap��״̬��Ϊidle״̬
    //m_iUseFlagNW=EHNS_IDLE;// ��HashMap��״̬��Ϊidle״̬

    m_iErrorNO = 0;

    m_iNodeNumber = iNodeNumber;

    m_pastHashNode = (THashMapNode_KSTRING*)((char*)(this) + sizeof(CHashMap_KString));
    m_paiHashFirstIndex = (int*)((char*)(m_pastHashNode) + sizeof(THashMapNode_KSTRING) * iNodeNumber);
    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);

    EraseAll();
}

template <typename VALUE_TYPE>
CHashMap_KString<VALUE_TYPE>::~CHashMap_KString()
{
}

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::EraseAll()
{
    m_iUsedNodeNumber = 0;
    m_iFirstFreeIndex = 0;

    int i;
    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_pastHashNode[i].m_iHashNext = i + 1;
        m_pastHashNode[i].m_iIsNodeUsed = EHNS_KSTRING_FREE;
    }

    m_pastHashNode[m_iNodeNumber-1].m_iHashNext = -1;

    for(i = 0; i < m_iNodeNumber; ++i)
    {
        m_paiHashFirstIndex[i] = -1;
    }

    //TRACESVR("pastHashNode size:%u, HashFirstIndex size:%u \n",(unsigned int)m_pastHashNode,(unsigned int)m_paiHashFirstIndex);
    return 0;
}

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::GetValueByIndex(const int iNodeIndex, VALUE_TYPE& riValue)
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
template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::GetKeyByIndex(const int iNodeIndex, TDataString& riHashMapKey)
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

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::GetValueByIndexAssist(const int iNodeIndex, VALUE_TYPE& riValue)
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
template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::GetKeyByIndexAssist(const int iNodeIndex, TDataString& riHashMapKey)
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




template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::GetValueByKey(const TDataString stPriKey, VALUE_TYPE& riValue)
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

template <typename VALUE_TYPE>
VALUE_TYPE* CHashMap_KString<VALUE_TYPE>::GetValuePtrByKey(const TDataString stPriKey)
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

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::DeleteByKey(const TDataString stPriKey, VALUE_TYPE& riValue)
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

        m_pastHashNode[iCurrentIndex].m_iIsNodeUsed = EHNS_KSTRING_FREE;
        m_pastHashNode[iCurrentIndex].m_iHashNext = m_iFirstFreeIndex;
        m_iFirstFreeIndex = iCurrentIndex;
        --m_iUsedNodeNumber;
    }

    return 0;
}

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::InsertValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue)
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
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTRING_USED;
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

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::UpdateValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue)
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

template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::ReplaceValueByKey(const TDataString stPriKey, const VALUE_TYPE iValue)
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
    m_pastHashNode[iNowAssignIdx].m_iIsNodeUsed = EHNS_KSTRING_USED;
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


const unsigned int SEED = 131;
const unsigned int MASK = 0x7FFFFFFF;

// BKDR Hash Function
template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::BKDRHash(const char* szStr,int iStrLength) const
{
    //unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
    unsigned int hash = 0;
    unsigned int *piTemp = (unsigned int*)szStr;

    //��szStr��ÿ���ֽڽ��в���
    unsigned int i;
    for( i = 0; i < iStrLength / sizeof(int); ++i)
    {
        //hash += piTemp[i];
        hash = hash * SEED + (piTemp[i]);
    }

    if(iStrLength % sizeof(int) > 0)
    {
        unsigned char* pByte = (unsigned char*)szStr;
        pByte += (iStrLength - (iStrLength % sizeof(int)));
        unsigned int uiTemp = 0; // ֮ǰδ��ʼ�������¶���ͬ��key���ɵ�hash location����ͬ
        memcpy((void *)&uiTemp, (const void *)pByte, iStrLength%sizeof(int));
        hash += uiTemp;
    }

    //int i=0;
    //for(i=0;i<iLength;i++)
    //{
    //	//hash = hash * seed + (*szStr++);
    //	hash = hash * SEED + (*szStr++);
    //}

    //return (hash & 0x7FFFFFFF);
    return (hash & MASK);
}


template <typename VALUE_TYPE>
int CHashMap_KString<VALUE_TYPE>::HashKeyToIndex(const TDataString stPriKey) const
{
    //int hash_index=BKDRHash(stPriKey.KeyString,strlen(stPriKey.KeyString));
    int iRealLength = strnlen(stPriKey.KeyString,sizeof(stPriKey.KeyString));
    int hash_index=BKDRHash(stPriKey.KeyString,iRealLength);

    hash_index=hash_index % m_iNodeNumber;
    //TRACESVR("HashKeyToIndex.Key=%s, hash_index=%d ,m_iNodeNumber=%d  \n",(char*)stPriKey.KeyString, hash_index,m_iNodeNumber);
    return hash_index;
    //size_t uiKeyLength = sizeof(stPriKey);
    //unsigned int uiHashSum = 0;
    //unsigned int *piTemp = (unsigned int*)&stPriKey;

    ////ĿǰHash�㷨ʵ�ֱȽϼ�ֻ�ǽ�Keyֵ��ÿ���ֽڵ�ֵ����������SIZEȡģ
    //unsigned int i;
    //for( i = 0; i < uiKeyLength / sizeof(unsigned int); ++i)
    //{
    //	uiHashSum += piTemp[i];
    //}

    //if(uiKeyLength % sizeof(unsigned int) > 0)
    //{
    //	unsigned char* pByte = (unsigned char*)&stPriKey;
    //	pByte += (uiKeyLength - (uiKeyLength % sizeof(unsigned int)));
    //	unsigned int uiTemp;
    //	memcpy((void *)&uiTemp, (const void *)pByte, uiKeyLength%sizeof(unsigned int));
    //	uiHashSum += uiTemp;
    //}

    //uiHashSum = (uiHashSum & ((unsigned int)0x7fffffff));

    //int hash_index = (int)(uiHashSum % m_iNodeNumber);

    //return hash_index;
}

template <typename VALUE_TYPE>
bool CHashMap_KString<VALUE_TYPE>::CompareKey(const TDataString& key1,const TDataString& key2)
{
    //if(memcmp((const void*)&key1, (const void*)&key2, sizeof(TDataString)) == 0)
    //if(strcmp((const char*)key1.KeyString,(const char*)key2.KeyString) == 0)
    if(strncmp((const char*)key1.KeyString,(const char*)key2.KeyString,sizeof(key1.KeyString)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


//
//#include <stdio.h>
//
//int main()
//{
//	printf("CountSize %d \n", CHashMap_KString::CountSize(0));
//	printf("CountSize %d \n", CHashMap_KString::CountSize(1));
//	printf("CountSize %d \n", CHashMap_KString::CountSize(2));
//	printf("CountSize %d \n", CHashMap_KString::CountSize(10));
//	printf("CountSize %d \n", CHashMap_KString::CountSize(100));
//	printf("CountSize %d \n", CHashMap_KString::CountSize(100000));
//
//	char* pszBuffer = new char[2500000];
//	CHashMap_KString* pstIntHash = new(pszBuffer) CHashMap_KSTRING(100000);
//
//	pstIntHash->InsertValueByKey(32186690, 10000);
//	pstIntHash->ReplaceValueByKey(10086, 10);
//	pstIntHash->ReplaceValueByKey(32186690, 20000);
//
//	printf("UsedNum = %d\n", pstIntHash->GetUsedNodeNumber());
//
//	int* v1 = pstIntHash->GetValuePtrByKey(32186690);
//
//	if(v1 == NULL)
//	{
//		printf("v1 == NULL\n");
//
//		return -1;
//	}
//
//	printf("v1 = %d\n", *v1);
//
//	pstIntHash->UpdateValueByKey(32186690, 10000);
//
//	v1 = pstIntHash->GetValuePtrByKey(32186690);
//
//	if(v1 == NULL)
//	{
//		printf("v1 == NULL\n");
//
//		return -1;
//	}
//
//	printf("v1 = %d\n", *v1);
//
//	int* v2 = pstIntHash->GetValuePtrByKey(10086);
//
//	if(v2 == NULL)
//	{
//		printf("v2 == NULL\n");
//
//		return -1;
//	}
//
//	printf("v2 = %d\n", *v2);
//
//	pstIntHash->DeleteByKey(10086);
//	printf("UsedNum = %d\n", pstIntHash->GetUsedNodeNumber());
//
//	v2 = pstIntHash->GetValuePtrByKey(10086);
//
//	if(v2 == NULL)
//	{
//		printf("v2 == NULL\n");
//	}
//
//	pstIntHash->EraseAll();
//	printf("UsedNum = %d\n", pstIntHash->GetUsedNodeNumber());
//
//	v1 = pstIntHash->GetValuePtrByKey(32186690);
//
//	if(v1 == NULL)
//	{
//		printf("v1 == NULL\n");
//	}
//
//	delete [] pszBuffer;
//
//	return 0;
//}

}

#endif //__HASH_MAP_KSTRING_HPP__
