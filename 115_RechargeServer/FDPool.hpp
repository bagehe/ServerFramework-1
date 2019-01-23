#pragma once

#include "AppDefine.hpp"
#include "ExternalClientPool.hpp"

class CFDPool
{
public:

	//��ʼ��External��Internal�׽��ֳ�
	int Initialize();

	//��ָ��FD����Ϊ��
	void SetFDInactive(int iFD);

	// ���ü���
	void SetFDActive(int iFD);

	//�Ƿ����ⲿ�����׽���
	bool IsListeningForExternalClient(int iFD);

	//����Ϊ�ⲿ�����׽���
	void SetListeningForExternalClient(int iFD);

	//�Ƿ����ⲿ�����׽���
	bool IsConnectedByExternalClient(int iFD);

	//����Ϊ�ⲿ�����׽���
	void SetConnectedByExternalClient(int iFD);

	TExternalClientSocket* GetExternalSocketByFD(int iFD);
	TExternalClientSocket* GetExternalSocketBySession(unsigned uiSessionFD);

private:
	unsigned int m_auiSocketFlag[FD_SIZE];

	CExternalClientPool m_stExternalClientPool;
};
