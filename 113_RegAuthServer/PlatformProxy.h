#pragma once

#include "GameProtocol.hpp"

class CPlatformProxy
{
public:

	//ƽ̨��֤
	virtual int PlatformAuth(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex) = 0;
};