#pragma once

#include <vector>
#include <string>
#include "GameProtocol.hpp"
#include "CommDefine.h"

struct SystemMailData
{
	int iVIPMin;	//VIP��С,Ϊ0û������
	int iVIPMax;	//VIP���,Ϊ0û������
	std::vector<std::string> vChannelIDs;	//���������ƣ�û��Ϊ��
	MailData stMail;	//�ʼ�����
	int iEndTime;		//�ʼ�����ʱ��

	SystemMailData()
	{
		Reset();
	}

	void Reset()
	{
		iVIPMin = 0;
		iVIPMax = 0;
		vChannelIDs.clear();
		stMail.Reset();
		iEndTime = 0;
	}
};

class CWorldMailManager
{
public:
	static CWorldMailManager* Instance();

	~CWorldMailManager();

	//��ʼ��Mail
	void Init();

	//����GM�ʼ�
	int SendGMMail(const GameMaster_Request& stReq);

	//���͸����ʼ�
	static int SendPersonalMail(const World_SendMail_Request& stReq);

	//��ȡϵͳ�ʼ�
	void GetSystemMail(unsigned uin, int iReason, unsigned uUniqID, int iVIPLevel, const std::string& strChannel, World_GetSystemMail_Response& stResp);

private:

	CWorldMailManager();

	//����ϵͳ�ʼ�
	void SendSystemMail(const GameMaster_Request& stReq, const OneMailInfo& stMailInfo);

	//����
	void SendSystemMailIDNotify();

	//�����ʼ���Ϣ
	void LoadMailInfo();

	//�����ʼ���Ϣ
	void SaveMailInfo();

	//����
	void Reset();

private:

	//ϵͳ�ʼ���ǰ���ID
	unsigned m_uSystemUniqID;

	//�ʼ���Ϣ
	std::vector<SystemMailData> m_vSystemMail;
};
