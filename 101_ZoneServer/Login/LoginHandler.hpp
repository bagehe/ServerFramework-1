#ifndef __LOGIN_HANDLER_HPP__
#define __LOGIN_HANDLER_HPP__

#include "Kernel/Handler.hpp"
#include "ErrorNumDef.hpp"

class CGameRoleObj;
class CGameSessionObj;

class CLoginHandler : public IHandler
{
public:
	virtual ~CLoginHandler();

public:
	virtual int OnClientMsg();

private:
    virtual int SecurityCheck(const TNetHead_V2& rstNetHead);

	//У��SessionKey
	int CheckSessionKey(const std::string& strSessionKey, unsigned uiUin, int nWorldID, int& iRealNameStat);

public:
    // ��¼���
	int LoginRole(const RoleID& stRoleID, TNetHead_V2* pNetHead, const Zone_LoginServer_Request& rstRequest);

    // ��¼�ɹ�
     static int LoginOK(unsigned int uiUin, bool bNeedResponse = true);

    // ��¼ʧ��
     static int LoginFailed(const TNetHead_V2& rstNetHead);
     
     //ͨ��World����ͬuin���Ѿ���¼�ĺ�������
     static int KickRoleFromWorldServer(const RoleID& stRoleID, int iFromSessionID, const Zone_LoginServer_Request& rstRequest);

private:
    int OnRequestLoginServer();

    //֪ͨ��ɫ��¼
    static int NotifyRoleLogin(CGameRoleObj* pstRoleObj);

    // �������ϵ����������
    int KickRoleFromZoneServer();
    
private:

    // ����ʧ�ܻظ�
     static int SendFailedResponse(const unsigned int uiResultID, const TNetHead_V2& rstNetHead);

     // ���ͳɹ��ظ�
     static int SendSuccessfulResponse(CGameRoleObj* pLoginRoleObj);

private:

	//��������ͷ��ID
	std::string m_strPicID;
};

#endif


