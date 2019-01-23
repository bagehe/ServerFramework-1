#ifndef __WORLD_NAME_HANDLER_HPP__
#define __WORLD_NAME_HANDLER_HPP__

#include "Handler.hpp"
#include "AppDefW.hpp"
#include "WorldObjectHelperW_K64.hpp"
#include "WorldRoleStatus.hpp"

class CWorldNameHandler : public IHandler
{
public:
	virtual ~CWorldNameHandler();

public:
	virtual int OnClientMsg(GameProtocolMsg* pMsg);

private:

    //���������ֵ�����
	int OnRequestAddNewName();

    //���������ֵķ���
    int OnResponseAddNewName();

    int SendAddNewNameResponseToAccount(const std::string& strName, unsigned int uiUin, int iResult, int iType);

    //ɾ�����ֵ�������
    int OnRequestDeleteName();

private:
	GameProtocolMsg* m_pMsg;

	static GameProtocolMsg m_stWorldMsg;
};

#endif


