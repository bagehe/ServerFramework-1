#pragma once

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "Kernel/ZoneMsgHelper.hpp"

using namespace ServerLib;

class CUnitUtility
{
public:
    // ��UnitID��ȡ��λ����, ��λ����, �Լ���������
	static TUNITINFO* GetUnitInfo(const int iUnitID);

    static CGameRoleObj* GetRoleObj(const int iUnitID);

    // ��ȡ��ɫ��λ
    static CGameRoleObj* GetRoleByUin(const int uiUin);
    static CGameRoleObj* GetRoleByID(const RoleID& rstRoleID);

public:
    // ���õ�λ״̬
    static void SetUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // �����λ״̬
    static void ClearUnitStatus(TUNITINFO* pstUnit, EUnitStatus enStatus);

    // �жϵ�λ״̬
    static inline bool IsUnitStatusSet(TUNITINFO* pstUnit, EUnitStatus enStatus)
    {
        ASSERT_AND_LOG_RTN_BOOL(pstUnit);

        return pstUnit->uiUnitStatus & enStatus;
    }

	//����ʱ��ͬ����Ϣ
	static void SendSyncTimeNotify(CGameRoleObj* pstRoleObj, long8 lNowTime);

public:
    // ������ͷ�UnitID
    static int AllocateUnitID();
    static void FreeUnitID(const int iUnitID);

    // ��GameUnitID�а󶨵�λ�ͳ���
    static int BindUnitToObject(int iUnitID, int iObjectIdx);

    // ����һ����λ����
    static CObj* CreateUnit(unsigned int uiKey = 0);

	// ɾ��һ����λ, �ͷ�UnitID, ���ٶ���ID. 
	// ����ɾ����һ���ǳ�Σ�յ���Ϊ, ���Ҫ�Ƚ���λ��ΪEUS_DELETED״̬, ��AppTick��ɾ��
    static int DeleteUnit(TUNITINFO* pUnitInfo);

public:
    // Ϊ�˾�ϸ����ÿ�ֵ�λ�ı�������, ��ϵͳ��λ�ֱ����

    // ��ȡ��һ����λ����
    static int IterateRoleIdx();

private:
    static int m_iUpdateRoleIdx;
};

