#pragma once

//������������
#include <string.h>
#include <vector>

#include "GameProtocol.hpp"
#include "CommDefine.h"
#include "BaseConfigManager.hpp"

class CGameRoleObj;
class CMailManager
{
public:
	CMailManager();
	~CMailManager();

public:

	//��ʼ��
	int Initialize();

	void SetOwner(unsigned int uin);
	CGameRoleObj* GetOwner();

	//�����ʼ�
	void AddNewMail(const OneMailInfo& stMailInfo, bool bSendNotify = true);

	//�����ʼ�
	int OperateMail(int iOperaType, unsigned uUniqID);

	//������ϵͳ�ʼ�ΨһID
	static unsigned GetServerMailUniqID();
	static void SetServerMailUniqID(unsigned uUniqID);

	//���ϵͳ�ʼ�ΨһID
	unsigned GetRoleSystemUniqID();
	void SetRoleSystemMailUniqID(unsigned uUniqID);

	//��ʱ��
	void OnTick(int iTimeNow);

	//�����ʼ���DB
	void UpdateMailToDB(MAILDBINFO& stMailDBInfo);

	//��DB��ʼ���ʼ�
	void InitMailFromDB(const MAILDBINFO& stMailDBInfo);

private:

	//��ȡ�ʼ�λ�ã������ڷ���-1
	int GetMailIndexByUniqID(unsigned uUniqID);

	//�������ʼ���Ϣ
	void SendAddNewMailNotify(const MailData& stOneMail, const std::vector<unsigned>& vDelUniqIDs);

	//��ȡ�ʼ�����
	int GetMailAppendix(MailData& stOneMail);

private:

	//���uin
	unsigned m_uiUin;

	//���ϵͳ�ʼ�ΨһID
	unsigned m_uRoleSystemUniqID;

	//��Ҹ����ʼ�ΨһID
	unsigned m_uPersonalUniqID;
	
	//����ʼ���Ϣ
	std::vector<MailData> m_vMails;

	//������ϵͳ�ʼ�ΨһID
	static unsigned m_uServerSystemMailUniqID;
};
