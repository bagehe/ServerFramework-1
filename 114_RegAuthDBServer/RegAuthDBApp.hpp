#pragma once

#include "AppDef.hpp"
#include "ErrorNumDef.hpp"
#include "ThreadLogManager.hpp"

// RegAuthDBӦ�ó���
class CRegAuthDBApp
{
public:
    CRegAuthDBApp();
    virtual ~CRegAuthDBApp();

    int Initialize(bool bResume, int iWorldID);
    void Run();
    int HandleMsgIn(int& riRecvMsgCount);
    int HandleThreadMsg(int& riSendMsgCount);

public:
    static void SetAppCmd(int iAppCmd);

private:
    int ReceiveAndDispatchMsg();
    int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);
    int ReloadConfig();
    int LoadConfig();

private:
	bool m_bResumeMode; // �����ڴ�ָ�ģʽ����������ϴ������������Ĺ����ڴ�δ��ɾ����ֱ��attach��ȥ
	static int ms_iAppCmd;

    char m_szCodeBuf[MAX_REGAUTHDB_MSGBUFFER_SIZE];
};
