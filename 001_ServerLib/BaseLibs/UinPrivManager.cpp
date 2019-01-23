
#include "UinPrivManager.hpp"

static int CompareUinPrivInfo(const void* pst1, const void* pst2)
{
    TUinPrivInfo* pstData1 = (TUinPrivInfo*)pst1;
    TUinPrivInfo* pstData2 = (TUinPrivInfo*)pst2;

    if (pstData1->m_uiUin == pstData2->m_uiUin)
    {
        return 0;
    }
    else
    {
        if (pstData1->m_uiUin < pstData2->m_uiUin)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }
}

size_t CUinPrivManager::GetObjTotalSize()
{
    if (m_iObjTotalSize > 0)
    {
        return m_iObjTotalSize;
    }

    TRACESVR("********************** OBJECT MEMORY STATICS *********************\n");

    // UinPrivInfo����
    size_t iUinPrivInfoSize = sizeof(TUinPrivInfo) * m_iMaxUinNumber;
    m_iObjTotalSize += iUinPrivInfoSize;
    TRACESVR("UinPrivInfo: Number = %d, UnitSize = %lu, Total = %lu\n",
             m_iMaxUinNumber, (unsigned long)sizeof(TUinPrivInfo), (unsigned long)iUinPrivInfoSize);

    // ���鹲���ڴ�
    m_iObjTotalSize = m_iObjTotalSize * 2;

    // ������������ʶ
    m_iObjTotalSize += sizeof(int*);

    // �ܼ�
    TRACESVR("Total Memory: %luB, %luMB\n", (unsigned long)m_iObjTotalSize, (unsigned long)m_iObjTotalSize/1024/1024);
    TRACESVR("*****************************************************************\n");

    return m_iObjTotalSize;
}

int CUinPrivManager::Initialize(int iUinNumber,const char* pszShmName, bool bIsUinPrivServer)
{
    m_iObjTotalSize = 0;
    m_iMaxUinNumber = iUinNumber;
    size_t iSharedMmorySize = GetObjTotalSize();

    // ���������ڴ�
    int iRet = m_stSharedMemory.CreateShmSegment(pszShmName, 'o', iSharedMmorySize);
    if(iRet < 0)
    {
        return -1;
    }

    m_piActiveController = (int*)m_stSharedMemory.GetFreeMemoryAddress();
    m_stSharedMemory.UseShmBlock(sizeof(int));

    m_astUinPrivControllers[0].m_piNumber = (int*)m_stSharedMemory.GetFreeMemoryAddress();
    m_stSharedMemory.UseShmBlock(sizeof(int));

    m_astUinPrivControllers[0].m_pstCache = (TUinPrivInfo*)m_stSharedMemory.GetFreeMemoryAddress();
    m_stSharedMemory.UseShmBlock(sizeof(TUinPrivInfo) * m_iMaxUinNumber);

    m_astUinPrivControllers[1].m_piNumber = (int*)m_stSharedMemory.GetFreeMemoryAddress();
    m_stSharedMemory.UseShmBlock(sizeof(int));

    m_astUinPrivControllers[1].m_pstCache = (TUinPrivInfo*)m_stSharedMemory.GetFreeMemoryAddress();
    m_stSharedMemory.UseShmBlock(sizeof(TUinPrivInfo) * m_iMaxUinNumber);

    // ��ʼ�������ڴ�
    if (bIsUinPrivServer)
    {
        *m_piActiveController = 0;
        *m_astUinPrivControllers[0].m_piNumber = 0;
        *m_astUinPrivControllers[1].m_piNumber = 0;
    }

    TRACESVR("Obj shared memory: Total allocated %lu, Used %lu, Free %lu\n",
             (unsigned long)iSharedMmorySize,
             (unsigned long)GetObjTotalSize(),
             (unsigned long)m_stSharedMemory.GetFreeMemorySize());

    Trace();
    return 0;
}

//////////////////////////////////////////////////////////////////////////
// ���½ӿڸ�Ӧ�ó���ʹ��

// ��ѯָ��Uin��Ȩ����Ϣ
//   ����: rstUinPrivInfo.m_uiUin ��ѯUin
//         rstUinPrivInfo.m_uiPriv ����Ȩ��ֵ
//   ����ֵ: 0 - �ɹ�, ��ֵʧ��
TUinPrivInfo* CUinPrivManager::GetUinPrivInfo(unsigned int uiUin, bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    TUinPrivInfo stUinPrivInfo;
    stUinPrivInfo.m_uiUin = uiUin;
    return (TUinPrivInfo*)bsearch(
               &stUinPrivInfo,
               pstController->m_pstCache,
               *pstController->m_piNumber,
               sizeof(TUinPrivInfo),
               CompareUinPrivInfo);
}


