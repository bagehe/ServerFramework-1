#pragma once

#include "CodeQueue.hpp"
#include "ExchangePublic.hpp"
#include "CommDefine.h"

using namespace ServerLib;

// ExchangeӦ�ó���
class CExchangeApp
{
public:
    CExchangeApp();
    virtual ~CExchangeApp();

    virtual int Initialize(bool bResume);
    virtual void Run();

public:
    static void SetAppCmd(int iAppCmd);

private:
    virtual int ReloadConfig();
    virtual int LoadConfig();

	int LoadLogConfig(const char* pszConfigFile, const char* pszLogName);

private:
	bool m_bResumeMode; // �����ڴ�ָ�ģʽ����������ϴ������������Ĺ����ڴ�δ��ɾ����ֱ��attach��ȥ
	static int ms_iAppCmd;

	static const unsigned int MAIN_LOOP_SLEEP = 5 * 1000; // ��ѭ����ÿ��ѭ�����˯��10΢��
	unsigned int m_uiLoopTimes; // ��ѭ������
};

