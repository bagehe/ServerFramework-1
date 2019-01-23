#pragma once

#include "LogTargetProxy.hpp"

class CMofangLogProxy : public LogTargetProxy
{
public:

	//дħ����־
	virtual int WriteLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iThreadIndex, const std::string& strLogData);

private:

	//�������ݿ������
	int GenerateQueryString(char* pQueryBuff, int& iLength, int iThreadIndex, const std::string& strLogData);

	//�����ݿ�д����־
	int WriteMofangLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iLength, int iThreadIndex);
};
