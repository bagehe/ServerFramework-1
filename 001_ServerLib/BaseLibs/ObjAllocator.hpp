/**
*@file ObjAllocator
*@author jasonxiong
*@date 2009-12-09
*@version 1.0
*@brief CObj��������࣬���°汾��idxobj
*
*	��1��һ�����ڴ��Ͷ�����ڴ����
*/

#ifndef __OBJ_ALLOCATOR_HPP__
#define __OBJ_ALLOCATOR_HPP__

#include <stdio.h>

namespace ServerLib
{

typedef enum enmObjAllocType
{
    EOAT_ALLOC_BY_SELF = 0, //!<�����ڴ���ObjAllocator���Ѷ�̬����
    EOAT_ALLOC_BY_SHARED_MEMORY = 1, //!<�����ڴ��ɹ����ڴ����
} ENMOBJALLOCTYPE;

class CObj;

typedef enum enmIdxUseFlag
{
    EIUF_FREE = 0, //!<�ö���δ��ʹ��
    EIUF_USED = 1, //!<�ö����ѱ�ʹ��
} ENMIDXUSEFLAG;

//!�����࣬����CObjAllocator��ʹ�ã����һ�㲻��
class CIdx
{
public:
    CIdx();
    ~CIdx();

public:
    //!��ʼ������
    int Initialize();

    //!����������Ϊδʹ��
    inline void SetFree()
    {
        m_iUseFlag = EIUF_FREE;
    }

    //!����������Ϊ��ʹ��
    inline void SetUsed()
    {
        m_iUseFlag = EIUF_USED;
    }

    //!�ж϶����Ƿ��ѱ�ʹ��
    inline int	IsUsed() const
    {
        return m_iUseFlag == EIUF_USED;
    }

    //!��ȡ����������һ������
    inline int	GetNextIdx() const
    {
        return m_iNextIdx;
    }

    //!��������������һ������
    inline void SetNextIdx(int iIdx)
    {
        m_iNextIdx = iIdx;
    }

    //!��ȡ����������һ������
    inline int	GetPrevIdx() const
    {
        return m_iPrevIdx;
    }

    //!��������������һ������
    inline void SetPrevIdx(int iIdx)
    {
        m_iPrevIdx = iIdx;
    }

    //!��ȡָ��Ķ���
    inline CObj* GetAttachedObj() const
    {
        return m_pAttachedObj;
    }

    //!����ָ��Ķ���
    inline void SetAttachedObj(CObj *pObj)
    {
        m_pAttachedObj = pObj;
    }

private:
    int	m_iNextIdx;	//!<��������������ָ�룬ָ���һ����/æ����
    int	m_iPrevIdx;	//!<��������������ָ�룬ָ��ǰһ����/æ����
    int	m_iUseFlag; //!<�ö����Ƿ��Ѿ���ʹ�ñ�־
    CObj *m_pAttachedObj; //!<��ָ��Ķ���ָ��
};

typedef CObj* (*Function_CreateObj)(void *);

class CObjAllocator
{
private:
    //Ĭ�Ϲ��캯�����������ϲ����е���
    CObjAllocator();

public:
    CObjAllocator(size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);
    ~CObjAllocator();

