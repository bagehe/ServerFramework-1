#pragma once

#include <vector>
#include "LogAdapter.hpp"

static const int THREADLOG_THREAD_NUM = 16;		//��־֧���߳��������

class CThreadLogManager
{
public:
	static CThreadLogManager* Instance();
	~CThreadLogManager();

	int Initialize(const int iLogNum, const ServerLib::TLogConfig& rLogConfig);
	ServerLib::CServerLogAdapter* GetLogAdapter(const int iThreadIdx);

private:
	CThreadLogManager();

private:
	int m_iLogNum;
	ServerLib::CServerLogAdapter m_astLogAdapter[THREADLOG_THREAD_NUM];
};

#ifdef _GAME_NO_LOG_
#define TRACE_THREAD(iThreadIdx, format, ...)
#else
//!������־������־�ȼ���ǿ�ƴ�
#define TRACE_THREAD(iThreadIdx, format, ...) CThreadLogManager::Instance()->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(ServerLib::LOG_LEVEL_ANY, CThreadLogManager::Instance()->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)	
#endif																

#ifdef _GAME_NO_LOG_													
#define DEBUG_THREAD(iThreadIdx, format, ...)								
#else															
//!������־��DEBUG��־��LOG_LEVEL_DETAIL�ȼ�
#define DEBUG_THREAD(iThreadIdx, format, ...) CThreadLogManager::Instance()->GetLogAdapter(iThreadIdx)->m_stLogFile.WriteLogEx(ServerLib::LOG_LEVEL_DETAIL, CThreadLogManager::Instance()->GetLogAdapter(iThreadIdx)->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif
