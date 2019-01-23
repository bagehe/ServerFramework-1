#pragma once

#include "CommDefine.h"

//���������һЩ�궨��
#ifdef _DEBUG_
const int APP_ROLEDB_MAX_SLEEP_USEC = 5000;	//�߳�sleepʱ��
#else
const int APP_ROLEDB_MAX_SLEEP_USEC = 5000;	//�߳�sleepʱ��
#endif

#define APP_CONFIG_FILE     "../conf/GameServer.tcm"

//���ӵ�MYSQL���ݿ���ص������ļ�
#define ROLEDBINFO_CONFIG_FILE "../conf/DBMSConf.xml"

//��ҽ�ɫ���ݱ�ı���
#define MYSQL_USERINFO_TABLE "t_userdata"

//��ҽ�ɫ���ݱ������,��ϸ�ֶβμ��������
#define MYSQL_USERINFO_FIELDS   9

//�����ֶ�����
enum RoleDBDataType
{
	ROLEDB_DATA_INVALID = 0,	//�Ƿ����ֶ�
	ROLEDB_DATA_BASEINFO = 1,	//������Ϣ
	ROLEDB_DATA_ITEMINFO = 2,	//������Ϣ
	ROLEDB_DATA_QUESTINFO = 3,	//������Ϣ
	ROLEDB_DATA_MAILINFO = 4,	//�ʼ���Ϣ
	ROLEDB_DATA_OFFLINE = 5,	//������Ϣ
	ROLEDB_DATA_RESERVED2 = 6,	//������Ϣ
	ROLEDB_DATA_MAX,			//�ֶ�������󣬳�����
};

//��������С
const int MAX_ROLEDB_MSGBUFFER_SIZE = 2048000;

//��ѯ��������С
const int MAX_QUERYBUFF_SIZE = 20480;
