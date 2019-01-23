/**
*@file LinearQueue.hpp
*@author jasonxiong
*@date 2009-12-21
*@version 1.0
*@brief ���Զ�����
*
*
*/

#ifndef __LINEAR_QUEUE_HPP__
#define __LINEAR_QUEUE_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
class CLinearQueue
{
public:
    CLinearQueue();
    ~CLinearQueue();

public:
    //!��ȡ�����ײ�����
    const DATA_TYPE* GetHeadData() const;

    //!��ȡ����β������
    const DATA_TYPE* GetTailData() const;

    //!��ȡ���г���
    int GetQueueLength() const;

    //!��ȡ�����ײ����ݣ�������Pop������
    DATA_TYPE* PopHeadData();

    //!��ն���
    int Clear();

    //!�������Push����
    int PushData(const DATA_TYPE& rstData);

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!������
    DATA_TYPE m_astData[MAX_NODE_NUMBER]; //!<��������
    int m_iHeadIdx; //!<�����ײ�����
    int m_iTailIdx; //!<����β������
};

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::CLinearQueue()
{
    m_iHeadIdx = -1;
    m_iTailIdx = -1;
    m_iErrorNO = 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::~CLinearQueue()
{

}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::Clear()
{
    m_iHeadIdx = -1;
    m_iTailIdx = -1;
    m_iErrorNO = 0;

    return 0;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
const DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetHeadData() const
{
    //������ͷ�ڵ�
    if(m_iHeadIdx < 0 || m_iHeadIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    return &m_astData[m_iHeadIdx];
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
const DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetTailData() const
{
    //������β�ڵ�
    if(m_iTailIdx < 0 || m_iTailIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    return &m_astData[m_iTailIdx];
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::GetQueueLength() const
{
    if(m_iHeadIdx == -1)
    {
        return 0;
    }

    if(m_iHeadIdx <= m_iTailIdx)
    {
        return (m_iTailIdx - m_iHeadIdx) + 1;
    }
    else
    {
        return MAX_NODE_NUMBER - (m_iHeadIdx - m_iTailIdx) + 1;
    }
}


template <typename DATA_TYPE, int MAX_NODE_NUMBER>
DATA_TYPE* CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::PopHeadData()
{
    //������ͷ�ڵ�
    if(m_iHeadIdx < 0 || m_iHeadIdx >= MAX_NODE_NUMBER)
    {
        return NULL;
    }

    DATA_TYPE* pstHeadData = &m_astData[m_iHeadIdx];

    //����ֻ��һ���ڵ���
    if(m_iHeadIdx == m_iTailIdx)
    {
        m_iHeadIdx = -1;
        m_iTailIdx = -1;
    }
    else
    {
        m_iHeadIdx = (m_iHeadIdx + 1) % MAX_NODE_NUMBER;
    }

    return pstHeadData;
}

template <typename DATA_TYPE, int MAX_NODE_NUMBER>
int CLinearQueue<DATA_TYPE, MAX_NODE_NUMBER>::PushData(const DATA_TYPE& rstData)
{
    if((m_iTailIdx + 1) % MAX_NODE_NUMBER == m_iHeadIdx)
    {
        SetErrorNO(EEN_LINEAR_QUEUE__QUEUE_IS_FULL);

        return -1;
    }

    //������û������
    if(m_iTailIdx == -1)
    {
        m_iHeadIdx = m_iTailIdx = 0;
        m_astData[m_iTailIdx] = rstData;
    }
    else
    {
        m_iTailIdx = (m_iTailIdx + 1) % MAX_NODE_NUMBER;
        m_astData[m_iTailIdx] = rstData;
    }

    return 0;
}

}

#endif //__LINEAR_QUEUE_HPP__
///:~
