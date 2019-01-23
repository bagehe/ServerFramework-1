#pragma once

#include "CommDefine.h"

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_NAMEDB_MAX_SLEEP_USEC = 5 * 1000;  //�߳�sleepʱ��
#else
const int APP_NAMEDB_MAX_SLEEP_USEC = 5 * 1000;	//�߳�sleepʱ��
#endif

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define NAMEDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//�������ݿ�ı���
#define MYSQL_NAMEINFO_TABLE "t_nameinfo"

//�������ݿ����������: nickname, nametype, nameid
#define MYSQL_NAMEINFO_FIELDS   3

const int MAX_NAMEDB_MSGBUFFER_SIZE = 204800;

//���ݿ������������С
const int MAX_QUERYBUFF_SIZE = 1024;
