/**
*@file ShmObjectCreator.hpp
*@author jasonxiong
*@date 2009-11-03
*@version 1.0
*@brief CShmObjectCreatorģ����Ķ���
*
*	�Թ����ڴ�CSharedMemory��ʹ�ü򵥷�װ����һ����С�̶������ù����ڴ����
*/

#ifndef __SHM_OBJECT_CREATOR_HPP__
#define __SHM_OBJECT_CREATOR_HPP__

#include <new>

#include "SharedMemory.hpp"



namespace ServerLib
{

template <class TYPE_ShmObject>
class CShmObjectCreator
{
public:

    /**
    *�ù����ڴ洴������
    *@param[in] pstSharedMemory ʹ�õĹ����ڴ��࣬���һ���ڴ���һ���࣬��ֹ�����ڴ�й©
    *@param[in] pszKeyFileName �����ڴ�������ļ���
    *@param[in] ucKeyPrjID �����ڴ������ID
    *@return 0 success
    */
    static TYPE_ShmObject* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID)
    {
        int iRet = 0;
        iRet = pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, sizeof(TYPE_ShmObject));
        if (iRet < 0)
        {
            printf("CreateShmSegment failed, iRet=%d\n ", iRet);
            return NULL;
        }
        TYPE_ShmObject* pTYPE_ShmObject = NULL;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject();
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE_ShmObject));
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    static TYPE_ShmObject* CreateObjectByKey(
        CSharedMemory* pstSharedMemory,
        key_t uiShmKey)
    {
        int iRet = 0;
        iRet = pstSharedMemory->CreateShmSegmentByKey(uiShmKey, sizeof(TYPE_ShmObject));
        if (iRet < 0)
        {
            printf("CreateShmSegment failed, iRet=%d\n ", iRet);
            return NULL;
        }
        TYPE_ShmObject* pTYPE_ShmObject = NULL;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject();
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE_ShmObject));
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    /**
    *�ù����ڴ洴�����󣬲��������һ����С
    *@param[in] pstSharedMemory ʹ�õĹ����ڴ��࣬���һ���ڴ���һ���࣬��ֹ�����ڴ�й©
    *@param[in] pszKeyFileName �����ڴ�������ļ���
    *@param[in] ucKeyPrjID �����ڴ������ID
    *@param[in] iAddistionalSize ���������ڴ��С
    *@return 0 success
    */
    static TYPE_ShmObject* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID,
        const int iAddistionalSize)
    {
        int iRet = 0;
        int iSharedMemorySize = sizeof(TYPE_ShmObject) + iAddistionalSize;
        pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, iSharedMemorySize);
        TYPE_ShmObject* pTYPE_ShmObject = NULL;;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject(iAddistionalSize);
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(iSharedMemorySize);
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

    static TYPE_ShmObject* CreateObjectByKey(
        CSharedMemory* pstSharedMemory,
        key_t uiShmKey,
        const int iAddistionalSize)
    {
        int iRet = 0;
        int iSharedMemorySize = sizeof(TYPE_ShmObject) + iAddistionalSize;
        pstSharedMemory->CreateShmSegmentByKey(uiShmKey, iSharedMemorySize);
        TYPE_ShmObject* pTYPE_ShmObject = NULL;;
        pTYPE_ShmObject = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE_ShmObject(iAddistionalSize);
        if(!pTYPE_ShmObject)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(iSharedMemorySize);
        if(iRet < 0)
        {
            return NULL;
        }
        return pTYPE_ShmObject;
    }

};



//////////////////////////////////////////////////////////////////////////

template <class TYPE>
class CShmSingleton
{
public:
    static TYPE* Instance(void)
    {
        if(m_pstInstance == NULL)
        {
            //TBD
            if(m_pszMemory == NULL)
            {
                m_pstInstance = new TYPE;
            }
            else
            {
                m_pstInstance = new(m_pszMemory) TYPE;
            }
        }
        return m_pstInstance;
    }

    static TYPE* CreateObject(
        CSharedMemory* pstSharedMemory,
        const char* pszKeyFileName,
        const unsigned char ucKeyPrjID)
    {
        int iRet = 0;
        pstSharedMemory->CreateShmSegment(pszKeyFileName, ucKeyPrjID, sizeof(TYPE));
        m_pstInstance = new((void*)pstSharedMemory->GetFreeMemoryAddress()) TYPE;
        if(!m_pstInstance)
        {
            return NULL;
        }
        iRet = pstSharedMemory->UseShmBlock(sizeof(TYPE));
        if(iRet < 0)
        {
            return NULL;
        }
        return m_pstInstance;
    }
protected:
    CShmSingleton()
    {

    }
protected:
    static TYPE* m_pstInstance;
    static char* m_pszMemory;

};

template <class TYPE>
TYPE* CShmSingleton<TYPE>::m_pstInstance = NULL;

template <class TYPE>
char* CShmSingleton<TYPE>::m_pszMemory = NULL;

}

#endif
