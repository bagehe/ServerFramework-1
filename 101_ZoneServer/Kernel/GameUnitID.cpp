
#include "GameProtocol.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/GameRole.hpp"

#include "GameUnitID.hpp"

IMPLEMENT_DYN(CGameUnitID)

int CGameUnitID::Initialize()
{
    m_iBindObjectIdx    = -1;
    m_pstUnitInfo  = NULL;

    return 0;
}

int CGameUnitID::Resume()
{
    // 0 ����Ч��UnitID
    if (GetObjectID() == 0)
    {
        return 0;
    }

    BindUnitInfo();

    return 0;
}

// �󶨶���Idx
void CGameUnitID::BindObject(int iObjectIdx)
{
    m_iBindObjectIdx = iObjectIdx;

    // ���ö�������
    BindUnitInfo();
}

// ��ȡ�󶨶��������
TUNITINFO* CGameUnitID::GetBindUnitInfo()
{
    return m_pstUnitInfo;
}

// ��ȡ�󶨶����Idx
int CGameUnitID::GetBindObjectIdx()
{
    return m_iBindObjectIdx;
}

void CGameUnitID::BindUnitInfo()
{
	CGameRoleObj* pRoleObj = GameTypeK32<CGameRoleObj>::GetByIdx(m_iBindObjectIdx);
	ASSERT_AND_LOG_RTN_VOID(pRoleObj);
	
	m_pstUnitInfo = &(pRoleObj->GetRoleInfo().stUnitInfo);
    ASSERT_AND_LOG_RTN_VOID(m_pstUnitInfo->iUnitID == GetObjectID());

	TRACESVR("BindUnit: ID = %d, Idx = %d\n", m_pstUnitInfo->iUnitID, m_iBindObjectIdx);
}
