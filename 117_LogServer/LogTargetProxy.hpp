#pragma once

//��־Ŀ��ƽ̨������

#include <string>

#include "DBClientWrapper.hpp"

class LogTargetProxy
{
public:

	//д��־
	virtual int WriteLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iThreadIndex, const std::string& strLogData) = 0;
};
