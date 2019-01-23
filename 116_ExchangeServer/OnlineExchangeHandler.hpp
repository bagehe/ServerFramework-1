#pragma once

#include <string>

#include "MsgHandler.hpp"

//���̴߳���Handler
class COnlineExchangeHandler : public CMsgHandler
{
public:
	COnlineExchangeHandler();

	//��������������
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

	//�����߳�index
	void SetThreadIdx(int iThreadIndex);

private:

	//���߶һ�
	int OnRequestOnlineExchange(SHandleResult& stHandleResult);

	//��װ���ѳ�ֵ����
	std::string PackBillRequestParams(const OnlineExchange& stInfo);

	//�������߶һ�����
	int ProcessOnlineExchange(unsigned uin, const std::string& strAPI, const std::string& strParams);

	//��ȡSP������
	const std::string GetSpOrderID();

private:

	// ���������Ϣ
	GameProtocolMsg* m_pstRequestMsg;

	//�߳�idnex
	int m_iThreadIndex;
};
