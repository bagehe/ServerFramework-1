#ifndef __FETCHROLE_WORLD_HANDLER_HPP__
#define __FETCHROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CFetchRoleWorldHandler : public IHandler
{
public:
	virtual ~CFetchRoleWorldHandler();

public:
	virtual int OnClientMsg(GameProtocolMsg* pMsg);

public:
    // ���ؽ�ɫ��ѯ����
    int SendFetchRoleResponse(const RoleID& stRoleID, int iReqID, const KickerInfo& stKicker);

    // ����ʧ�ܲ�ѯ
    int SendFailedFetchRoleResponse(const RoleID& stRoleID, int iReqID);

private:
    // ��ѯ���������
	int OnRequestFetchRoleWorld();

    //��ѯ���󷵻ص�World�Ĵ������
    int OnResponseFetchRoleWorld();

	//������ɫ���صĴ������
	int OnResponseCreateRoleWorld();

	//���ʹ�����ɫ������
	int SendCreateRoleRequestToDBSvr(const KickerInfo& stKicker);

	//��ʼ�������Ϣ
	int InitRoleBirthInfo(World_CreateRole_Request& rstRequest, const KickerInfo& stKicker);

	//�����ɹ�
	int OnFetchSuccess(const RoleID& stRoleID, const GameUserInfo& stUserInfo, bool bIsLogin, const KickerInfo& stKicker);

private:
	GameProtocolMsg* m_pMsg;

	GameProtocolMsg m_stWorldMsg;
};

#endif


