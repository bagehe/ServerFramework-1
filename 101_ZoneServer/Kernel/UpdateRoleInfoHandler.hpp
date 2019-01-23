#ifndef __UPDATE_ROLEINFO_HANDLER__HPP__
#define __UPDATE_ROLEINFO_HANDLER__HPP__

#include <assert.h>
#include <string.h>

#include "Kernel/Handler.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"


using namespace ServerLib;

class CUpdateRoleInfo_Handler: public IHandler
{
public:
	virtual ~CUpdateRoleInfo_Handler();

public:
	virtual int OnClientMsg();

public:
	// ͬ��������ݵ����ݿ�
    static int UpdateRoleInfo(CGameRoleObj* pRoleObj, unsigned char ucNeedResponse);

	//���������Ϣ
	static void TallyRoleData(CGameRoleObj& stRoleObj);

	int OnUpdateRoleInfoResponse();
};

#endif
