
#include "GameProtocol.hpp"
#include "PasswordEncryptionUtility.hpp"
#include "ThreadLogManager.hpp"

#include "MainPlatformProxy.h"

//��Ϸ�Խ��˺�

//ƽ̨��֤
int CMainPlatformProxy::PlatformAuth(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex)
{
	//��������
	char szEncryptPasswd[256] = { 0 };
	int iEncryptBuffLen = sizeof(szEncryptPasswd);

	const std::string& strPassword = stInfo.strpassword();

	//����password
	int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(strPassword.c_str(), strPassword.size(), szEncryptPasswd, iEncryptBuffLen);
	if (iRet)
	{
		TRACE_THREAD(iThreadIndex, "Failed to encrypt password, account: %s, password: %s\n", stInfo.straccount().c_str(), strPassword.c_str());
		return -1;
	}

	//��������
	stResp.mutable_stinfo()->set_strpassword(szEncryptPasswd, strlen(szEncryptPasswd));

	return 0;
}