    /**
    *ʹ�ù����ڴ洴��ObjAllocator
    *@param[in] pszKeyFileName �����ڴ�Attach���ļ���
    *@param[in] ucKeyPrjID �����ڴ�Ĺ���ID
    *@param[in] uiObjSize �����С
    *@param[in] iObjCount �������
    *@param[in]
    *@return 0 success
    */
    static CObjAllocator* CreateBySharedMemory(const char* pszKeyFileName,
            const unsigned char ucKeyPrjID,
            size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

    /**
    *ָ���ڴ�ָ��������CObjAllocator
    *@param[in] pszMemoryAddress ָ�����ڴ�
    *@param[in] uiMemorySize �ڴ��С
    *@param[in] uiObjSize �����С
    *@param[in] iObjCount �������
    *@param[in] pfCreateObj ����CObj����ĺ�����Ĭ�Ͽ�����DECLARE_DYN�е�CreateObject
    *@return 0 success
    */
    static CObjAllocator* CreateByGivenMemory(char* pszMemoryAddress, size_t uiMemorySize,
            size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

    static size_t CountSize(size_t uiObjSize, int iObjCount);

    static CObjAllocator* ResumeByGivenMemory(char* pszMemoryAddress,
            size_t uiMemorySize, size_t uiObjSize, int iObjCount, Function_CreateObj pfCreateObj);

public:
    //!��ʼ�����������������
    int Initialize();

    //!����һ�����󣬳ɹ����ض���ID��ʧ�ܷ���С��0��ֵ
    int	CreateObject();

    //!����һ������, ��ָ����ID���ɹ����ض���ID��ʧ�ܷ���С��0��ֵ
    int	CreateObjectByID(int iID);

    //!���ݶ���ID����һ�����󣬳ɹ�����0
    int	DestroyObject(int iObjectID);

    //!����ObjectID���ض��󣬱��뱣֤�ö����ѱ�ʹ��
    CObj* GetObj(int iObjectID);

    //!����ObjectID��������
    CIdx* GetIdx(int iObjectID);

    //!��ȡ���ö�������ͷ����
    inline int	GetUsedHead() const
    {
        return m_iUsedHeadIdx;
    }

    //!��ȡ���ж�������ͷ����
    inline int	GetFreeHead() const
    {
        return m_iFreeHeadIdx;
    }

    //��ȡ���ö������
    inline int	GetUsedCount() const
    {
        return m_iUsedCount;
    }

    //��ȡ���ж������
    inline int	GetFreeCount() const
    {
        return m_iObjCount - m_iUsedCount;
    }

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    inline int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    //��ȡ�����������
    inline int GetObjAllocType() const
    {
        return m_shObjAllocType;
    }

    //!��ȡ��һ������
    CObj* GetNextObj(int iObjectID);

    //!���ö����ʼ��ָ��
    inline void SetCreateObjFunc(Function_CreateObj pfCreateObjFunc)
    {
        m_pfCreateObjFunc = pfCreateObjFunc;
    }

private:
    //!���ô����
    inline void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    //�⼸������ֻ���ڹ��캯��ʱȷ�������治�����
    short m_shObjAllocType; //!<�����������
    size_t m_uiObjSize; //!<��������
    int m_iObjCount; //!<������Ķ������
    CIdx* m_astIdxs; //!<�������飬���ڹ����������
    CObj* m_pstObjBuffer; //!<����Ķ����ڴ�
    Function_CreateObj m_pfCreateObjFunc; //!<���ڴ��ϴ���CObj����ĺ�����ÿ��������Ҫ�Լ�ʵ��

    //���µĶ������ģ�����Initialize��ʼ��
    int m_iErrorNO; //!<������
    int	m_iFreeHeadIdx; //!<���ж�������ͷ����
    int m_iFreeTailIdx; //!<���ж�������β����
    int	m_iUsedHeadIdx; //!<���ö�������ͷ����
    int	m_iUsedCount; //!<���ö������
};

//!����������
class CObj
{
public:
    CObj() {}
    virtual ~CObj() {}

public:
    //!����ĳ�ʼ����������CObjAllocator��������ʱ����ã���������һ��Ҫʵ��
    virtual int Initialize() = 0;

    //!��ʾ��������������
    virtual int Show() const
    {
        return 0;
    }

    //!��ȡ����ID
    inline int GetObjectID() const
    {
        return m_iObjectID;
    }

    //!���ö���ID
    inline void SetObjectID(int iObjectID)
    {
        m_iObjectID = iObjectID;
    }

    virtual int Resume()
    {
        return 0;
    }

private:
    int m_iObjectID; //!����ID������CObjAllocator�е������±�

    friend int CObjAllocator::Initialize(); //!<�������������Ҫֱ�Ӹ�ֵm_iObjectID��������Ϊ��Ԫ
};

#define DECLARE_DYN \
	public: \
		void* operator new(size_t uiSize, const void* pThis) throw(); \
		static CObj* CreateObject(void* pMem);


#define IMPLEMENT_DYN(class_name) \
	void* class_name::operator new(size_t uiSize, const void* pThis) throw() \
	{ \
		if(!pThis) \
		{ \
			return NULL; \
		} \
		\
		return (void*)pThis; \
	} \
	\
	CObj* class_name::CreateObject( void *pMem ) \
	{ \
		return (CObj*)new(pMem) class_name; \
	}

}

#endif //__OBJ_ALLOCATOR_HPP__
///:~
