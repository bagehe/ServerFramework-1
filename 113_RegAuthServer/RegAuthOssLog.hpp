#pragma once

//��Ӫ��־��

#include "GameProtocol.hpp"

enum RegAuthOssType
{
	REGAUTH_OSS_INVALID		= 0,	//�Ƿ�
	REGAUTH_OSS_REGAUTH		= 1,	//ע����֤
};

class CRegAuthOssLog
{
public:

	//�����֤ע��
	static void PlayerRegAuth(const AccountInfo& stInfo, unsigned uin, bool bIsRegister, bool bSuccess);
};
