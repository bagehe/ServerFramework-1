#pragma once

#include "CommDefine.h"
#include "Kernel/Handler.hpp"
#include "QuestManager.h"

class CGameRoleObj;
class CQuestHandler : public IHandler
{
public:
	virtual ~CQuestHandler();

	virtual int OnClientMsg();

private:

	//����������
	int OnRequestFinQuest();

	//�����ȡ��Ծ�Ƚ���
	int OnRequestGetLivnessReward();

	//���ͻظ�
	int SendResponse(unsigned uiMsgID, const unsigned int uiResultID, const TNetHead_V2& rstNetHead);

protected:

	static GameProtocolMsg ms_stZoneMsg;
};
