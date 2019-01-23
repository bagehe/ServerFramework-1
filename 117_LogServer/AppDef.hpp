#pragma once

#include "CommDefine.h"

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_LOGSERVER_MAX_SLEEP_USEC = 5 * 1000;  //�߳�sleepʱ��
#else
const int APP_LOGSERVER_MAX_SLEEP_USEC = 5 * 1000;	//�߳�sleepʱ��
#endif

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define MOFANGDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//��־���ݿ�ĵ�¼��
#define MYSQL_MOFANGLOGIN_TABLE "t_mofang_login"

//��־���ݿ�ĳ�ֵ��
#define MYSQL_MOFANGPAY_TABLE "t_mofang_pay"

//��־���ݿ����ҽ����
#define MYSQL_TALLYINFO_TABLE "t_tally_info"

const int MAX_LOGSERVER_MSGBUFFER_SIZE = 204800;

//���ݿ������������С
const int MAX_QUERYBUFF_SIZE = 2048;
