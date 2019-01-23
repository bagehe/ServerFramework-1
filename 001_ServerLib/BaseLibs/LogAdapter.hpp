/**
*@file LogAdapter.hpp
*@author jasonxiong
*@date 2009-11-04
*@version 1.0
*@brief Ϊ����CLogFile��ʹ�ã�д����ƫҵ��Ĵ�ӡ��־��
*
*   ʹ�÷�����
*   ��1��ʹ��TRACESVR��ӡ��Ҫ������־���ش���ı���־�ļ�������ServerLogSingleton::Instance()->SetServerLogName
*   ��2��ʹ��TRACESVR_EX��ӡ�еȼ���������־
*   ��3��TRACE_FUNC_BEG��TRACE_FUNC_END(RET)���ڴ�ӡ������ʼ�ͽ���
*   ��4��ʹ��ERRORLOG��ӡ������־���ش�
*   ��5��ʹ��TRACEPLAYER������Uin��ӡ��־
*   ��6��ʹ��PlayerLogSingleton::Instance()->ClearTraceUinSet���Uin������
*   ��7��ʹ��PlayerLogSingleton::Instance()->AddTraceUin���Uin����������
*/

#ifndef __LOG_ADAPTER_HPP__
#define __LOG_ADAPTER_HPP__

#include "LogFile.hpp"
#include "CommonDef.hpp"
#include "SingletonTemplate.hpp"

#ifndef _GAME_NO_BACKTRACE_
#include "BacktraceInfo.hpp"
#endif //_GAME_NO_BACKTRACE_

typedef struct tagEventId
{
    int m_iEventTime;                       // �¼�������ʱ��
    unsigned short m_ucWorldID : 12;        // �¼�������World
    unsigned short m_ucZoneID  : 4;         // �¼�������Zone
    unsigned short m_usEventSeq;            // �¼����������

    // Ĭ�Ϲ��캯��
    tagEventId()
    {
        m_iEventTime = 0;
    }

} TEventId;

namespace ServerLib
{

class CServerLogAdapter
{
public:
    CServerLogAdapter();
    ~CServerLogAdapter() {}

    int SetServerLogName(const char* pszLogName);
    int SetServerLogSuffix(ENMADDDATESUFFIXTYPE eType);
    int SetLogLevel(int iLogLevel);
    int ReloadLogConfig(TLogConfig& rstLogConfig);

