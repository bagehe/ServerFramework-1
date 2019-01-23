#pragma once

//��ȡDB������Ϣ

#include <vector>
#include <string.h>

struct OneDBInfo
{
	int iDBMajorVersion;    //ʹ�õ����ݿ�����汾��
	int iDBMinVersion;      //ʹ�õ����ݿ���Ӱ汾��
	char szDBEngine[64];    //ʹ�õ�DB���������
	char szDBHost[64];      //�������ݿ��HOST
	char szUserName[64];    //�������ݿ���û���
	char szUserPasswd[64];  //�������ݿ������
	char szDBName[64];      //���ӵ����ݿ��DB����

	OneDBInfo()
	{
		Reset();
	};

	void Reset()
	{
		memset(this, 0, sizeof(*this));
	}
};

class DBConfigManager
{
public:
	DBConfigManager();
	~DBConfigManager();

	int LoadDBConfig(const char* pszFilePath);

	const OneDBInfo* GetDBConfigByIndex(int iDBIndex);

private:

	std::vector<OneDBInfo> m_vDBConfigInfos;
};
