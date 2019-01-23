#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"

//�����ʺ��������Ĵ���
// MSGID_REGAUTHDB_ADDACCOUNT_REQUEST ��Ϣ������
class CAddRegAuthHandler : public CMsgHandler
{
public:
	CAddRegAuthHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

    //���б�Ҫ�Ĳ������
    int CheckParams(const RegAuthDB_AddAccount_Request& stReq);

    void OnAddRegAuthRequest(SHandleResult& stHandleResult);

    //����ʺ��Ƿ����
    int CheckAccountExist(const AccountInfo& stInfo, bool& bIsExist);

    //��ȡ��δ��ʹ�õ�UIN
    int GetAvaliableUin(unsigned int& uin);

    //�����µļ�¼
    int AddNewRecord(const AccountInfo& stInfo, unsigned int uin, int iWorldID);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//DB����������					
	int m_iThreadIdx;					//�����߳�idx
};
