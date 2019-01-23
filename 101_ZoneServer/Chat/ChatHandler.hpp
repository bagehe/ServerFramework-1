
#ifndef __CHAT_HANDLER_HPP__
#define __CHAT_HANDLER_HPP__

#include "GameProtocol.hpp"
#include "Kernel/Handler.hpp"
#include "CommDefine.h"
#include "ErrorNumDef.hpp"

class CGameRoleObj;
class CChatHandler: public IHandler
{
public:
    virtual ~CChatHandler(void);
    CChatHandler();
public:
    int OnClientMsg();

private:

    //������Ϣ�Ĵ���
    int OnRequestChat();

    //����������Ϣ������
    int OnNotifyChat();
	
	//��������Ƶ�����
	int OnNotifyHorseLamp();

	//����������Ϣ
	int SendChatMsg(CGameRoleObj& stRoleObj, int iChannel, const char* pMsg);

	//����ʧ�ܵĻظ�
    int SendFailedResponse(int iMsgID, int iResultID, const TNetHead_V2& rstNetHead);

    //���ͳɹ��Ļظ�
    int SendSuccessfulResponse();
};

#endif
