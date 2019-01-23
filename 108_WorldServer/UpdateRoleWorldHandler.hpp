#ifndef __UPDATEROLE_WORLD_HANDLER_HPP__
#define __UPDATEROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CUpdateRoleWorldHandler : public IHandler
{
public:
	virtual ~CUpdateRoleWorldHandler();

public:
	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:
    // �������ݴ������
	int OnRequestUpdateRoleWorld();

    //�������ݷ��صĴ������
    int OnResponseUpdateRoleWorld();

    // ����ʧ����Ϣ
	int SendUpdateRoleResponseWGS(const RoleID& stRoleID, int iZoneID, unsigned int uiResultID);

private:
	GameProtocolMsg* m_pMsg;

	GameProtocolMsg m_stWorldMsg;
};

#endif


