#pragma once

#include "MsgHandlerSet.hpp"
#include "WriteLogHandler.hpp"

// LogServerӦ���е���Ϣ�����߹�����
class CLogServerHandlerSet : public CMsgHandlerSet
{
public:
    CLogServerHandlerSet();
    ~CLogServerHandlerSet();

    // ��ʼ���ü����е���Ϣ�����ߺ����ݿ����ò��������ݿ�
	virtual int Initialize(int iThreadIndex = -1);

private:
	int RegisterAllHandlers();

private:
	// �ü��Ϲ����������Ϣ������
	CWriteLogHandler m_stWriteLogHandler;

private:
	// ��Ϣ�����ߴ�����Ϣʱ��Ҫ�������ݿ�
	DBClientWrapper m_oDBClient;

	//DB����������
	char* m_pQueryBuff;

	int m_iThreadIdx;
};