    TLogConfig GetLogConfig()
    {
        return m_stLogFile.GetLogConfig();
    }
    void IncTraceDepth()
    {
        ++m_iTraceDepth;
    }
    void DecTraceDepth()
    {
        --m_iTraceDepth;
    }

public:
    CLogFile m_stLogFile;
    int m_iTraceDepth;
};

//!һ����˵ֻ���õ�һ��CServerLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CServerLogAdapter> ServerLogSingleton;

class CAutoCtrlTraceDepth
{
public:
    CAutoCtrlTraceDepth()
    {
        ServerLogSingleton::Instance()->IncTraceDepth();
    }
    ~CAutoCtrlTraceDepth()
    {
        ServerLogSingleton::Instance()->DecTraceDepth();
    }
};

#ifdef _GAME_NO_LOG_
    #define TRACESVR(format, ...)
#else
    //!������־������־�ȼ���ǿ�ƴ�
    #define TRACESVR(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#define SVRLOGFD ServerLogSingleton::Instance()->m_stLogFile.GetFD
#define SVRLOGFP ServerLogSingleton::Instance()->m_stLogFile.GetFilePointer

#ifdef _GAME_NO_LOG_
    #define TRACESVR_EX(iLogLevel, format, ...)
#else
    //!������־������־�ȼ�
    #define TRACESVR_EX(iLogLevel, format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLevel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

//!���巽����־��ӡ�ĺ�
#ifdef _GAME_NO_LOG_
    #define LOGDEBUG(format, ...)
    #define LOGINFO(format, ...)
    #define LOGDETAIL(format, ...)
    #define LOGWARNING(format, ...)
    #define LOGERROR(format, ...)
#else
    #define LOGDEBUG(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DEBUG, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGINFO(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_INFO, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGDETAIL(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGWARNING(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_WARNING, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
    #define LOGERROR(format, ...) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ERROR, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#ifdef _GAME_NO_LOG_
    #define DUMP_HEX(iLogLevel, szDesc, acBuf, iBufLen)
#else
//!��Buffer��16���Ƶĸ�ʽд��������־�ļ���
#define DUMP_HEX(iLogLevel, szDesc, acBuf, iBufLen) \
        ServerLogSingleton::Instance()->m_stLogFile.WriteLog(iLogLevel, "%s\n", szDesc); \
        ServerLogSingleton::Instance()->m_stLogFile.DumpHex(acBuf, iBufLen);
#endif

//!����������־����־�ȼ�
#define SETTRACELEVEL ServerLogSingleton::Instance()->SetLogLevel

//!����������־���ļ���
#define SETLOGTAG ServerLogSingleton::Instance()->SetServerLogName

//!����������־���ļ�����ʽ
#define SETLOGSUFFIX ServerLogSingleton::Instance()->SetServerLogSuffix

#ifdef _GAME_NO_LOG_
#define TRACE_DEPTH_AUTO_INC
#else
#define TRACE_DEPTH_AUTO_INC CAutoCtrlTraceDepth stTempTraceDepth
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_BEG
#else
//!������־����ӡ������ʼ����־�ȼ�ΪLOG_LEVEL_CALL
#define TRACE_FUNC_BEG ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******beg*******\n")
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_BEG_EX(iLogLevel)
#else
//!������־����ӡ������ʼ����־�ȼ��ɿ�
#define TRACE_FUNC_BEG_EX(iLogLevel) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLevel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******beg*******\n")
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_END
#else
//!������־����ӡ��������������ֵ����־�ȼ�ΪLOG_LEVEL_CALL
#define TRACE_FUNC_END ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n");
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_FUNC_END_EX(iLogLvel)
#else
//!������־����ӡ��������������ֵ����־�ȼ�ΪLOG_LEVEL_CALL
#define TRACE_FUNC_END_EX(iLogLvel) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(iLogLvel, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n");
#endif //_GAME_NO_LOG_

#ifdef _GAME_NO_LOG_
#define TRACE_RETURN(RET) return RET
#else
//!������־����ӡ��������������ֵ����־�ȼ�ΪLOG_LEVEL_CALL
#define TRACE_RETURN(RET) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******\n"); \
        return RET
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_ERROR_RETURN(RET) return RET
#else
//!������־����ӡ��������������ֵ����־�ȼ�ΪLOG_LEVEL_CALL
#define TRACE_ERROR_RETURN(RET) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_CALL, \
            ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "*******end*******, iRet = %d\n", RET); \
        return RET
#endif

#ifdef _GAME_NO_LOG_
#define TRACE_AND_DO(X) X
#else
//!������־����ӡ������䲢ִ�У���־�ȼ�ΪLOG_LEVEL_DETAIL
#define TRACE_AND_DO(X) ServerLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_DETAIL, \
        ServerLogSingleton::Instance()->m_iTraceDepth, __FILE__, __LINE__, __FUNCTION__, "%s\n", #X); X
#endif

template <bool>
struct CompileAssert
{
};

#undef COMPILE_ASSERT
#define COMPILE_ASSERT(expr, msg) \
    typedef CompileAssert<(bool(expr))> msg[bool(expr)? 1: -1]

class CErrorLogAdapter
{
public:
    CErrorLogAdapter();
    ~CErrorLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);

public:
    CLogFile m_stLogFile;
};

//!һ����˵ֻ���õ�һ��CErrorLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CErrorLogAdapter> ErrorLogSingleton;

