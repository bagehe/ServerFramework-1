
#ifndef __LIST_ZONE_HANDLER_HPP__
#define __LIST_ZONE_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

//��ȡ��ǰ���п��õķ�����Ϣ

class CListZoneHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
public:
    CListZoneHandler(void);
public:
    ~CListZoneHandler(void);

public:
    virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:
    GameProtocolMsg* m_pRequestMsg;
};

#endif

