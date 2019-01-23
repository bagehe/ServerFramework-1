/**
*@file LogFile.hpp
*@author jasonxiong
*@date 2009.08.20
*@version 1.0
*@brief ����־��
*
*   1. ����־��¼�ļ���
*   2. ��־�ļ����в��뵱ǰ���ڡ�
*   3. ֧����־���ļ���ű��ݣ��������n����־�ļ������磺run.log.07, run.log.08, run.log.09��
*   4. ��ǰ����������־��ԭrun.log���� (�ļ�������ָ������
*   5. �������ð�pidд�벻ͬ��־�����磺run.5432.log��
*/

#ifndef  __LOG_FILE_HPP__
#define  __LOG_FILE_HPP__

//������ӱ�׼��ͷ�ļ�
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

//�������ServerLibͷ�ļ�
#include "CommonDef.hpp"
#include "ErrorDef.hpp"

namespace ServerLib
{
const int DEFAULT_CHECK_SIZE_STEPS = 2000; //!<Ĭ�϶��ٴδ�ӡ��־�����ļ���С
const int DEFAULT_MAX_FILE_SIZE = 0x10000000; //!<Ĭ��ʱ��������־�ļ�������С
const int DEFAULT_MAX_FILE_COUNT = 5; //!<Ĭ��ʱ����־�ļ������Թ�������
const int MAX_DUMP_HEX_BUF_LENGTH = 32 * 1024; //!<��ӡʮ������ʱ�����Buf��󳤶�
const int MAX_INDENT_DISTANCE = 5; //!<��������ľ��루Tab�ĸ�����
const int DEFAULT_CHECK_FILE_INTERVAL = 60; //!<Ĭ�ϸ����������ļ��Ƿ����
const int DEFAULT_CHECK_CLEANUP_INTERVAL = 24 * 3600;//!<Ĭ�ϸ�����������־�ļ�����

/**
*@brief ����־�ļ����������ں�׺������
*
*   ȡֵ����Ϊ����ö���е�һ��ֵ
*/
typedef enum enmAddDateSuffixType
{
    EADST_DATE_NONE = 0, //!<���������ں�׺
    EADST_DATE_YMD = 1, //!<����YYYYmmdd��ʽ��������
    EADST_DATE_YMDH = 2, //!<����YYYYmmddHH��ʽ��������
} ENMADDDATESUFFIXTYPE;

/**
*@brief TLogConfig�ṹ��m_shLogType��ȡֵ
*
*   ȡֵ����Ϊ����ö���е�����һ��ֵ
*/
typedef enum enmLogType
{
    ELT_LOG_USE_FPRINTF = 0, //!<ÿ��WriteLogʱ��ֱ��ʹ��fprintf��ӡ
    ELT_LOG_AND_FFLUSH = 1, //!<ÿ��WriteLogʱ��ֱ��ʹ��fprintf��ӡ��������fflush��������д���ļ��У�һ������д�˵���Щ�ؼ�����
    ELT_LOG_AND_FCLOSE = 2, //!<ÿ��WriteLogʱ��ֱ��ʹ��fprintf��ӡ��������fclose�����ر��ļ���Ϊ��ֹ�ļ���ɾ��
} ENMLOGTYPE;

/**
*@brief TLogConfig�ṹ��m_iLogLevel��ȡֵ
*
*   ȡֵ����Ϊ����ö���е�����һ��ֵ
*/
typedef enum enmLogLevel
{
    LOG_LEVEL_NONE      = 0x00000000,   //����ӡ��־
    LOG_LEVEL_DEBUG     = 0x00000001,   //DEBUG��־
    LOG_LEVEL_INFO      = 0x00000002,   //INFO��־
    LOG_LEVEL_DETAIL    = 0x00000004,   //DETAIL��־
    LOG_LEVEL_WARNING   = 0x00000008,   //WARNING��־
    LOG_LEVEL_CALL      = 0x00000010,   //����������־
    LOG_LEVEL_ERROR     = 0x00000020,   //������־
    LOG_LEVEL_ANY       = 0xFFFFFFFF,   //ǿ�ƴ�ӡ
} ENMLOGLEVEL;

/**
*@brief ��־�������
*
*   ��־������ã���CLogFile��ʹ�ã�����WriteLog����������������ӡ��־
*/
typedef struct tagLogConfig
{
    char m_szPath[MAX_FILENAME_LENGTH]; //!<��־�ļ�����·��
    char m_szBaseName[MAX_FILENAME_LENGTH]; //!<��־�ļ�����
    char m_szExtName[MAX_FILENAME_LENGTH]; //!<��־�ļ���չ��
    int m_iAddDateSuffixType; //!<���ļ������������ں�׺������
    short m_shLogType; //!<��ӡ��־����
    int m_iLogLevel; //!<��ӡ��־�ȼ�
    int m_iChkSizeSteps; //!<�ڵ���һ����ӡ����������־�ļ���С
    int m_iChkFileInterval; //!<����ļ��Ƿ���ڵ�ʱ��������ҪΪ�˷�ֹ�ļ���ɾ��
    int m_iMaxFileSize; //!<ÿ����־�ļ����Ĵ�С
    int m_iMaxFileCount; //!<����������־�ļ�����
    int m_iMaxFileExistDays;//!<��־��������
} TLogConfig;

class CLogFile
{

public:
    CLogFile();
    virtual ~CLogFile();

