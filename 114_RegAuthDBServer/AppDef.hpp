#pragma once

#include "CommDefine.h"

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_REGAUTHDB_MAX_SLEEP_USEC = 5 * 1000;	//�߳�sleepʱ��
#else
const int APP_REGAUTHDB_MAX_SLEEP_USEC = 5 * 1000;	//�߳�sleepʱ��
#endif

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define REGAUTHDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//���ӵ�UniqUinDB���ݿ���ص������ļ�
#define UNIQUINDBINFO_CONFIG_FILE "../conf/DBMSConf_UniqUin.xml"

//����ʺ����ݿ�ı���
#define MYSQL_ACCOUNTINFO_TABLE "t_accountdata"

//����ʺ����ݱ��������Ϊ: accountID, accountType, uin, password, worldID
#define MYSQL_ACCOUNTINFO_FIELDS   5

//�������ΨһUIN�����ݿ����
#define MYSQL_UNIQUININFO_TABLE "t_uniquindata"

//��Ϣ��������С
const int MAX_REGAUTHDB_MSGBUFFER_SIZE = 204800;

//���ݿ������������С
const int MAX_QUERYBUFF_SIZE = 1024;
