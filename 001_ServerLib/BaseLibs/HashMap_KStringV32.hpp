/**
*@file HashMap_KStringV32.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_KSTRINGV32_HPP__
#define __HASH_MAP_KSTRINGV32_HPP__



#include <new>

#include "ErrorDef.hpp"



namespace ServerLib
{
#include "StringUtility.hpp"

#define MAX_KEY_SIZE 64

typedef enum enmHashNodeStatus
{
    EHNS_KSTRINGV32_FREE = 0, //!<Hash�ڵ�δʹ��
    EHNS_KSTRINGV32_USED = 1, //!<Hash�ڵ���ʹ��
} ENMHASHNODESTATUS;

typedef struct tagDataStringV32
{
    char KeyStringV32[MAX_KEY_SIZE];

    tagDataStringV32()
    {
        KeyStringV32[0] = '\0';
    }

    tagDataStringV32(const char* pszStringV32)
    {
        SAFE_STRCPY(KeyStringV32, pszStringV32, sizeof(KeyStringV32) - 1);
        KeyStringV32[MAX_KEY_SIZE - 1] = '\0';
    }
} TDataStringV32;
//
//const int HASHMAP_KSTRINGV32_MAX_NODE_NUMBER = 200000;

class CHashMap_KStringV32
{
    typedef struct tagHashMapNode_KSTRINGV32
    {
        TDataStringV32 m_stPriKey; //!<char[]���͵Ľڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        int m_iValue; //!<�������
        int m_iIsNodeUsed; //!<�ڵ��Ƿ�ʹ�� 1-ʹ�� 0-δʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode_KSTRINGV32;

public:

    static CHashMap_KStringV32* CreateHashMap(char* pszMemoryAddress,
            const unsigned int uiFreeMemorySize,
            const int iNodeNumber);




    bool CompareKey(const TDataStringV32& key1,const TDataStringV32& key2);

    static size_t CountSize(const int iNodeNumber);

private:
    CHashMap_KStringV32();

public:
    CHashMap_KStringV32(int iNodeNumber);

    ~CHashMap_KStringV32();

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
    int GetValueByIndex(const int iNodeIndex, int& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndex(const int iNodeIndex, TDataStringV32& riHashMapKey);

    //��������Assist���ߵĸ�������
    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndexAssist(const int iNodeIndex, int& riValue);

    //!ָ������ֵ����ȡkey��һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetKeyByIndexAssist(const int iNodeIndex, TDataStringV32& riHashMapKey);




    //!ָ��Keyֵ����ȡ���ݣ�����ֵ0��ʾ�ɹ�
    int GetValueByKey(const TDataStringV32 stPriKey, int& riValue);

    int* GetValuePtrByKey(const TDataStringV32 stPriKey);

    //!�����ӦKey�Ľڵ�
    int DeleteByKey(const TDataStringV32 stPriKey, int& riValue);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int InsertValueByKey(const TDataStringV32 stPriKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int UpdateValueByKey(const TDataStringV32 stPriKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int ReplaceValueByKey(const TDataStringV32 stPriKey, const int iValue);

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
    int HashKeyToIndex(const TDataStringV32 stPriKey) const;

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
    THashMapNode_KSTRINGV32* m_pastHashNode; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndex; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

    //��HashMap�ĸ���ָ��
    THashMapNode_KSTRINGV32* m_pastHashNodeAssist; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndexAssist; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

};
}

#endif //__HASH_MAP_KSTRINGV32_HPP__
