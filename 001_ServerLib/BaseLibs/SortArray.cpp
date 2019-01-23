
#include "SortArray.hpp"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������㷨, �ο��Ի������
// �����˳��Ϊ�� compar ����ֵ��������. ��:
// compar(1, 2): ����-1, ���С��������
// compar(1, 2): ����1, ��Ӵ�С����

// ����һ��ֵΪkey��Ԫ��
// ����: piEqualΪ1ʱ, �����ҵ�����key������, piEqualΪ0ʱ, �ҵ���һ��С��key��ֵ
int MyBSearch (const void *key, const void *base, int nmemb, int size, int *piEqual, int (*compar) (const void *, const void *))
{
    int l, u, idx;
    const void *p, *p2;
    int comparison, comparison2;

    *piEqual = 0;
    if (!nmemb) return 0;
    l = 0;
    u = nmemb;

    while (l < u)
    {
        idx = (l + u) / 2;
        p = (void *) (((const char *) base) + (idx * size));
        comparison = (*compar) (key, p);

        if (comparison == 0)
        {
            *piEqual = 1;
            return idx;
        }
        else if (comparison < 0)
        {
            if (idx == 0) return idx;

            p2 = (void *) (((const char *) base) + ((idx - 1) * size));
            comparison2 = (*compar) (key, p2);

            if (comparison2 > 0) return idx;

            u = idx;
        }
        else /*if (comparison > 0)*/
        {
            l = idx + 1;
        }
    }

    return u;
}

// ����һ��ֵΪkey��Ԫ��
// iUnique: 1Ϊ�������ظ�, 0 - Ϊ�����ظ�
// ����ֵ: -1 ʧ��, >=0 �ɹ�
int MyBInsert (const void *key, const void *base, int *pnmemb, int size, int iUnique, int (*compar) (const void *, const void *))
{
    int i, iInsert, iEqu;

    iInsert = MyBSearch(key, base, *pnmemb, size, &iEqu, compar);
    if (iEqu && iUnique) return -1;
    if (iInsert < *pnmemb)
        for (i = *pnmemb; i > iInsert; i--) memcpy((char *)base + i * size, (char *)base + (i - 1)*size, size);
    memcpy((char *)base + iInsert*size, key, size);
    (*pnmemb)++;
    return iInsert;
}

// ɾ������Ϊindex��Ԫ��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyIDelete (const void *base, int *pnmemb, int size, int index)
{
    int i;

    if (0 > index || index >= *pnmemb)
        return -1;

    for (i = index; i < (*pnmemb) - 1; i++) memcpy((char *)base + i * size, (char *)base + (i + 1)*size, size);
    (*pnmemb)--;

    return 0;
}

// ɾ��ֵΪkey��Ԫ��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyBDelete (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *))
{
    int i, iDelete, iEqu;

    iDelete = MyBSearch(key, base, *pnmemb, size, &iEqu, compar);
    if (!iEqu) return -1;
    if (iDelete < *pnmemb)
        for (i = iDelete; i < (*pnmemb) - 1; i++) memcpy((char *)base + i * size, (char *)base + (i + 1)*size, size);
    (*pnmemb)--;
    return 0;
}


// ����һ��ֵΪkey��Ԫ��
// �ƶ�Ԫ��ʹ��memmove�����Ч��
// iUnique: 1Ϊ�������ظ�, 0 - Ϊ�����ظ�
// ����ֵ: -1 ʧ��, >=0 �ɹ�
int MyBInsert2 (const void *key, const void *base, int *pnmemb, int size, int iUnique, int (*compar) (const void *, const void *))
{
    int iInsert, iEqu;

    iInsert = MyBSearch(key, base, *pnmemb, size, &iEqu, compar);

    if (iEqu && iUnique)
    {
        return -1;
    }

    if (iInsert < (*pnmemb))
    {
        memmove((char*)base + (iInsert+1)*size, (char*)base + iInsert*size, (*pnmemb-iInsert)*size);
    }

    memcpy((char*)base + iInsert* size, key, size);
    ++(*pnmemb);

    return iInsert;

}


// ɾ��ֵΪkey��Ԫ��
// �ƶ�Ԫ��ʹ��memmove�����Ч��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyBDelete2 (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *))
{
    int iDelete, iEqu;

    iDelete = MyBSearch(key, base, *pnmemb, size, &iEqu, compar);
    if (!iEqu)
    {
        return -1;
    }

    if (iDelete < *pnmemb)
    {
        memmove((char*)base + iDelete * size, (char *)base + (iDelete + 1)*size,  (*pnmemb - iDelete - 1)*size);
    }
    (*pnmemb)--;

    return 0;
}

