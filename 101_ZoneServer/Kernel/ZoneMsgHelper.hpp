#ifndef __ZONE_MSG_HELPER_HPP__
#define __ZONE_MSG_HELPER_HPP__

#include "GameProtocol.hpp"
#include "Kernel/GameSession.hpp"

#include "LogAdapter.hpp"
#include "GameObjCommDef.hpp"

// ��װ�������·����ͻ��˵ĸ�����Ϣ, ������:
// 1. ����IHandler֮���ģ��ʹ��

class CGameRoleObj;

typedef struct tagRoleObjList
{
    int m_iRoleNumber;
    CGameRoleObj* m_apstRoleObj[MAX_ROLE_OBJECT_NUMBER_IN_ZONE];
}TRoleObjList;

class CZoneMsgHelper
{
public:
    //��ʼ����Ϣͷ��
    static void GenerateMsgHead(GameProtocolMsg& rstGameMsg, const unsigned int uiMsgID);

public:
    //������Ϣ���ͻ���
    static int SendZoneMsgToClient(GameProtocolMsg& rstZoneMsg, const TNetHead_V2& rstNetHead);

    //������Ϣ��World
    static int SendZoneMsgToWorld(const GameProtocolMsg& rstZoneMsg);

public:
    //������Ϣ��pRoleObj
    static int SendZoneMsgToRole(GameProtocolMsg& rstGameMsg, CGameRoleObj *pRoleObj); 

    //������Ϣ���ܶ���
    static int SendZoneMsgToRoleList(GameProtocolMsg& rstZoneMsg, const TRoleObjList& rstRoleList);

    //������Ϣ�����������û�
    static int SendZoneMsgToZoneAll(GameProtocolMsg& rstZoneMsg);

    //������Ϣ�����������û�, ����pMe
    static int SendZoneMsgToZoneAllExcludeMe(GameProtocolMsg& rstZoneMsg, CGameRoleObj* pRoleObj);  

	//��ӵ������б�
	static inline void AddRoleToList(TRoleObjList& rstRoleList, CGameRoleObj* pRoleObj);
};


#endif
