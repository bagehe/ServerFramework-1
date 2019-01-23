#include "GameProtocol.hpp"
#include "ObjAllocator.hpp"
#include "AppLoopW.hpp"


#include "WorldObjectAllocatorW.hpp"

template <typename TYPE_Obj>
CObjAllocator* AllocateShmObj(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
    CObjAllocator* pAllocator = NULL;

    if (!bResume)
    {
        pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        TRACESVR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // �ָ�ʹ���е�obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

    int iRet = WorldType<TYPE_Obj>::RegisterAllocator(pAllocator);
    if(iRet < 0)
    {
        return NULL;
    }

    return pAllocator;
}

template <typename TYPE_Obj>
CObjAllocator* AllocateShmObjK32(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
    CObjAllocator* pAllocator = NULL;

    if (!bResume)
    {
        pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        TRACESVR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // �ָ�ʹ���е�obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

    CHashMap_K32* pHashMap = NULL; //�ڵ�����Ϊ������������

    if (!bResume)
    {
        pHashMap = CHashMap_K32::CreateHashMap(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }
    else
    {
        pHashMap = CHashMap_K32::ResumeHashMap(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }

    if (!pHashMap)
    {
        TRACESVR("Failed to allocate hashmap\n");
        return NULL;
    }

    rstSharedMemory.UseShmBlock(CHashMap_K32::CountSize(iObjCount << 1));

    int iRet = WorldTypeK32<TYPE_Obj>::RegisterHashAllocator(pAllocator, pHashMap);
    if(iRet < 0)
    {
        return NULL;
    }

    return pAllocator;
}

template <typename TYPE_Obj>
CObjAllocator* AllocateShmObjK64(CSharedMemory& rstSharedMemory, const int iObjCount, bool bResume)
{
    CObjAllocator* pAllocator = NULL;

    if (!bResume)
    {
        pAllocator = CObjAllocator::CreateByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }
    else
    {
        pAllocator = CObjAllocator::ResumeByGivenMemory(
            (char*)rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            sizeof(TYPE_Obj),
            iObjCount,
            TYPE_Obj::CreateObject);
    }

    if (!pAllocator)
    {
        TRACESVR("Failed to allocate object segment\n");
        return NULL;
    }

    if (bResume)
    {
        // �ָ�ʹ���е�obj
        int iUsedIdx = pAllocator->GetUsedHead();
        while (iUsedIdx != -1)
        {
            CObj* pObj = pAllocator->GetObj(iUsedIdx);
            pObj->Resume();
            iUsedIdx = pAllocator->GetIdx(iUsedIdx)->GetNextIdx();
        }
    }

    rstSharedMemory.UseShmBlock(CObjAllocator::CountSize(sizeof(TYPE_Obj), iObjCount));

    CHashMap_K64* pHashMap = NULL; //�ڵ�����Ϊ������������

    if (!bResume)
    {
        pHashMap = CHashMap_K64::CreateHashMap(
            (char*) rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }
    else
    {
        pHashMap = CHashMap_K64::ResumeHashMap(
            (char*) rstSharedMemory.GetFreeMemoryAddress(),
            rstSharedMemory.GetFreeMemorySize(),
            iObjCount << 1);
    }

    if (!pHashMap)
    {
        TRACESVR("Failed to allocate hashmap\n");
        return NULL;
    }

    rstSharedMemory.UseShmBlock(CHashMap_K64::CountSize(iObjCount << 1));

    int iRet = WorldTypeK64<TYPE_Obj>::RegisterHashAllocator(pAllocator, pHashMap);
    if(iRet < 0)
    {
        return NULL;
    }

    return pAllocator;
}

CWorldObjectAllocatorW::CWorldObjectAllocatorW()
{
    m_iObjTotalSize = 0;
}

int CWorldObjectAllocatorW::Initialize(bool bResume)
{
	int iRet = -1;

	CObjAllocator* pAllocator = NULL;

    size_t iSharedMmorySize = GetObjTotalSize();
	iRet = m_stSharedMemory.CreateShmSegmentByKey(CSharedMemory::GenerateShmKey(GAME_SERVER_WORLD, CModuleHelper::GetWorldID()), iSharedMmorySize);
	if(iRet < 0)
	{
		return -1;
	}

	pAllocator = AllocateShmObjK32<CWorldRoleStatusWObj>(m_stSharedMemory, MAX_ROLE_OBJECT_NUMBER_IN_WORLD, bResume);
	if(!pAllocator)
	{
		return -2;
	}

    TRACESVR("Obj shared memory: Total allocated %ld, Used %ld, Free %ld\n", 
                    iSharedMmorySize,
                    GetObjTotalSize(),
                    m_stSharedMemory.GetFreeMemorySize());

	return 0;
}

size_t CWorldObjectAllocatorW::GetObjTotalSize()
{

	if (m_iObjTotalSize > 0)
	{
		return m_iObjTotalSize;
	}

    // uin��Ϊ��������Ҷ���
	size_t iRoleSize = CObjAllocator::CountSize(sizeof(CWorldRoleStatusWObj), MAX_ROLE_OBJECT_NUMBER_IN_WORLD); 
    m_iObjTotalSize += iRoleSize;

	size_t iRoleHashSize = CHashMap_K32::CountSize(MAX_ROLE_OBJECT_NUMBER_IN_WORLD << 1);
    m_iObjTotalSize += iRoleHashSize;

	return m_iObjTotalSize;
}

