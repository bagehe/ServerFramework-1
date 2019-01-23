/**
*@file TempMemoryMng.hpp
*@author jasonxiong
*@date 2009-11-28
*@version 1.0
*@brief ��ʱ�ڴ���������
*
*	ע�⣺
*	��1����֧�ֶ���̻���߳�
*	��2�������ڴ���ڲ��õ�ʱ����Ҫ����DestroyTempMem�������ڴ�
*/

#ifndef __TEMP_MEMORY_MNG_HPP__
#define __TEMP_MEMORY_MNG_HPP__

#include <stdio.h>

namespace ServerLib
{

typedef enum enmTempMemAllocType
{
    ETMT_ALLOC_BY_MEMORY_MNG = 0, //!<��CTempMemoryMng���з���
    ETMT_ALLOC_BY_SHARED_MEMEORY = 1, //!<�ɹ����ڴ洴��
} ENMTEMPMEMALLOCTYPE;

typedef struct tagTempMemoryObj
{
    int m_iNexIndex; //!<��һ����ʱ�ڴ����������
    unsigned int m_uiMemOffset; //!<��������ʱ�ڴ��е�ƫ����
} TTempMemoryObj;

class CTempMemoryMng
{
private:
    CTempMemoryMng();
    //void* operator new(unsigned int uiSize, const void* pThis) throw();

public:
    /**
    *������ʱ�ڴ������
    *@param[in] iObjSize ÿ����ʱ������С
    *@param[in] iMaxObjCount ������ʱ��������
    *@return 0 success
    */
    CTempMemoryMng(int iObjSize, int iMaxObjCount);
    ~CTempMemoryMng();

    /**
    *ͨ�������ڴ�������CTempMemoryMng��ע������������CTempMemoryMng�����ʼ��������ı����ڹ����ڴ��ֵ��
    *@param[in] pszKeyFileName �����ڴ�Attach���ļ���
    *@param[in] ucKeyPrjID �����ڴ��ProjectID
    *@param[in] iObjSize ÿ����ʱ������С
    *@param[in] iMaxObjCount ������ʱ��������
    *@return 0 success
    */
    static CTempMemoryMng* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            int iObjSize, int iMaxObjCount);

public:
    /**
    *��ʼ����ʱ�ڴ������
    *@return 0 success
    */
    int Initialize();

    //!������ʱ�ڴ�
    void* CreateTempMem();

    //!������ʱ�ڴ�
    int DestroyTempMem(void *pTempMem);

    //!��ȡ���ö������
    int	GetUsedCount() const;

    //!��ȡ���ж������
    int	GetFreeCount() const;

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    int m_iErrorNO; //!������
    int m_iMemAllocType; //!<�ڴ�������ͣ�0��CTempMemoryMng���䣬1��ʹ����ָ��
    unsigned char* m_aucObjMem; //!<����Ķ����ڴ�
    int m_iObjSize; //!<ÿ���������ڴ��С
    int m_iMaxObjCount; //!<������ٸ������
    int m_iUsedCount; //!<�Ѿ�ʹ�õ��ڴ�����
    int m_iFirstFreeObjIdx; //!<������ʱ�ڴ���׶�������
    TTempMemoryObj* m_astTmpMemObj; //!<��ʱ�ڴ��������
};

}

#endif //__TEMP_MEMORY_MNG_HPP__
///:~
