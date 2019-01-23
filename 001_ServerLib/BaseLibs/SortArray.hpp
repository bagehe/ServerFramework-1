
#ifndef __ARRAY_SORT_HPP__
#define __ARRAY_SORT_HPP__

#include <stdlib.h>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���������㷨, �ο��Ի������
// �����˳��Ϊ�� compar ����ֵ��������. ��:
// compar(1, 2): ����-1, ���С��������
// compar(1, 2): ����1, ��Ӵ�С����

template <int offset, typename T>
bool CheckZero(const void *p1)
{
    T a1 = *(T *)((char *)p1 + offset);
    return (bool)a1;
}

#define check_zero(s, m) CheckZero<offsetof(s, m), typeof(((s*)0)->m)>

template <class T>
int Cmp(const void *a1, const void *a2)
{
    T *t1 = (T*)a1;
    T *t2 = (T*)a2;
    if (*t1 < *t2)
    {
        return -1;
    }
    else if (*t1 > *t2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

template <class T>
int DescCmp(const void *a1, const void *a2)
{
    return -Cmp<T>(a1, a2);
}

template <int offset, typename T>
int CmpOneKey(const void *p1, const void *p2)
{
    T a1 = *(T *)((char *)p1 + offset);
    T a2 = *(T *)((char *)p2 + offset);
    if (a1 < a2)
    {
        return -1;
    }
    else if (a1 > a2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �Ƚ϶����ĳ����Ա,������MyDeleteArrayElement�ĺ���ָ��
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <int offset, typename T>
int CmpOneMem(const void *pObj, const void *pMem)
{
    T a1 = *(T *)((char *)pObj + offset);
    T a2 = *(T *)(pMem);
    if (a1 < a2)
    {
        return -1;
    }
    else if (a1 > a2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


template <int offset, typename T>
int DescCmpOneKey(const void *p1, const void *p2)
{
    return -CmpOneKey<offset, T>(p1, p2);
}

template <int offset1, typename T1, int offset2, typename T2>
int CmpTwoKey(const void *p1, const void *p2)
{
    int diff = CmpOneKey<offset1, T1>(p1, p2);
    if (diff == 0)
    {
        return CmpOneKey<offset2, T2>(p1, p2);
    }
    return diff;
}

template <int offset1, typename T1, int offset2, typename T2>
int DescCmpTwoKey(const void *p1, const void *p2)
{
    return -CmpTwoKey<offset1, T1, offset2, T2>(p1, p2);
}

#define cmp_one_key(s, m) CmpOneKey<offsetof(s, m), typeof(((s*)0)->m)>
#define cmp_two_key(s, m1, m2) CmpTwoKey<offsetof(s, m1), typeof(((s*)0)->m1), offsetof(s, m2), typeof(((s*)0)->m2)>

#define cmp_one_key_desc(s, m) DescCmpOneKey<offsetof(s, m), typeof(((s*)0)->m)>
#define cmp_two_key_desc(s, m1, m2) DescCmpTwoKey<offsetof(s, m1), typeof(((s*)0)->m1), offsetof(s, m2), typeof(((s*)0)->m2)>

#define cmp_one_mem(s, m) CmpOneMem<offsetof(s, m), typeof(((s*)0)->m)>

typedef int (*MYBCOMPAREFPTR) (const void *, const void *);

// ����һ��ֵΪkey��Ԫ��
// ����ֵ: piEqualΪ1ʱ, �����ҵ�����key������, piEqualΪ0ʱ, �ҵ���һ��С��key��ֵ
int MyBSearch (const void *key, const void *base, int nmemb, int size, int *piEqual, MYBCOMPAREFPTR compar);

// ����һ��ֵΪkey��Ԫ��
// iUnique: 1Ϊ�������ظ�, 0 - Ϊ�����ظ�
// ����ֵ: -1 ʧ��, >=0 �ɹ�
int MyBInsert (const void *key, const void *base, int *pnmemb, int size, int iUnique, MYBCOMPAREFPTR compar);

// ɾ������Ϊindex��Ԫ��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyIDelete (const void *base, int *pnmemb, int size, int index);

// ɾ��ֵΪkey��Ԫ��
// ����ֵ: -1 ʧ��, 0 �ɹ�
int MyBDelete (const void *key, const void *base, int *pnmemb, int size, MYBCOMPAREFPTR compar);


// ����һ��ֵΪkey��Ԫ��
// iUnique: 1Ϊ�������ظ�, 0 - Ϊ�����ظ�
// ����ֵ: -1 ʧ��, >=0 �ɹ�
int MyBInsert2 (const void *key, const void *base, int *pnmemb, int size, int iUnique, MYBCOMPAREFPTR compar);

int MyBDelete2 (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *));


// ���²����ʺ������
// Ԫ������Ϊkey����Ԫ�ز��Һ�ɾ��ʱ���������objcmp�ıȽϽ�����жϡ�һ��key���ܶ�Ӧ���Ԫ�ء���ЩԪ��û������Ƚϣ��������Ⱥ�˳��������
int MyBSearchDup (const void *key, const void *base, int nmemb, int size, int *piEqual,  MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);
int MyBInsertDup (const void *key, const void *base, int *pnmemb, int size, int iUnique, bool bTail, MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);
int MyBDeleteDup (const void *key, const void *base, int *pnmemb, int size,  MYBCOMPAREFPTR compar, MYBCOMPAREFPTR objcmp);


// ����ɾ��ָ����Աֵ��ʣ�µ����飨����Ԫ�ظ���
int MyDeleteArrayElement (const void *key, const void *base, int *pnmemb, int size, int (*compar) (const void *, const void *));

int MyDeleteArray_If(const void *base, int *pnmemb, int iSize, bool (*IfPred)(const void *));

#endif
