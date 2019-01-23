#ifndef __ONLINE_STAT_HANDLER_HPP__
#define __ONLINE_STAT_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class COnlineStatHandler : public IHandler
{
public:
    virtual ~COnlineStatHandler();

public:
    virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:
    int OnZoneOnlineStatReq();
    int OnWorldOnlineStatResp();

private:
    GameProtocolMsg* m_pRequestMsg;
    GameProtocolMsg m_stWorldMsg;
};

#endif


