
#ifndef __HANDLER_FACTORY_HPP__
#define __HANDLER_FACTORY_HPP__

#include "CommDefine.h"
#include "Kernel/Handler.hpp"

class IHandler;

using namespace std;


typedef enum tagenZoneMsgType
{
	EKMT_CLIENT = 1,    // �ͻ�����Ϣ
	EKMT_SERVER = 2,    // ��������Ϣ

}EZoneMsgType;

typedef enum tagEnumZoneMsgFlag
{
	// �����
	EKMF_CHECKNONE		= 0,

	// ������漰������Զ�����, �����������״̬
	EKMF_CHECKDEAD		= 1,	

	// ����漰���κ���Ʒ, ������鱳��״̬
	EKMF_CHECKBAGLOCK	= 2,	

	// ���ȫ��
	EKMF_CHECKALL		= EKMF_CHECKDEAD | EKMF_CHECKBAGLOCK,

}EZoneMsgFlag;

typedef struct tagZoneMsgConfig
{
	EZoneMsgType m_enMsgType;
	unsigned int m_uiMsgFlag;
	int m_iMsgInterval;
	bool m_bDisabled;
}TZoneMsgConfig;

class CHandlerFactory
{
public:
    // ��ȡ��Ϣ������
	static IHandler* GetHandler(const unsigned int uiMsgID, EZoneMsgType enMsgType = EKMT_SERVER);

	// ��ȡ��Ϣ������Ϣ
	static TZoneMsgConfig* GetMsgConfig(const unsigned int uiMsgID);

    // ע����Ϣ������
	static int RegisterHandler(const unsigned int uiMsgID, const IHandler* pHandler, 
		EZoneMsgType enMsgType = EKMT_SERVER, 
		unsigned int uiHandlerFlag = EKMF_CHECKALL
		);

	static void DisabledMsg(const unsigned int uiMsgID);
	static bool IsMsgDisabled(const unsigned int uiMsgID);
	static void ClearAllDisableMsg();
protected:
	static IHandler* m_apHandler[MAX_HANDLER_NUMBER];
	static TZoneMsgConfig m_astMsgConfig[MAX_HANDLER_NUMBER];
};

#endif
