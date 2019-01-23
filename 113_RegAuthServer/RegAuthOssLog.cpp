
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "TimeUtility.hpp"

#include "RegAuthOssLog.hpp"

using namespace ServerLib;

//�����֤ע��
void CRegAuthOssLog::PlayerRegAuth(const AccountInfo& stInfo, unsigned uin, bool bIsRegister, bool bSuccess)
{
	//��ʽ�� REGAUTH_OSS_REGAUTH | time | accout | bIsregister | uin | channelid | deviceid | bSuccess 
	TRACEBILL("| %d | %d | %s | %d | %u | %s | %s | %d\n", REGAUTH_OSS_REGAUTH, CTimeUtility::GetNowTime(), stInfo.straccount().c_str(), 
		bIsRegister, uin, stInfo.strchannel().c_str(), stInfo.strdeviceid().c_str(), bSuccess);
}
