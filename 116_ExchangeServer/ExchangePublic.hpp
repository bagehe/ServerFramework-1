#pragma once

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define CARDNODBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//���ܵ����ݿ��
#define MYSQL_CARDNO_TABLE "t_cardno_info"

static const int MAX_CODE_LEN = 10240;

static const int MAX_FD_NUMBER = 1000000;

//�����߳�����
static const int WORK_THREAD_NUM = 8;

//���ݿ������������С
const int MAX_QUERYBUFF_SIZE = 2048;
