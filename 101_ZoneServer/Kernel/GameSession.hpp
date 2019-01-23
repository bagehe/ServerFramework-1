#pragma once

#include <string>

#include "GameProtocol.hpp"
#include "ObjAllocator.hpp"
#include "CommonDef.hpp"
#include "CommDefine.h"
#include "CommDefine.h"

using namespace ServerLib;

const int MAX_GAMESESSION_NUMBER = MAX_ROLE_OBJECT_NUMBER_IN_ZONE;

class CGameRoleObj;
class CGameSessionObj : public CObj
{
public:
    CGameSessionObj();
    virtual ~CGameSessionObj();
    virtual int Initialize();
    DECLARE_DYN

public:
    ////////////////////////////////////////////////////////////////////////////////
    // �Ự��Ϣ
    void SetRoleID(const RoleID& stRoleID) { m_stRoleID.CopyFrom(stRoleID); };
    const RoleID& GetRoleID() { return m_stRoleID; };

    // �ỰID (Socket FD)
    void SetID(int iID) {m_iID = iID;}
    int GetID() {return m_iID;}

    // �Ự����
    void SetNetHead(const TNetHead_V2* pNetHead);
    const TNetHead_V2& GetNetHead(){return m_stNetHead;};

    const char* GetClientIP() {return &m_szClientIP[0];}

    // �Ự�ͻ��˰汾��
    void SetClientVersion(int iVersion) {m_iClientVersion = iVersion;};
    int GetClientVersion() {return m_iClientVersion;};

    // ��¼��, �󶨵Ľ�ɫ����
    CGameRoleObj* GetBindingRole();
    void SetBindingRole(const CGameRoleObj* pZoneRoleObj);

    // ����ʱ��
    int GetCreateTime() {return m_iCreateTime;};

	//��¼ԭ��
	unsigned char GetLoginReason() { return m_ucLoginReason; }
	void SetLoginReason(unsigned char ucLoginReason) { m_ucLoginReason = ucLoginReason; }

	//���ͷ��
	void SetPictureID(const std::string& strPicID);
	const char* GetPictureID();

private:

    //socket FD
    int m_iID;
    TNetHead_V2 m_stNetHead;
    int m_iRoleIdx;
    int m_iClientVersion;
    int m_iCreateTime;

    char m_szClientIP[MAX_IPV4_LENGTH];
    RoleID m_stRoleID;

    unsigned char m_ucLoginReason;

	char m_szPicID[128];
};
