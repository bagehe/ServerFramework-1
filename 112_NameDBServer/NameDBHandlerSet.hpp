#pragma once

#include "MsgHandlerSet.hpp"
#include "AddNameHandler.hpp"
#include "DeleteNameHandler.hpp"

// NameDBӦ���е���Ϣ�����߹�����
class CNameDBHandlerSet : public CMsgHandlerSet
{
public:
    CNameDBHandlerSet();
    ~CNameDBHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:
	// �ü��Ϲ����������Ϣ������
	CAddNameHandler m_stAddNameHandler;
	CDeleteNameHandler m_stDeleteNameHandler;

private:
	// ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
	DBClientWrapper m_oDBClient;

	//DB����������
	char* m_pQueryBuff;

	int m_iThreadIdx;
};
