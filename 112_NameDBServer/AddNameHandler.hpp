#pragma once

#include "DBClientWrapper.hpp"
#include "MsgHandler.hpp"
#include "AppDef.hpp"
#include "ErrorNumDef.hpp"

//����������ֵ�����
// MSGID_ADDNEWNAME_REQUEST ��Ϣ������
class CAddNameHandler : public CMsgHandler
{
public:
	CAddNameHandler(DBClientWrapper* pDatabase, char** ppQueryBuff);

	void SetThreadIdx(const int iThreadIdx){m_iThreadIdx = iThreadIdx;}

	//��������������Ҫ��
	virtual void OnClientMsg(GameProtocolMsg& stReqMsg, SHandleResult* pstHandleResult = NULL, TNetHead_V2* pstNetHead = NULL);

private:

    //���б�Ҫ�Ĳ������
    int CheckParams(const AddNewName_Request& stReq);

    void OnAddNameRequest(SHandleResult& stHandleResult);

    int CheckNameExist(const std::string& strName, int iType, bool& bIsExist);

    //�����µļ�¼
    int AddNewRecord(const std::string& strName, int iNameType, unsigned uNameID);

private:
	DBClientWrapper* m_pDatabase;		//�������ݿ��ָ��
	GameProtocolMsg* m_pstRequestMsg;	//���������Ϣ
	char** m_ppQueryBuff;				//DB����������					
	int m_iThreadIdx;					//�����߳�idx
};
