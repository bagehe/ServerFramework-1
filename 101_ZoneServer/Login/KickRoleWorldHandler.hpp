// ���������¼�

#ifndef __KICKROLE_WORLD_HANDLER_HPP__
#define __KICKROLE_WORLD_HANDLER_HPP__

#include "Kernel/Handler.hpp"
#include "ErrorNumDef.hpp"

class CGameSessionObj;
class CKickRoleWorldHandler : public IHandler
{
public:
    virtual ~CKickRoleWorldHandler();

public:
    virtual int OnClientMsg();

public:
    // ���سɹ��ظ�
    static int SendSuccessfullResponse(const World_KickRole_Request& rstResp);

    // ����ʧ�ܻظ�
    static int SendFailedResponse(const World_KickRole_Request& rstResp);

private:
    // ��������World������֪ͨ, ��������
    int OnRequestKickRole();

    // ��������World�����߻ظ�, ��������
    int OnResponseKickRole();

private:
    // ��World�����������
    int FetchRoleFromWorldServer(const RoleID& stRoleID, const KickerInfo& stKicker, char cEnterType);    
};

#endif
