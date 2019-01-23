/**
*@file HashMap_K32.hpp
*@author
*@date
*@version 1.0
*@brief
*
*
*/

#ifndef __HASH_MAP_K32_HPP__
#define __HASH_MAP_K32_HPP__



#include <new>



#include "ErrorDef.hpp"



namespace ServerLib
{
class CHashMap_K32
{

    typedef enum enmHashNodeStatus
    {
        EHNS_K32_FREE = 0, //!<Hash�ڵ�δʹ��
        EHNS_K32_USED = 1, //!<Hash�ڵ���ʹ��
    } ENMHASHNODESTATUS;

    //const int HASHMAP_K32_MAX_NODE_NUMBER = 200000;

    typedef struct tagHashMapNode_K32
    {
        unsigned int m_uiKey; //!<�ڵ�����ֵ���������Key�����Hashֵ���ҵ��ڵ�
        int m_iValue; //!<�������
        int m_iIsNodeUsed; //!<�ڵ��Ƿ�ʹ�� 1-ʹ�� 0-δʹ��
        int m_iHashNext; //!<��Hashֵ��ͻֵ�����¼ӽڵ���ڽڵ���棬�γɳ�ͻ��
    } THashMapNode_K32;

public:
    static CHashMap_K32* CreateHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize,
                                       const int iNodeNumber);

    static size_t CountSize(const int iNodeNumber);

    static CHashMap_K32* ResumeHashMap(char* pszMemoryAddress,
                                       const unsigned int uiFreeMemorySize, const int iNodeNumber);

private:
    CHashMap_K32();

public:
    CHashMap_K32(int iNodeNumber);

    ~CHashMap_K32();

public:
    //!ָ������ֵ����ȡ���ݣ�һ�����ڱ����У�����ֵ0��ʾ�ɹ�
    int GetValueByIndex(const int iNodeIndex, int& riValue);

    //!ָ��Keyֵ����ȡ���ݣ�����ֵ0��ʾ�ɹ�
    int GetValueByKey(const unsigned int uiKey, int& riValue);

    int* GetValuePtrByKey(const unsigned int uiKey);

    //!�����ӦKey�Ľڵ�
    int DeleteByKey(const unsigned int uiKey, int& riValue);

    //!ָ��Keyֵ������һ�����ݣ��������ͬKeyֵ�Ľڵ���ڣ���ʧ�ܣ�
    int InsertValueByKey(const unsigned int uiKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������κ��£�
    int UpdateValueByKey(const unsigned int uiKey, const int iValue);

    //!ָ��Keyֵ������һ�����ݣ����δ���ָ�Keyֵ�����������һ�����ݣ�
    int ReplaceValueByKey(const unsigned int uiKey, const int iValue);

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
    int HashKeyToIndex(const unsigned int uiKey) const;

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
    THashMapNode_K32* m_pastHashNode; //!<���д�ŵ����ݽڵ�
    int* m_paiHashFirstIndex; //!<ͨ��Key��Hash������ĳ�ͻ�����ͷ�ڵ�����

};



}

#endif //__HASH_MAP_HPP__
///:~
