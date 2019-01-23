/**
*@file HashMap.hpp
*@author jasonxiong
*@date 2009-12-08
*@version 1.0
*@brief ����Hash������ʵ�ֵļ�Map
*
*	�ڵ������ҪԤ��ָ����С���ݲ�֧���ڽڵ�����ʱ��̬��չ������
*/

#ifndef __HASH_MAP_HPP__
#define __HASH_MAP_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{

const int DEFAULT_HASH_MAP_NODE_SIZE = 1024; //!<Ĭ��HashMap�����ɽڵ����
const int DEFAULT_HASH_MAP_SIZE = 1024; //!<Ĭ��Hash���С�����ֵԽ���ͻԽС

typedef enum enmHashNodeUseFlag
{
    EHNF_NOT_USED = 0, //!<Hash�ڵ�δʹ��
    EHNF_USED = 1, //!<Hash�ڵ���ʹ��
} ENMHASHNODEUSEFLAG;

template <typename KEY_TYPE>
class CDefaultHashKeyCmp
{
public:
    CDefaultHashKeyCmp() {}
    ~CDefaultHashKeyCmp() {}

public:
    bool operator() (const KEY_TYPE& rstKey1, const KEY_TYPE& rstKey2);
};

template <typename KEY_TYPE>
bool CDefaultHashKeyCmp<KEY_TYPE>::operator() (const KEY_TYPE& rstKey1, const KEY_TYPE& rstKey2)
{
    if(memcmp((const void*)&rstKey1, (const void*)&rstKey2, sizeof(rstKey1)) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//!����ģ��ֵ��Key���������ͣ�T���������ͣ�NODE_SIZE���������Ĵ�С����ҪԤ�ȷ��䣬HASH_SIZE��Hash���С
template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE = DEFAULT_HASH_MAP_NODE_SIZE,
         typename CMP_FUNC = CDefaultHashKeyCmp<KEY_TYPE>, int HASH_SIZE = NODE_SIZE>
class CHashMap
{
    typedef struct tagHashMapNode
    {
        KEY_TYPE m_stKey; //!<�ڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        DATA_TYPE m_stData; //!<�������
        char m_cUseFlag; //!<�ڵ��Ƿ�ʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode;

public:
    CHashMap();
    ~CHashMap();

public:
    ////!ͨ�������±�ķ�ʽ��ȡֵ����������STL��Map������ָ�Keyֵ�Ľڵ㲻���ڻ���룩
    ////TBD ���ڲ����쳣���ƣ���ʱ��ʵ��
    //DATA_TYPE& operator[](const KEY_TYPE& rstKeyval);

    //!ָ��Keyֵ����ȡ����ָ�룬����������򷵻�NULL������ֱ�ӷ������ݵ�ָ��
    DATA_TYPE* Find(const KEY_TYPE& rstKeyval);

    //!ָ������ֵ����ȡ����ָ�룬һ�����ڱ�����
    DATA_TYPE* GetByIndex(int iDataIdx);

    //!ָ������ֵ����ȡ�ؼ���ָ�룬һ�����ڱ�����
    KEY_TYPE* GetKeyByIndex(int iDataIdx);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int Insert(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int Update(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int Replace(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData);

    //!��ȡ���ýڵ����
    int GetUsedNum() const
    {
        return m_iUsedNodeNum;
    }

    //!��ȡ���ýڵ����
    int GetFreeNum() const
    {
        return NODE_SIZE - m_iUsedNodeNum;
    }

    //!��ȡ�ܹ��Ľڵ����
    int GetSize() const
    {
        return NODE_SIZE;
    }

    //!������нڵ�
    int Erase();

    //!�����ӦKey�Ľڵ�
    int Erase(const KEY_TYPE& rstKey);

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!������Keyֵͨ��Hashת��������
    int HashKeyToIndex(const KEY_TYPE& rstKey, int& riIndex) const;

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!������
    int m_iUsedNodeNum; //!<�Ѿ�ʹ�õĽڵ����
    int m_iFirstFreeIdx; //!<��������ͷ�ڵ�
    THashMapNode m_astHashMap[NODE_SIZE]; //!<���д�ŵ����ݽڵ�
    int m_aiHashFirstIdx[HASH_SIZE]; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����
};

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::CHashMap()
{
    m_iErrorNO = 0;
    m_iUsedNodeNum = 0;
    m_iFirstFreeIdx = 0;

    int i;
    for(i = 0; i < NODE_SIZE; ++i)
    {
        m_astHashMap[i].m_iHashNext = i + 1;
        m_astHashMap[i].m_cUseFlag = EHNF_NOT_USED;
    }

    m_astHashMap[NODE_SIZE-1].m_iHashNext = -1;

    for(i = 0; i < HASH_SIZE; ++i)
    {
        m_aiHashFirstIdx[i] = -1;
    }
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::~CHashMap()
{
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Erase()
{
    m_iUsedNodeNum = 0;
    m_iFirstFreeIdx = 0;

    int i;
    for(i = 0; i < NODE_SIZE; ++i)
    {
        m_astHashMap[i].m_iHashNext = i + 1;
        m_astHashMap[i].m_cUseFlag = EHNF_NOT_USED;
    }

    m_astHashMap[NODE_SIZE-1].m_iHashNext = -1;

    for(i = 0; i < HASH_SIZE; ++i)
    {
        m_aiHashFirstIdx[i] = -1;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
DATA_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::GetByIndex(int iDataIdx)
{
    if(iDataIdx < 0 || iDataIdx >= NODE_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    if(m_astHashMap[iDataIdx].m_cUseFlag == EHNF_USED)
    {
        return &m_astHashMap[iDataIdx].m_stData;
    }
    else
    {
        return NULL;
    }
}


template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
KEY_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::GetKeyByIndex(int iDataIdx)
{
    if(iDataIdx < 0 || iDataIdx >= NODE_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    if(m_astHashMap[iDataIdx].m_cUseFlag == EHNF_USED)
    {
        return &m_astHashMap[iDataIdx].m_stKey;
    }
    else
    {
        return NULL;
    }
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Erase(const KEY_TYPE& rstKeyval)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //�Ѿ����ڸ�Key�򷵻�ʧ��
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            break;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    //�ҵ���Ҫɾ���Ľڵ�
    if(iCurIndex != -1)
    {
        //�ǳ�ͻ����ͷ�ڵ�
        if(m_aiHashFirstIdx[iHashIdx] == iCurIndex)
        {
            m_aiHashFirstIdx[iHashIdx] = m_astHashMap[iCurIndex].m_iHashNext;
        }
        //����һ���ڵ��Next�������ڵ�ǰҪɾ���ڵ��Next����
        else
        {
            m_astHashMap[iPreIndex].m_iHashNext = m_astHashMap[iCurIndex].m_iHashNext;
        }

        m_astHashMap[iCurIndex].m_cUseFlag = EHNF_NOT_USED;
        m_astHashMap[iCurIndex].m_iHashNext = m_iFirstFreeIdx;
        m_iFirstFreeIdx = iCurIndex;
        --m_iUsedNodeNum;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Insert(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    //�Ѿ�û�п��õĽڵ���
    if(m_iFirstFreeIdx < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //�Ѿ����ڸ�Key�򷵻�ʧ��
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            SetErrorNO(EEN_HASH_MAP__INSERT_FAILED_FOR_KEY_DUPLICATE);

            return -3;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    int iNowAssignIdx = m_iFirstFreeIdx;
    m_iFirstFreeIdx = m_astHashMap[m_iFirstFreeIdx].m_iHashNext;
    ++m_iUsedNodeNum;
    memcpy(&m_astHashMap[iNowAssignIdx].m_stKey, &rstKeyval, sizeof(rstKeyval));
    memcpy(&m_astHashMap[iNowAssignIdx].m_stData, &rstData, sizeof(rstData));
    m_astHashMap[iNowAssignIdx].m_cUseFlag = EHNF_USED;
    m_astHashMap[iNowAssignIdx].m_iHashNext = -1;

    //�ǳ�ͻ����ĵ�һ���ڵ�
    if(m_aiHashFirstIdx[iHashIdx] == -1)
    {
        m_aiHashFirstIdx[iHashIdx] = iNowAssignIdx;
    }
    else
    {
        m_astHashMap[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Update(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    DATA_TYPE* pstData = Find(rstKeyval);

    if(pstData == NULL)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_NOT_EXISTED);

        return -1;
    }

    memcpy(pstData, &rstData, sizeof(rstData));

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Replace(const KEY_TYPE& rstKeyval, const DATA_TYPE& rstData)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return -1;
    }

    int iPreIndex = -1;
    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        //�Ѿ����ڸ�Key��ֱ�Ӹ���
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            memcpy(&m_astHashMap[iCurIndex].m_stKey, &rstKeyval, sizeof(rstKeyval));
            memcpy(&m_astHashMap[iCurIndex].m_stData, &rstData, sizeof(rstData));

            return 0;
        }

        iPreIndex = iCurIndex;
        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    //�Ѿ�û�п��õĽڵ���
    if(m_iFirstFreeIdx < 0)
    {
        SetErrorNO(EEN_HASH_MAP__NODE_IS_FULL);

        return -2;
    }

    int iNowAssignIdx = m_iFirstFreeIdx;
    m_iFirstFreeIdx = m_astHashMap[m_iFirstFreeIdx].m_iHashNext;
    ++m_iUsedNodeNum;
    memcpy(&m_astHashMap[iNowAssignIdx].m_stKey, &rstKeyval, sizeof(rstKeyval));
    memcpy(&m_astHashMap[iNowAssignIdx].m_stData, &rstData, sizeof(rstData));
    m_astHashMap[iNowAssignIdx].m_cUseFlag = EHNF_USED;
    m_astHashMap[iNowAssignIdx].m_iHashNext = -1;

    //�ǳ�ͻ����ĵ�һ���ڵ�
    if(m_aiHashFirstIdx[iHashIdx] == -1)
    {
        m_aiHashFirstIdx[iHashIdx] = iNowAssignIdx;
    }
    else
    {
        m_astHashMap[iPreIndex].m_iHashNext = iNowAssignIdx;
    }

    return 0;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
DATA_TYPE* CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::Find(const KEY_TYPE& rstKeyval)
{
    int iHashIdx = 0;
    HashKeyToIndex(rstKeyval, iHashIdx);

    if(iHashIdx < 0 || iHashIdx >= HASH_SIZE)
    {
        SetErrorNO(EEN_HASH_MAP__INVALID_INDEX);

        return NULL;
    }

    int iCurIndex = m_aiHashFirstIdx[iHashIdx];
    CMP_FUNC stCmpFunc;

    while(iCurIndex != -1)
    {
        if(stCmpFunc(rstKeyval, m_astHashMap[iCurIndex].m_stKey))
        {
            return &m_astHashMap[iCurIndex].m_stData;
        }

        iCurIndex = m_astHashMap[iCurIndex].m_iHashNext;
    }

    return NULL;
}

template <typename KEY_TYPE, typename DATA_TYPE, int NODE_SIZE, typename CMP_FUNC, int HASH_SIZE>
int CHashMap<KEY_TYPE, DATA_TYPE, NODE_SIZE, CMP_FUNC, HASH_SIZE>::HashKeyToIndex(const KEY_TYPE& rstKey, int& riIndex) const
{
    size_t uiKeyLength = sizeof(rstKey);
    unsigned int uiHashSum = 0;
    unsigned int *piTemp = (unsigned int*)&rstKey;

    //ĿǰHash�㷨ʵ�ֱȽϼ�ֻ�ǽ�Keyֵ��ÿ���ֽڵ�ֵ����������SIZEȡģ
    unsigned int i;
    for( i = 0; i < uiKeyLength / sizeof(unsigned int); ++i)
    {
        uiHashSum += piTemp[i];
    }

    if(uiKeyLength % sizeof(unsigned int) > 0)
    {
        unsigned char* pByte = (unsigned char*)&rstKey;
        pByte += (uiKeyLength - (uiKeyLength % sizeof(unsigned int)));
        unsigned int uiTemp;
        memcpy((void *)&uiTemp, (const void *)pByte, uiKeyLength%sizeof(unsigned int));
        uiHashSum += uiTemp;
    }

    uiHashSum = (uiHashSum & ((unsigned int)0x7fffffff));

    riIndex = (int)(uiHashSum % HASH_SIZE);

    return 0;
}

}

#endif //__HASH_MAP_HPP__
///:~
