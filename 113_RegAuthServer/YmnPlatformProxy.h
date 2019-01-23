#pragma once

#include "PlatformProxy.h"

//��è���˺�
class CYmnPlatformProxy : public CPlatformProxy
{
public:
	//���캯��
	CYmnPlatformProxy();

	//ƽ̨��֤
	virtual int PlatformAuth(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex);

private:

	//��֤�˺�
	int AuthYmnAccount(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex);

	//��ȡʵ����֤��Ϣ
	void GetRealNameStat(const AccountInfo& stInfo, RegAuth_PlatformAuth_Response& stResp, int iThreadIndex);
};