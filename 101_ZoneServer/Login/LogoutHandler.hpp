#ifndef __LOGOUT_HANDLER__HPP__
#define __LOGOUT_HANDLER__HPP__


#include "Kernel/Handler.hpp"
#include "ErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CLogoutHandler : public IHandler
{
public:
    virtual ~CLogoutHandler();

public:
    virtual int OnClientMsg();

private:
    virtual int SecurityCheck();

public:
    // ����������
    static int LogoutRole(CGameRoleObj* pRoleObj, int iReason);

    // ����������
    // 1. ׼���׶�
    static int LogoutPrepare(CGameRoleObj* pRoleObj);

    // 2. ͬ������

    // 3. ɾ������, �Ͽ�����
    static int LogoutAction(CGameRoleObj* pRoleObj);

    // ֪ͨworld����
    static int NotifyLogoutToWorld(const RoleID& stRoleID);

private:
    int OnRequestLogout();

    static void NotifyRoleLogout(CGameRoleObj* pstRoleObj);

    static int SendSuccessfulResponse(CGameRoleObj* pRoleObj);
};
#endif
