#pragma once

#include "GameProtocol.hpp"
#include "CommDefine.h"

typedef struct
{
	GameProtocolMsg stResponseMsg;	//��Ӧ��Ϣ
	int iNeedResponse;				//true��Ҫ�ظ���false����Ҫ

	void Reset()
	{
		stResponseMsg.Clear();
		iNeedResponse = true;
	}
} SHandleResult;

// ��Ϣ�����߳�����
class CMsgHandler
{
protected:
	CMsgHandler() {};

public:
    virtual ~CMsgHandler() {};

    // ��Ϣ����
    virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL) = 0;

protected:

	void GenerateMsgHead(GameProtocolMsg& stMsg, unsigned int uiSessionID, unsigned int uiMsgID, unsigned int uiUin);
};