// ����һ��ֵΪkey��Ԫ�أ���Ҫ���Ԫ��ͨ��objcmp�Ƚ���ȡ���Ԫ����keyΪ����ֵ������ͬkeyֵ�����ظ�
// ����: piEqualΪ1ʱ, �����ҵ�����key������, piEqualΪ0ʱ, �ҵ���һ��С��key��ֵ
// key��ͬʱ��ͨ��objcmp���Ƚ��������ͬ
int MyBSearchDup (const void *key, const void *base, int nmemb, int size, int *piEqual, int (*compar) (const void *, const void *), int (*objcmp)(const void *, const void *))
{
    // ���ҵ�һ����ͬkey�� Ȼ������ǰ���Ѱ����ͬobj
    int u = MyBSearch(key, base, nmemb, size, piEqual, compar);

    if (*piEqual == 1)
    {
        *piEqual = 0;
        for (int i = u; i >= 0; --i)
        {
            void *pobj = (void *) (((const char *) base) + (i * size));
            int keycomparison = (*compar)(key, pobj);
            if (keycomparison != 0)
            {
                break;
            }

            int objcmpariosn = (*objcmp)(key, pobj);
            if (objcmpariosn == 0)
            {
                *piEqual = 1;
                return i;
            }
        }
        for (int i = u; i < nmemb; ++i)
        {
            void *pobj = (void *) (((const char *) base) + (i * size));
            int keycomparison = (*compar)(key, pobj);
            if (keycomparison != 0)
            {
                break;
            }

            int objcmpariosn = (*objcmp)(key, pobj);
            if (objcmpariosn == 0)
            {
                *piEqual = 1;
                return i;
            }
        }
    }

    return u;
}

// ����һ��ֵΪkey��Ԫ�أ���Ҫ���Ԫ��ͨ��objcmp�Ƚ���ȡ���Ԫ����keyΪ����ֵ������ͬkeyֵ�����ظ�
// �ƶ�Ԫ��ʹ��memmove�����Ч��
// iUnique: 1Ϊ�������ظ�, 0 - Ϊ�����ظ�
// bTail: trueΪ�ӵ���ͬkey��� falseΪ�ӵ���ǰ
// ����ֵ: -1 ʧ��, >=0 �ɹ�
int MyBInsertDup (const void *key, const void *base, int *pnmemb, int size, int iUnique, bool bTail, int (*compar) (const void *, const void *),  int (*objcmp)(const void *, const void *))
{
    int iInsert, iEqu;

    iInsert = MyBSearch(key, base, *pnmemb, size, &iEqu, compar);

    if (iEqu)
    {
        while(iInsert >= 0 && iInsert < *pnmemb)
        {
            void *pobj = (void *) (((const char *) base) + (iInsert * size));
            int keycomparison = (*compar)(key, pobj);
            if (keycomparison != 0)
            {
                break;
            }
            if (iUnique)
            {
                int objcmpariosn = (*objcmp)(key, pobj);
                if (objcmpariosn == 0)
                {
                    return -1;
                }
            }
            bTail ? ++iInsert : --iInsert;
        }
    }

    if (iInsert < 0 || iInsert > *pnmemb)
    {
        return -1;
    }

    if (iInsert < (*pnmemb))
    {
        memmove((char*)base + (iInsert+1)*size, (char*)base + iInsert*size, (*pnmemb-iInsert)*size);
    }

    memcpy((char*)base + iInsert* size, key, size);
    ++(*pnmemb);

    return iInsert;

}


// ɾ��ֵΪkey��Ԫ��
// �ƶ�Ԫ��ʹ��memmove�����Ч��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyBDeleteDup (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *), int (*objcmp)(const void *, const void *))
{
    int iDelete, iEqu;

    iDelete = MyBSearchDup(key, base, *pnmemb, size, &iEqu, compar, objcmp);
    if (!iEqu)
    {
        return -1;
    }


    if (iDelete < *pnmemb)
    {
        memmove((char*)base + iDelete * size, (char *)base + (iDelete + 1)*size,  (*pnmemb - iDelete - 1)*size);
    }
    (*pnmemb)--;

    return 0;
}

// ����ɾ��ָ����Աֵ��ʣ�µ����飨����Ԫ�ظ���
int MyDeleteArrayElement (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *))
{
    int iIdx = 0;
    for (int i=0; i<*pnmemb; ++i)
    {
        void *p = (void *) (((const char *) base) + (i * size));
        if ((*compar)(p, key) != 0)
        {
            if (i != iIdx)
            {
                memcpy((char *)base + iIdx * size, (char *)base + i * size, size);
            }
            iIdx++;
        }
    }

    return iIdx;
}

int MyDeleteArray_If(const void *base, int *pnmemb, int iSize, bool (*IfPred)(const void *))
{
    int iRealIdx = 0;
    for (int i = 0; i < *pnmemb; i++)
    {
        void *p = (void *) (((const char *) base) + (i * iSize));
        if ((*IfPred)(p))
        {
            if (i != iRealIdx)
            {
                memcpy((char *)base + iRealIdx * iSize, (char *)base + i * iSize, iSize);
            }
            iRealIdx++;
        }

    }

    *pnmemb = iRealIdx;
    return iRealIdx;
}
