#ifndef __UINPRV_MANAGER_HPP__
#define __UINPRV_MANAGER_HPP__

// UinȨ�޹����

#include "SharedMemory.hpp"

// Ȩ����Ϣ
typedef struct tagUinPrivInfo
{
    unsigned int m_uiUin;                                    	/*   uin */
    unsigned int m_uiPriv;                                   	/*   Ȩ�� */
} TUinPrivInfo;

// Ȩ�޹�����, �������ڴ��е�Ȩ�޶���
// ͬʱ��Ӧ�ó���͸��½���ʹ��

typedef struct tagUinPrivController
{
    int* m_piNumber;
    TUinPrivInfo* m_pstCache;
} TUinPrivController;

class CUinPrivManager
{
public:

    // ��ʼ�������
    //   ����: pszShmName �����ڴ�Key
    int Initialize(int iUinNumber, const char* pszShmName, bool bIsUinPrivServer = false);

    // ��ѯָ��Uin��Ȩ����Ϣ
    //   uiUin:  ��ѯUin
    //   bActiveCache: ��ѯ���������� (Ĭ����)
    //   ����ֵ: 0 - �ɹ�, ��ֵʧ��
    TUinPrivInfo* GetUinPrivInfo(unsigned int uiUin, bool bActiveCache = true);

    // ��ӡ
    void Trace();
public:
    //////////////////////////////////////////////////////////////////////////
    // ���½ӿڸ�UinPrivServerʹ��

    // �л�����������
    void SwitchActiveCache();

    // ��ձ��ݻ�����
    int CleanBackupCache();

    // ��������
    // bActiveCache: �Ƿ���������������
    int InsertUinPrivInfo(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache);

    // ɾ������
    // bActiveCache: �Ƿ�����������ɾ��
    int DeleteUinPrivInfo(const unsigned int uiUin, bool bActiveCache);

    // ��Ӷ��� ������
    // bActiveCache: �Ƿ���������������
    int InsertUinPrivInfoWithoutSort(const TUinPrivInfo& rstUinPrivInfo, bool bActiveCache);

    // �������
    void SortUinPrivInfo(bool bActiveCache);

    // ��ȡ����
    int GetUinPrivNumber(bool bActiveCache);

    // ��ȡȨ��
    const TUinPrivInfo* GetUinPrivInfoByIndex(int iIndex, bool bActiveCache);

private:
    //////////////////////////////////////////////////////////////////////////
    // �ڲ��ӿ�
    size_t GetObjTotalSize();

private:
    // �����ڴ�
    CSharedMemory m_stSharedMemory;
    int m_iMaxUinNumber;
    size_t m_iObjTotalSize;

    // ����������������
    TUinPrivController m_astUinPrivControllers[2];

    // ������ʶ
    int* m_piActiveController;
};

#endif
