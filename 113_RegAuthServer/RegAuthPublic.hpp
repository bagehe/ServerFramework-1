#pragma once

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

static const int MAX_CODE_LEN = 10240;

static const int MAX_FD_NUMBER = 1000000;

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_RegAuth_MAX_SLEEP_USEC = 10;	//�߳�sleepʱ��
#else
const int APP_RegAuth_MAX_SLEEP_USEC = 10;	//�߳�sleepʱ��
#endif

//ƽ̨��֤�̳߳��߳�����
static const int PLATFORM_AUTH_THREAD_NUM = 8;
