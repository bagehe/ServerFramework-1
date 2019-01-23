#ifndef __KICKROLE_WORLD_HANDLER_HPP__
#define __KICKROLE_WORLD_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

//�������˵���Ϣ����
class CKickRoleWorldHandler : public IHandler
{
protected:
	static GameProtocolMsg ms_stGameMsg;
public:
    virtual ~CKickRoleWorldHandler();

public:
    virtual int OnClientMsg(GameProtocolMsg* pstMsg);

private:
    // A1. ��������Zone����������
    int OnZoneRequestKickRole();

private:
    // B2: ��������Zone�����˻ظ�
    int OnZoneResponseKickRole();

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif
