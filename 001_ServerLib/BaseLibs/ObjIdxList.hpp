#ifndef __OBJIDX_LIST_HPP__
#define __OBJIDX_LIST_HPP__

#include "LogAdapter.hpp"
using namespace ServerLib;

struct TObjIdxListNode
{
    int m_iPrevIdx; // ��һ���ڵ�
    int m_iNextIdx; // ��һ���ڵ�

public:
    void Initialize()
    {
        m_iPrevIdx = -1;
        m_iNextIdx = -1;
    }
};

struct TObjIdxListHead
{
    int m_iNumber;      // ����Ԫ������
    int m_iFirstIdx;    // ͷ�����
    int m_iLastIdx;     // β���ڵ�

public:
    void Initialize()
    {
        m_iNumber = 0;
        m_iFirstIdx = -1;
        m_iLastIdx = -1;
    }
};

typedef TObjIdxListNode* (GETOBJLISTNODE)(int iIdx);

// ��Idx����������
int AddObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode);

// ��Idx��������ɾ��
int DelObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode);

#endif
