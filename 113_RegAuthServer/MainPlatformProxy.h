#pragma once

#include "PlatformProxy.h"

//��Ϸ�Խ��˺�
class CMainPlatformProxy : public CPlatformProxy
{
public:
	
	//ƽ̨��֤
	virtual int PlatformAuth(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex);
};