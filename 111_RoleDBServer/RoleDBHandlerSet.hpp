#pragma once

#include "MsgHandlerSet.hpp"
#include "RoleDBFetchRoleHandler.hpp"
#include "RoleDBUpdateRoleHandler.hpp"
#include "RoleDBCreateRoleHandler.hpp"
#include "RoleDBListRoleHandler.hpp"
#include "RoleDBRechargeHandler.hpp"
#include "RoleDBMailHandler.hpp"
#include "RoleDBGMHandler.hpp"

// RoleDBӦ���е���Ϣ�����߹�����
class CRoleDBHandlerSet : public CMsgHandlerSet
{
public:
    CRoleDBHandlerSet();
    ~CRoleDBHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:
	// �ü��Ϲ����������Ϣ������
	CRoleDBFetchRoleHandler m_FetchRoleHandler;
	CRoleDBUpdateRoleHandler m_UpdateRoleHandler;
	CRoleDBCreateRoleHandler m_CreateRoleHandler;
	CRoleDBListRoleHandler m_ListRoleHandler;
	CRoleDBRechargeHandler m_RechargeHandler;
	CRoleDBGMHandler m_GameMasterHandler;
	CRoleDBMailHandler m_MailHandler;

private:
	// ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
	DBClientWrapper m_oDBClient;

	//���ݿ����������
	char* m_pQueryBuff;

	int m_iThreadIdx;
};
