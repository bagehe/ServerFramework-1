#pragma once

#include "Kernel/Handler.hpp"

class CFetchRoleWorldHandler : public IHandler
{
public:
    virtual ~CFetchRoleWorldHandler();

public:
    virtual int OnClientMsg();

private:
    int OnFetchRole();

    ////////////////////////////////////////////////////////
    // ��¼��ɫ
    int LoginRole();

    // ��ʼ����ɫ����
    int InitRoleData();
    
    // ��¼���ʼ��
    int InitRoleAfterLogin(CGameRoleObj& stRoleObj, const World_FetchRole_Response& stResp);
};