// ��ӡ
void CUinPrivManager::Trace()
{
    TRACESVR("Active is %d.\n", *m_piActiveController);

    int i = 0;
    for (i = 0; i < 2; i++)
    {
        int k = 0;
        for (k = 0; k < *m_astUinPrivControllers[i].m_piNumber; k++)
        {
            //TRACESVR("%d:%d = %d.\n", i, k, m_astUinPrivControllers[i].m_pstCache[k].m_uiUin);
        }
    }

    return;
}

//////////////////////////////////////////////////////////////////////////
// ���½ӿڸ�ServerUinPrivServerʹ��

// ����ActiveController
void CUinPrivManager::SwitchActiveCache()
{
    *m_piActiveController = (*m_piActiveController + 1) % 2;
}

// ��ձ��ݻ�����
int CUinPrivManager::CleanBackupCache()
{
    TUinPrivController* pstBackupController = &m_astUinPrivControllers[1 - *m_piActiveController];
    *pstBackupController->m_piNumber = 0;
    return 0;
}

int CUinPrivManager::InsertUinPrivInfo(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    if (*pstController->m_piNumber >= m_iMaxUinNumber)
    {
        return -1;
    }

    // ���Ȩ��Ϊ��, ��ɾ��Ȩ����Ϣ
    if (rstUinPrivInfo.m_uiPriv == 0)
    {
        return DeleteUinPrivInfo(rstUinPrivInfo.m_uiUin, bActiveCache);
    }

    // �������е�Ȩ����Ϣ
    TUinPrivInfo* pstUinPrivInfo = GetUinPrivInfo(rstUinPrivInfo.m_uiUin, bActiveCache);
    if (pstUinPrivInfo)
    {
        pstUinPrivInfo->m_uiPriv = rstUinPrivInfo.m_uiPriv;
        return 0;
    }

    // ����µ�Ȩ����Ϣ
    pstController->m_pstCache[*pstController->m_piNumber] = rstUinPrivInfo;
    (*pstController->m_piNumber)++;

    qsort(pstController->m_pstCache, *pstController->m_piNumber, sizeof(TUinPrivInfo), CompareUinPrivInfo);

    return 0;
}


int CUinPrivManager::InsertUinPrivInfoWithoutSort(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    if (*pstController->m_piNumber >= m_iMaxUinNumber)
    {
        return -1;
    }

    // ����µ�Ȩ����Ϣ
    pstController->m_pstCache[*pstController->m_piNumber] = rstUinPrivInfo;
    (*pstController->m_piNumber)++;

    return 0;
}

// �������
void CUinPrivManager::SortUinPrivInfo(bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    qsort(pstController->m_pstCache, *pstController->m_piNumber, sizeof(TUinPrivInfo), CompareUinPrivInfo);
}

//ɾ�����󻹱���ɾ��Hash
int CUinPrivManager::DeleteUinPrivInfo(const unsigned int uiUin, bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    TUinPrivInfo* pstUinPrivInfo = GetUinPrivInfo(uiUin, bActiveCache);
    if (!pstUinPrivInfo)
    {
        return 0;
    }

    int iStart = pstUinPrivInfo - pstController->m_pstCache;
    for (int i = iStart; i < m_iMaxUinNumber - 1; i++)
    {
        pstController->m_pstCache[i] = pstController->m_pstCache[i+1];
    }

    (*pstController->m_piNumber)--;

    return 0;
}


// ��ȡ����
int CUinPrivManager::GetUinPrivNumber(bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    return *(pstController->m_piNumber);
}

// ��ȡȨ��
const TUinPrivInfo* CUinPrivManager::GetUinPrivInfoByIndex(int iIndex, bool bActiveCache)
{
    TUinPrivController* pstController = NULL;;

    if (bActiveCache)
    {
        pstController = &m_astUinPrivControllers[*m_piActiveController];
    }
    else
    {
        pstController = &m_astUinPrivControllers[1 - *m_piActiveController];
    }

    if (iIndex < 0 || iIndex >= *(pstController->m_piNumber))
    {
        return NULL;
    }

    return &(pstController->m_pstCache[iIndex]);
}
