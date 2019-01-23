#ifndef __LIST_ROLE_ACCOUNT_HANDLER_HPP__
#define __LIST_ROLE_ACCOUNT_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"

//����Account Server��������ȡ����ʺŻ�����Ϣ��Э��
class CListRoleAccountHandler : public IHandler
{
protected:
	static GameProtocolMsg m_stWorldMsg;
private:
    GameProtocolMsg* m_pRequestMsg;

public:
    virtual ~CListRoleAccountHandler();

public:
    virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:
    int OnAccountListRoleRequest();
    int OnAccountListRoleResponse();
};

#endif // __LIST_ROLE_ACCOUNT_HANDLER_HPP__
