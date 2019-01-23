#ifndef __LOGOUT_HANDLER_HPP__
#define __LOGOUT_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CLogoutHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
public:
    CLogoutHandler(void);
public:
    ~CLogoutHandler(void);

public:
    virtual int OnClientMsg(GameProtocolMsg* pMsg);

public:
    // ֪ͨZone��������ɫ����
    static int LogoutRole(int iZoneID, const unsigned int uin);

public:
    // ͨ���ɫ������Ϣ
    static int LogoutNotify(CWorldRoleStatusWObj* pRoleObj);

private:
    int OnLogoutNotify();

private:
    GameProtocolMsg* m_pRequestMsgWGS;
};

#endif

