/**
*@file HashMap_K64.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_K64_HPP__
#define __HASH_MAP_K64_HPP__

#include <new>
#include "ErrorDef.hpp"

namespace ServerLib
{
typedef struct tagData64
{
    unsigned int m_uiKey1;
    unsigned int m_uiKey2;
} TData64;

class CHashMap_K64
{
    typedef enum enmHashNodeStatus
    {
        EHNS_K64_FREE = 0, //!<Hash�ڵ�δʹ��
        EHNS_K64_USED = 1, //!<Hash�ڵ���ʹ��
    } ENMHASHNODESTATUS;

    //const int HASHMAP_K64_MAX_NODE_NUMBER = 200000;

    typedef struct tagHashMapNode_K64
    {
        TData64 m_stPriKey; //!<64λ�ڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        int m_iValue; //!<�������
        int m_iIsNodeUsed; //!<�ڵ��Ƿ�ʹ�� 1-ʹ�� 0-δʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode_K64;

public:
    static CHashMap_K64* CreateHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize,
                                       const int iNodeNumber);
    int CompareKey(const TData64& key1,const TData64& key2);

    static size_t CountSize(const int iNodeNumber);

    static CHashMap_K64* ResumeHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize, const int iNodeNumber);

private:
    CHashMap_K64();

public:
    CHashMap_K64(int iNodeNumber);

    ~CHashMap_K64();

public:
    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndex(const int iNodeIndex, int& riValue);

    //!ָ��Keyֵ����ȡ���ݣ�����ֵ0��ʾ�ɹ�
    int GetValueByKey(const TData64 stPriKey, int& riValue);

    int* GetValuePtrByKey(const TData64 stPriKey);

    //!�����ӦKey�Ľڵ�
    int DeleteByKey(const TData64 stPriKey, int& riValue);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int InsertValueByKey(const TData64 stPriKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int UpdateValueByKey(const TData64 stPriKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int ReplaceValueByKey(const TData64 stPriKey, const int iValue);

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
    int HashKeyToIndex(const TData64 stPriKey) const;

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
    THashMapNode_K64* m_pastHashNode; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndex; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

};



}

#endif //__HASH_MAP_HPP__
///:~
