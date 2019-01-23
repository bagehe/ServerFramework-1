/**
*@file BiDirQueue.hpp
*@author jasonxiong
*@date 2009-12-14
*@version 1.0
*@brief ˫�����
*
*
*/

#ifndef __BI_DIR_QUEUE_HPP__
#define __BI_DIR_QUEUE_HPP__

namespace ServerLib
{

typedef enum enmItemUseFlag
{
    EINF_NOT_USED = 0, //!<Item�ڵ�δʹ��
    EINF_USED = 1, //!<Item�ڵ���ʹ��
} ENMITEMUSEFLAG;

typedef struct tagBiDirQueueItem
{
    int m_iNextItem; //!<��һ��Item������
    int m_iPreItem; //!<��һ��Item������
    char m_cUseFlag; //!<�ڵ��Ƿ�ʹ��
} TBiDirQueueItem;

typedef enum enmBiDirQueueAllocType
{
    EBDT_ALLOC_BY_SELF = 0, //!<ͨ���Լ���̬�����ڴ�
    EBDT_ALLOC_BY_SHARED_MEMORY = 1, //!<ͨ�������ڴ�����ڴ�
} ENMBIDIRQUEUEALLOCTYPE;

class CBiDirQueue
{
public:
    static CBiDirQueue* CreateByGivenMemory(char* pszMemoryAddress,
                                            const size_t uiFreeMemorySize,
                                            const int iNodeNumber);

    static size_t CountSize(const int iNodeNumber);

private:
    CBiDirQueue();

public:
    CBiDirQueue(int iMaxItemCount);
    ~CBiDirQueue();

    /**
    *ͨ�������ڴ�������CBiDirQueue��ע������������CBiDirQueue�����ʼ����֧�ָֻ�ģʽ������ı����ڹ����ڴ��ֵ��
    *@param[in] pszKeyFileName �����ڴ�Attach���ļ���
    *@param[in] ucKeyPrjID �����ڴ��ProjectID
    *@param[in] iMaxItemCount ����������Item����
    *@return 0 success
    */
    static CBiDirQueue* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iMaxItemCount);

    static CBiDirQueue* ResumeByGivenMemory(char* pszMemoryAddress,
                                            const size_t uiFreeMemorySize,
                                            const int iNodeNumber);

public:
    //!��ʼ������ն���
    int Initialize();

    //!��ȡ���г���
    int GetQueueItemCount() const
    {
        return m_iCurItemCount;
    }

    //!��ȡ����ͷ������
    int	GetHeadItem() const
    {
        return m_iHeadIdx;
    }

    //!��ȡ����β������
    int GetTailItem() const
    {
        return m_iTailIdx;
    }

    //!ɾ��ָ��Item
    int	DeleteItem(int iItemIdx);

    //!��һ��Item��ӵ�ĩβ
    int	AppendItemToTail(int iItemIdx);

    //!��һ��Item��ӵ�����
    int	InsertItemToHead(int iItemIdx);

    //!��ȡ����ͷ���������������Ƴ�����
    int	PopHeadItem();

    //!��ȡ����β���������������Ƴ�����
    int PopTailItem();

    //!��ȡָ��Item����һ��Item�������ɹ�����0
    int	GetNextItem(int iItemIdx, int& iNextItemIdx);

    //!��ȡָ��Item����һ��Item�������ɹ�����0
    int	GetPrevItem(int iItemIdx, int& iPrevItemIdx);

    //!��һ��Item�����һ��Item����
    int	InsertItemAfter( int iItemIdx, int iPrevItemIdx);

    //!��һ��Item�����һ��Itemǰ��
    int	InsertItemBefore(int iItemIdx, int iNextItemIdx);

public:
    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    int	SetNextItem(int iItemIdx, int iNextItemIdx);
    int	SetPrevItem(int iItemIdx, int iPrevItemIdx);

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!������
    int m_iCurItemCount; //!<�����е�ǰ��Item����
    int	m_iHeadIdx; //!<����ͷ������
    int	m_iTailIdx; //!<����β������
    short m_shQueueAllocType; //!<���з�������
    int m_iMaxItemCount; //!<����������Item����
    TBiDirQueueItem* m_astQueueItems; //!<Item����
};

}

#endif //__BI_DIR_QUEUE_HPP__
///:~
