#pragma once

#include "AppDef.hpp"
#include "ErrorNumDef.hpp"
#include "NameDBHandlerSet.hpp"

// NameDBӦ�ó���
class CNameDBApp
{
public:
    CNameDBApp();
    virtual ~CNameDBApp();

    int Initialize(bool bResume, int iWorldID);
    void Run();
    int HandleMsgIn(int& riRecvMsgCount);
    //process thread msg
    int HandleThreadMsg(int& riSendMsgCount);

public:
    static void SetAppCmd(int iAppCmd);

private:
    //receive msg and dispatch
    int ReceiveAndDispatchMsg();

private:
    bool m_bResumeMode; // �����ڴ�ָ�ģʽ����������ϴ������������Ĺ����ڴ�δ��ɾ����ֱ��attach��ȥ
    static int ms_iAppCmd;

	static unsigned short m_usWorldID;

private:
    int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);
    int ReloadConfig();
    int LoadConfig();

private:
    char m_szCodeBuf[MAX_NAMEDB_MSGBUFFER_SIZE];
};
