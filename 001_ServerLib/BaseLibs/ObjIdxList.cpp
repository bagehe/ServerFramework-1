#include "ObjIdxList.hpp"
#include "LogAdapter.hpp"

// ���ڵ����������
int AddObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode)
{
    // ����ظ�Ԫ�غ���ЧԪ��
    int iPreIdx = -1;
    TObjIdxListNode* pPreListNode = NULL;
    int iCurIdx = rstListHead.m_iFirstIdx;
    int iListNodeNumber = 0;

    while (iCurIdx >= 0)
    {
        // �ظ�Ԫ�أ����سɹ�
        if (iCurIdx == iIdx)
        {
            //ERRORLOG("Duplicate CurIdx: iCurIdx = %d, idx = %d\n", iCurIdx, iIdx);
            return 0;
        }

        TObjIdxListNode* pCurListNode = pGetObjListNode(iCurIdx);

        // ��ЧԪ�أ��ض��б�
        if (!pCurListNode)
        {
            //ERRORLOG("Invalid CurIdx: iCurIdx = %d, idx = %d\n", iCurIdx, iIdx);

            if (pPreListNode)
            {
                pPreListNode->m_iNextIdx = -1;
                rstListHead.m_iNumber = iListNodeNumber;
                rstListHead.m_iLastIdx = iPreIdx;
            }
            else
            {
                rstListHead.Initialize();
            }

            break;
        }

        iPreIdx = iCurIdx;
        pPreListNode = pCurListNode;
        iCurIdx = pCurListNode->m_iNextIdx;
        iListNodeNumber++;
    }

    TObjIdxListNode* pCurListNode = pGetObjListNode(iIdx);
    if (!pCurListNode)
    {
        //ERRORLOG("Invalid Idx: idx = %d\n", iIdx);
        return -3;
    }

    // ��������β��
    TObjIdxListNode* pLastListNode = pGetObjListNode(rstListHead.m_iLastIdx);
    if (pLastListNode)
    {
        pLastListNode->m_iNextIdx = iIdx;
    }

    pCurListNode->m_iPrevIdx = rstListHead.m_iLastIdx;
    pCurListNode->m_iNextIdx = -1;

    rstListHead.m_iLastIdx = iIdx;

    // ��������ͷ��
    if (rstListHead.m_iFirstIdx < 0)
    {
        rstListHead.m_iFirstIdx = iIdx;
    }

    rstListHead.m_iNumber++;

    return 0;
}

// ���ڵ��������ɾ��
int DelObjIdxList(TObjIdxListHead& rstListHead, int iIdx, GETOBJLISTNODE* pGetObjListNode)
{
    TObjIdxListNode* pCurListNode = pGetObjListNode(iIdx);
    ASSERT_AND_LOG_RTN_INT(pCurListNode);

    int iPrevIdx = pCurListNode->m_iPrevIdx;
    int iNextIdx = pCurListNode->m_iNextIdx;

    TObjIdxListNode* pPrevListNode = pGetObjListNode(iPrevIdx);
    TObjIdxListNode* pNextListNode = pGetObjListNode(iNextIdx);

    bool bDeleted = false;

    // ��������ɾ��
    if (pPrevListNode)
    {
        pPrevListNode->m_iNextIdx = iNextIdx;
        bDeleted = true;
    }

    if (pNextListNode)
    {
        pNextListNode->m_iPrevIdx = iPrevIdx;
        bDeleted = true;
    }

    // ��������β��
    if (rstListHead.m_iLastIdx == iIdx)
    {
        rstListHead.m_iLastIdx = iPrevIdx;

        if (pPrevListNode)
        {
            pPrevListNode->m_iNextIdx = -1;
        }

        bDeleted = true;
    }

    // ��������ͷ��
    if (rstListHead.m_iFirstIdx == iIdx)
    {
        rstListHead.m_iFirstIdx = iNextIdx;

        if (pNextListNode)
        {
            pNextListNode->m_iPrevIdx = -1;
        }

        bDeleted = true;
    }

    pCurListNode->m_iPrevIdx = -1;
    pCurListNode->m_iNextIdx = -1;

    if (bDeleted)
    {
        rstListHead.m_iNumber--;
    }

    return 0;
}


