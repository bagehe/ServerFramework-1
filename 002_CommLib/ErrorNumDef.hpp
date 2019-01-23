#pragma once

enum ErrorNumType
{
	T_SERVER_SUCCESS			= 0,		//����ɹ�

	//[1000,2000) ΪZoneServer����
	T_ZONE_ERROR_BASE			= 1000,						//Zone�����뿪ʼ
	T_ZONE_PARA_ERROR			= T_ZONE_ERROR_BASE + 1,	//Zone��������
	T_ZONE_PROTOBUFF_ERR		= T_ZONE_ERROR_BASE + 2,	//protobuf ����
	T_ZONE_LOGINSERVER_FAILED	= T_ZONE_ERROR_BASE + 3,	//��¼������������
	T_ZONE_LOGOUT_KICKOFF		= T_ZONE_ERROR_BASE + 4,    //�������ߴ���
	T_ZONE_SESSION_EXISTS_ERR	= T_ZONE_ERROR_BASE + 5,    //��ҵ�Session�Ѿ�����
	T_ZONE_LOGOUT_UNACTIVE		= T_ZONE_ERROR_BASE + 6,    //���Ѿ�����Ծ���������
	T_ZONE_INVALID_CFG			= T_ZONE_ERROR_BASE + 7,    //�Ƿ�������
	T_ZONE_GAMEROLE_NOT_EXIST	= T_ZONE_ERROR_BASE + 8,    //��ҽ�ɫ��Ϣ������
	T_ZONE_SECURITY_CHECK_FAILED= T_ZONE_ERROR_BASE + 9,    //���������ȫУ�����
	T_ZONE_GM_COMMAND_FAILED	= T_ZONE_ERROR_BASE + 10,   //ִ��GM����ʧ��
	T_ZONE_INVALID_BULLETNUM	= T_ZONE_ERROR_BASE + 11,   //�Ƿ����ӵ���Ŀ
	T_ZONE_INVALID_NOSILVER		= T_ZONE_ERROR_BASE + 12,   //�����������
	T_ZONE_INVALID_BULLET		= T_ZONE_ERROR_BASE + 13,   //�Ƿ����ӵ���Ϣ
	T_ZONE_INVALID_MAILTYPE		= T_ZONE_ERROR_BASE + 14,   //�Ƿ����ʼ�����
	T_ZONE_INVALID_SKILLTYPE	= T_ZONE_ERROR_BASE + 15,   //�Ƿ��ļ�������
	T_ZONE_INVALID_AIMSTAT		= T_ZONE_ERROR_BASE + 16,   //�Ƿ�����׼״̬
	T_ZONE_INVALID_FISH			= T_ZONE_ERROR_BASE + 17,   //�Ƿ�����
	T_ZONE_INVALID_LOTTERYSTEP	= T_ZONE_ERROR_BASE + 18,   //�Ƿ���ת�̽׶�
	T_ZONE_INVALID_GMUSER		= T_ZONE_ERROR_BASE + 19,   //�Ƿ���GM�û�
	T_ZONE_PERSON_LIMIT			= T_ZONE_ERROR_BASE + 20,	//�û���������

	//[2000,3000) ΪWorldServer����
	T_WORLD_ERROR_BASE			= 2000,						//World�����뿪ʼ
	T_WORLD_PARA_ERROR			= T_WORLD_ERROR_BASE + 1,	//World��������
	T_WORLD_CANNOT_CREATE_ROLE	= T_WORLD_ERROR_BASE + 2,   //World���ܴ�����ɫ
	T_WORLD_SERVER_BUSY			= T_WORLD_ERROR_BASE + 3,   //World������æ
	T_WORLD_SESSION_EXIST		= T_WORLD_ERROR_BASE + 4,   //session�Ѿ�����  
	T_WORLD_FETCHROLE_FAIED		= T_WORLD_ERROR_BASE + 5,   //��ȡ��ɫ������Ϣʧ�� 

	//[3000,4000) ΪNameDBServer����
	T_NAMEDB_ERROR_BASE			= 3000,						//NameDB�����뿪ʼ
	T_NAMEDB_PARA_ERROR			= T_NAMEDB_ERROR_BASE + 1,	//NameDB��������
	T_NAMEDB_NAME_EXISTS		= T_NAMEDB_ERROR_BASE + 2,  //Ҫ�������ʺ��Ѿ�����

	//[4000,5000) ΪRoleDBServer����
	T_ROLEDB_ERROR_BASE			= 4000,						//RoleDB�����뿪ʼ
	T_ROLEDB_PARA_ERR			= T_ROLEDB_ERROR_BASE + 1,  //��������
	T_ROLEDB_SQL_EXECUTE_FAILED = T_ROLEDB_ERROR_BASE + 2,  //SQL���ִ��ʧ��
	T_ROLEDB_INVALID_RECORD		= T_ROLEDB_ERROR_BASE + 3,  //�Ƿ������ݿ��ѯ��¼

	//[5000,6000) ΪRegAuthServer����
	T_REGAUTH_ERROR_BASE		= 5000,						//RegAuth�����뿪ʼ
	T_REGAUTH_PARA_ERROR		= T_REGAUTH_ERROR_BASE + 1,	//RegAuth��������
	T_REGAUTH_SERVER_BUSY		= T_REGAUTH_ERROR_BASE + 2, //RegAuth������æ

	//[6000,7000) ΪRegAuthDBServer����
	T_REGAUTHDB_ERROR_BASE		= 6000,							//RegAuthDB�����뿪ʼ
	T_REGAUTHDB_PARA_ERROR		= T_REGAUTHDB_ERROR_BASE + 1,	//RegAuthDB��������
	T_REGAUTHDB_ACCOUNT_EXISTS	= T_REGAUTHDB_ERROR_BASE + 2,	//Ҫ�������ʺ��Ѿ�����
	T_REGAUTHDB_SQL_FAILED		= T_REGAUTHDB_ERROR_BASE + 3,	//SQL���ִ��ʧ��
	T_REGAUTHDB_INVALID_RECORD	= T_REGAUTHDB_ERROR_BASE + 4,	//�Ƿ������ݿ��¼   
	T_REGAUTHDB_INVALID_PASSWD	= T_REGAUTHDB_ERROR_BASE + 5,	//�ʺ��������

	//[7000,8000) ΪExchangeServer����
	T_EXCHANGE_ERROR_BASE = 7000,						//Exchange�����뿪ʼ
	T_EXCHANGE_PARA_ERROR = T_EXCHANGE_ERROR_BASE + 1,	//Exchange��������											
};