    /**
    *��������־��Ϣ
    *@param[in] pszLogPath ��־�ļ�·��
    *@param[in] pszLogName ��־��
    *@param[in] iLogLevel ��־�ȼ�
    *@return 0 success
    */
    int Initialize(const char* pszLogPath, const char* pszLogName, int iLogLevel);

    /**
    *��ʼ����־��ϸ����
    *@param[in] rstLogConfig ��־����
    *@return 0 success
    */
    int Initialize(const TLogConfig& rstLogConfig);

    /**
    *��ȡ��־����
    *@return TLogConfig ���ص�ǰ��־����
    */
    const TLogConfig& GetLogConfig()
    {
        return m_stLogConfig;
    }

    /**
    *�ر���־�ļ�
    *@return 0 success
    */
    int CloseFile();

    /**
    *ʹ�ñ䳤��������ӡ��־������fprintf
    *@param[in] iLoglevel ��־�ȼ�
    *@param[in] pcContent ��־��ʽ��
    *@return 0 success
    */
    int WriteLog(int iLoglevel, const char *pcContent, ...);

    /**
    *ʹ��va����ӡ��־������vfprintf
    *@param[in] iLoglevel ��־�ȼ�
    *@param[in] pcContent ��־��ʽ��
    *@return 0 success
    */
    int WriteLogVA(int iLogLevel, const char *pcContent, va_list& ap);

    /**
    *�䳤������ӡ��־��չ����
    *@param[in] iLoglevel ��־�ȼ�
    *@param[in] pszFile ���ô�ӡ��־�������ļ���
    *@param[in] iLine ���ô�ӡ��־�������к�
    *@param[in] pszFunc ���ô�ӡ��־�����ĺ�����
    *@param[in] pcContent ��־��ʽ��
    *@return 0 success
    */
    int WriteLogEx(int iLoglevel, int iIndentDistance, const char* pszFile, int iLine, const char* pszFunc, const char *pcContent, ...) __attribute__((format(printf, 7, 8)));

    /**
    *va������ӡ��־��չ����
    *@param[in] iLoglevel ��־�ȼ�
    *@param[in] pszFile ���ô�ӡ��־�������ļ���
    *@param[in] iLine ���ô�ӡ��־�������к�
    *@param[in] pszFunc ���ô�ӡ��־�����ĺ�����
    *@param[in] pcContent ��־��ʽ��
    *@return 0 success
    */
    int WriteLogExVA(int iLoglevel, int iIndentDistance, const char* pszFile, int iLine, const char* pszFunc, const char *pcContent, va_list& ap);

    /**
    *��ӡʮ������
    *@param[in] pcBuffer �����ʮ�����ƴ�
    *@param[in] iLength �ֽڴ�����
    *@return 0 success
    */
    int DumpHex(char *pcBuffer, int iLength);

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

    //!��ȡ�ļ�ָ��
    FILE* GetFilePointer()
    {
        OpenLogFile();
        return m_pFile;
    }

    //��ȡ�ļ��׽���
    int GetFD() const;

    //!����־�ļ�
    int OpenLogFile();

private:
    //!���д��־�ȼ�
    int CheckWriteLogLevel(int iLogLevel);
    //!������־
    int BackupLog();
    //!��ȡ��ʵ��־�ļ���
    int GetRealLogName(int iLogIndex, char* szRealName, int iNameSize);

    //!�����־·��
    int CheckPath();
    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

    //!������־�ļ�
    void CleanupLogFile();

    const char* GetLogLevelStr(int iLogLevel);

private:
    int m_iErrorNO; //!������
    TLogConfig m_stLogConfig; //!<��־�ļ�����

    FILE* m_pFile; //!<�ļ�ָ��
    char m_szCurLogName[MAX_FILENAME_LENGTH]; //!<��ǰ�򿪵���־�ļ���

    int m_iLogCount; //!<��ӡ��־�Ĵ���
    time_t m_tLastChkFileTime; //!<��һ�δ�ӡ��־��ʱ��
    time_t m_tLastCleanupFileTime;//!<��һ��������־��ʱ��
    int m_iLastLogHour; //!<��һ�δ�ӡ��־��Сʱ��ÿСʱ���´�һ���ļ�

    //extended function, insert __FILE__, __LINE__, __FUNCTION__ automatically
    int m_iLine; //!<���ô�ӡ��־�������к�
    const char* m_pszFile; //!<���ô�ӡ��־�������ļ���
    const char* m_pszFunc; //!<���ô�ӡ��־�����ĺ�����
    int m_iIndentDistance; //!<��������
    bool m_bEnableFLF; //!<�Ƿ��ӡ�������кš��ļ�����Ϣ
};

}

#endif /* __LOG_FILE_HPP__ */
///:~
