#pragma once

#include "LogTargetProxy.hpp"

//TapDB��־������

class CTapDBLogProxy : public LogTargetProxy
{
public:
	
	//д��־
	virtual int WriteLog(DBClientWrapper* pDataBase, char* pQueryBuff, int iThreadIndex, const std::string& strLogData);
};