#ifdef _GAME_NO_LOG_
#define ERRORLOG(format, ...)
#else
#ifndef _GAME_NO_BACKTRACE_
//!������־������־�ȼ���ǿ�ƴ򣬴��ջ
#define ERRORLOG(format, ...) \
    TRACESVR("[ERRORLOG]"format, ##__VA_ARGS__); \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, -1, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__); \
    BacktraceSingleton::Instance()->GenBacktraceInfo(); \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLog(LOG_LEVEL_ANY, "%s\n", \
    BacktraceSingleton::Instance()->GetAllBackTraceInfo())
#else
//!������־������־�ȼ���ǿ�ƴ򣬲����ջ
#define ERRORLOG(format, ...) \
    ErrorLogSingleton::Instance()->m_stLogFile.WriteLogEx(LOG_LEVEL_ANY, -1, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif //_GAME_NO_BACKTRACE_
#endif

// Ĭ�Ͽ���assert
#ifdef _GAME_NO_LOG_
#define __ASSERT_AND_LOG(X) __ASSERT_AND_LOG(X)
#else

#ifdef _ENABLE_ASSERT_
#define __ASSERT_AND_LOG(X) if(!(X)) { ERRORLOG("%s\n", #X); assert(X);}
#else
#define __ASSERT_AND_LOG(X) \
            if(!(X)) { ERRORLOG("%s\n", #X); }
#endif

#endif

// assertʧ�ܺ�, ���ش���ֵ
#define ASSERT_AND_LOG_RTN_INT(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return -1

#define ASSERT_AND_LOG_RTN_BOOL(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return false

#define ASSERT_AND_LOG_RTN_PTR(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return NULL

#define ASSERT_AND_LOG_RTN_VOID(X) \
            __ASSERT_AND_LOG(X); if (!(X)) return

#define ASSERT_AND_LOG_RTN_EX(X, iReturn) \
    __ASSERT_AND_LOG(X); if (!(X)) return (iReturn)


const int MAX_TRACE_UIN_NUMBER = 1024;

class CPlayerLogAdapter
{
public:
    CPlayerLogAdapter();
    ~CPlayerLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int ClearTraceUinSet();
    int AddTraceUin(unsigned int uiUin);
    int WriteLog(unsigned int uiUin, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);

    TLogConfig& GetLogConfig();
    FILE* GetPlayerFile(unsigned int uiUin);
    FILE* GetPlayerProtoFile(unsigned int uiUin);

private:
    int CheckUin(unsigned int uiUin);

private:
    short m_shTraceUinNum;
    unsigned int m_auiTraceUin[MAX_TRACE_UIN_NUMBER];
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;
    CLogFile m_stProtoLogFile;
};

//!һ����˵ֻ���õ�һ��CPlayerLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CPlayerLogAdapter> PlayerLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEPLAYER(uiUin, format, ...) PlayerLogSingleton::Instance()->WriteLog(uiUin, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#else
//!������־��ֻ����TraceUin�б��е�Uin�Ż��
#define TRACEPLAYER(uiUin, format, ...) PlayerLogSingleton::Instance()->WriteLog(uiUin, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#define PLAYERLOGFD PlayerLogSingleton::Instance()->m_stLogFile.GetFD
#define PLAYERLOGFP(uiUin) PlayerLogSingleton::Instance()->GetPlayerFile(uiUin)
#define PLAYERPROTOFILE(uiUin) PlayerLogSingleton::Instance()->GetPlayerProtoFile(uiUin)

#ifdef _GAME_NO_LOG_
#define CLEARTRACEDUIN()
#else
#define CLEARTRACEDUIN() PlayerLogSingleton::Instance()->ClearTraceUinSet()
#endif

#ifdef _GAME_NO_LOG_
#define ADDTRACEDUIN(uiUin)
#else
#define ADDTRACEDUIN(uiUin) PlayerLogSingleton::Instance()->AddTraceUin(uiUin)
#endif
const int MAX_TRACE_MSG_NUMBER = 1024;

class CMsgLogAdapter
{
public:
    CMsgLogAdapter();
    ~CMsgLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int ClearTraceMsgSet();
    int AddTraceMsg(int iMsgID);
    int WriteLog(int iLogLevel, int iMsgID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);
    int WriteLog(int iLogLevel, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);

private:
    int CheckMsg(int iMsgID);

private:
    short m_shTraceMsgNum;
    int m_aiTraceMsg[MAX_TRACE_MSG_NUMBER];
    CLogFile m_stLogFile;
};

//!һ����˵ֻ���õ�һ��CPlayerLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CMsgLogAdapter> MsgLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEMSG(iLogLevel, format, ...)
#else
//!������־��ֻ����TraceMsg�б��е�MsgID�Ż��
#define TRACEMSG(iLogLevel, format, ...) MsgLogSingleton::Instance()->WriteLog(iLogLevel, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

#ifdef _GAME_NO_LOG_
#define TRACEMSG_EX(iLogLevel, iMsgID, format, ...)
#else
//!������־��ֻ����TraceMsg�б��е�MsgID�Ż��
#define TRACEMSG_EX(iLogLevel, iMsgID, format, ...) MsgLogSingleton::Instance()->WriteLog(iLogLevel, iMsgID, \
        __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


class CBillLogAdapter
{
public:
    CBillLogAdapter();
    ~CBillLogAdapter() {}

    int SetBillLogName(const char* pszLogName);
    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int CheckBillPath();

public:
    CLogFile m_stLogFile;
    int m_iCurMonth;
};

//!һ����˵ֻ���õ�һ��CServerLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CBillLogAdapter> BillLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEBILL(format, ...)
#else
//!������־������־�ȼ���ǿ�ƴ�
#define TRACEBILL(format, ...) BillLogSingleton::Instance()->CheckBillPath(); \
        BillLogSingleton::Instance()->m_stLogFile.WriteLog(LOG_LEVEL_ANY, format, ##__VA_ARGS__)
#endif



//////////////////////////////////////////////////////////////////////////
// Thread Log
class CThreadLogAdapter
{
public:
    CThreadLogAdapter();
    ~CThreadLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iThreadID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!һ����˵ֻ���õ�һ��CThreadLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CThreadLogAdapter> ThreadLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACETHREAD(uiUin, format, ...)
#else
#define TRACETHREAD(iThreadID, format, ...) ThreadLogSingleton::Instance()->WriteLog(iThreadID, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Room Log
class CRoomLogAdapter
{
public:
    CRoomLogAdapter();
    ~CRoomLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iRoomID, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!һ����˵ֻ���õ�һ��CRoomLogAdapter�࣬����ʵ�ֳɵ���
typedef CSingleton<CRoomLogAdapter> RoomLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACEROOM(iRoomID, format, ...)
#else
//!������־��ֻ����TraceUin�б��е�Uin�Ż��
#define TRACEROOM(iRoomID, format, ...) RoomLogSingleton::Instance()->WriteLog(iRoomID, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif


//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Client Log
class CClientLogAdapter
{
public:
    CClientLogAdapter();
    ~CClientLogAdapter() {}

    int ReloadLogConfig(TLogConfig& rstLogConfig);
    int WriteLog(int iUin, const char* pszFile, int iLine,
                 const char* pszFunc, const char *pcContent, ...);


private:
    char m_szBaseName[MAX_FILENAME_LENGTH];

    TLogConfig m_stLogConfig;
    CLogFile m_stLogFile;

};

//!һ����˵ֻ���õ�һ��ClientLogSingleton�࣬����ʵ�ֳɵ���
typedef CSingleton<CClientLogAdapter> ClientLogSingleton;

#ifdef _GAME_NO_LOG_
#define TRACECLIENT(iUin, format, ...)
#else
//!������־��ֻ����TraceUin�б��е�Uin�Ż��
#define TRACECLIENT(iUin, format, ...) ClientLogSingleton::Instance()->WriteLog(iUin, \
    __FILE__, __LINE__, __FUNCTION__, format, ##__VA_ARGS__)
#endif

//////////////////////////////////////////////////////////////////////////
}
#endif //__LOG_ADAPTER_HPP__
///:~
