
#include "GameProtocol.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "GameUnitID.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/GameRole.hpp"
#include "TimeUtility.hpp"

#include "Kernel/UnitUtility.hpp"

int CUnitUtility::m_iUpdateRoleIdx = -1;

TUNITINFO* CUnitUtility::GetUnitInfo(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }
	
	if (!GetRoleObj(iUnitID))
	{
		return NULL;
	}

    return pGameUnitID->GetBindUnitInfo();
}

CGameRoleObj* CUnitUtility::GetRoleObj(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return NULL;
    }

    int iObjectIdx = pGameUnitID->GetBindObjectIdx();

    CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(iObjectIdx);

    return pRoleObj;
}

// ��ȡ��һ����ɫ����
int CUnitUtility::IterateRoleIdx()
{
    if (m_iUpdateRoleIdx < 0)
    {
        // ��λ��ʼ��λ
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetUsedHead();
    }
    else
    {
        // ��λ��һ����λ
        m_iUpdateRoleIdx = GameTypeK32<CGameRoleObj>::GetNextIdx(m_iUpdateRoleIdx);
    }

    return m_iUpdateRoleIdx;
}

//����ʱ��ͬ����Ϣ
void CUnitUtility::SendSyncTimeNotify(CGameRoleObj* pstRoleObj, long8 lNowTime)
{
	static GameProtocolMsg stMsg;
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_SYNCTIME_NOTIFY);

	Zone_SyncTime_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_synctime_notify();
	pstNotify->set_lservertime(lNowTime);

	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	return;
}

int CUnitUtility::AllocateUnitID()
{
    int iUnitID = GameType<CGameUnitID>::Create();

    // ��ʹ��0��UnitID
    if (iUnitID == 0)
    {
        iUnitID = AllocateUnitID();
    }

    if (iUnitID < 0)
    {
        TRACESVR("Too many Units in Zone!!\n");
    }

    TRACESVR("Allocate UnitID = %d\n", iUnitID);

    return iUnitID;
}

void CUnitUtility::FreeUnitID(const int iUnitID)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    if (!pGameUnitID)
    {
        return;
    }
	
	if (iUnitID == m_iUpdateRoleIdx)
	{
		IterateRoleIdx();
	}
           

    GameType<CGameUnitID>::Del(iUnitID);

    TRACESVR("Free UnitID = %d\n", iUnitID);
}

// ��Unit��Object
int CUnitUtility::BindUnitToObject(int iUnitID, int iObjectIdx)
{
    CGameUnitID* pGameUnitID = GameType<CGameUnitID>::Get(iUnitID);
    ASSERT_AND_LOG_RTN_INT(pGameUnitID);

    pGameUnitID->BindObject(iObjectIdx);

    return 0;
}

// ����һ����λ
CObj* CUnitUtility::CreateUnit(unsigned int uiKey)
{
    // ������λID
    int iUnitID = CUnitUtility::AllocateUnitID();
    if (iUnitID <= 0)
    {
        TRACESVR("Cannot create role unit, Alloc unitid fail!\n");
        return NULL;
    }

    // �����ʹ�����λ
    CObj* pObj = (CObj*)GameTypeK32<CGameRoleObj>::CreateByKey(uiKey);
	CGameRoleObj* pRoleObj = (CGameRoleObj*)pObj;
	if (pRoleObj)
	{
		pRoleObj->GetRoleInfo().stUnitInfo.iUnitID = iUnitID;
		pRoleObj->SetUin(uiKey);
	}

    if (pObj)
    {
        CUnitUtility::BindUnitToObject(iUnitID, pObj->GetObjectID());
    }
    else
    {
        CUnitUtility::FreeUnitID(iUnitID);
        TRACESVR("Cannot create role unit, Alloc obj fail!\n");
    }

    return pObj;
}

// ɾ��һ����λ, �ͷ�UnitID, ���ٶ���ID.
// ����ɾ����һ���ǳ�Σ�յ���Ϊ, ���Ҫ�Ƚ���λ��ΪEUS_DELETED״̬, ��AppTick��ɾ��
int CUnitUtility::DeleteUnit(TUNITINFO* pUnitInfo)
{
    int iUnitID = pUnitInfo->iUnitID;
	
	CGameRoleObj* pRoleObj = CUnitUtility::GetRoleObj(iUnitID);
	ASSERT_AND_LOG_RTN_INT(pRoleObj);
	
	unsigned uin = pRoleObj->GetRoleID().uin();

	//����GameRole
	pRoleObj->ResetRole();

	//ɾ��GameRole
	GameTypeK32<CGameRoleObj>::DeleteByKey(uin);

    // ɾ��UnitID
    CUnitUtility::FreeUnitID(iUnitID);

    return 0;
}

// ��ȡ��ɫ��λ
CGameRoleObj* CUnitUtility::GetRoleByUin(const int uiUin)
{
    return GameTypeK32<CGameRoleObj>::GetByKey(uiUin);
}

CGameRoleObj* CUnitUtility::GetRoleByID(const RoleID& rstRoleID)
{
    CGameRoleObj* pRoleObj = GetRoleByUin(rstRoleID.uin());
    if (!pRoleObj || pRoleObj->GetRoleID().uiseq() != rstRoleID.uiseq())
    {
        return NULL;
    }

    return pRoleObj;
}

// ���õ�λ״̬
void CUnitUtility::SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus |= enStatus;
}

// �����λ״̬
void CUnitUtility::ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus)
{
    ASSERT_AND_LOG_RTN_VOID(pstUnit);

    pstUnit->uiUnitStatus &= ~(enStatus);
}

