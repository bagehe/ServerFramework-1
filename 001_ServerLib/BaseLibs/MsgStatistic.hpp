/**
*@file MsgStatistic.hpp
*@author jasonxiong
*@date 2009-11-20
*@version 1.0
*@brief ��Ϣͳ����
*
*
*/

#ifndef __MSG_STATISTIC_HPP__
#define __MSG_STATISTIC_HPP__

#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{
const int MAX_STAT_MSG_NUMBER = 4096; //���ͳ�Ƶ���Ϣ������
const int MAX_STAT_MSG_TYPE = 4; // ���ͳ����Ϣ����
const int MAX_STAT_LOOP_STAT = 1;

//!����ͨ��MsgID���㶨λ����CStatistic�е�Section����
typedef struct tagStatMsgInfo
{
    int m_iMsgID; //!<��ϢID
    int m_iMsgIndex; //!<��Ϣ��CStaitistic�е�Section����
} TStatMsgInfo;


typedef struct tagStatMsgTypeInfo
{
    int m_iMsgType; //!<��Ϣ����
    int m_iMsgTypeIndex; //!<��Ϣ��CStaitistic�е�Section����
} TStatMsgTypeInfo;

typedef enum enmStatMsgType
{
    ESMT_FROM_CLIENT = 0, // �ӿͻ����յ���Ϣ
    ESMT_FROM_SERVER = 1, // �ӷ������յ���Ϣ
    ESMT_TO_CLIENT = 2,   // ���͸��ͻ���
    ESMT_TO_SERVER = 3,   // ���͸�������
} ENMSTATMSGTYPE;

typedef enum enmStatMsgResult
{
    ESMR_SUCCEED = 0, //!<��Ϣ����ɹ�
    ESMR_FAILED = 1, //!<��Ϣ����ʧ��
    ESMR_TIMEOUT = 2, //!<��Ϣ����ʱ
} ENMSTATMSGRESULT;

/**
*@brief ��Ϣ��Ҫͳ�Ƶ�ͳ����
*
*
*/
typedef enum enmStatMsgItemIndex
{
    ESMI_SUCCESS_MSG_NUMBER_IDX = 0, //!<��Ϣ����ɹ��ĸ���
    ESMI_FAILED_MSG_NUMBER_IDX, //!<��Ϣ����ʧ�ܵĸ���
    ESMI_TIMEOUT_MSG_NUMBER_IDX, //!<��Ϣ����ʱ�ĸ���
    ESMI_SUM_PROCESSTIME_IDX, //!<�ܴ����ʱ
    ESMI_MAX_PROCESSTIME_IDX, //!<������ʱ
    ESMI_AVA_PROCESSTIME_IDX, //!<ƽ�������ʱ
    ESMI_MSG_LENGTH,  // ��Ϣ����
    ESMI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMSTATMSGITEMINDEX;

typedef enum enmStatMsgTypeItemIndex
{
    ESMTI_TOTAL_MSG_NUMBER = 0, // ��Ϣ����
    ESMTI_TOTAL_SUCCESS_MSG_NUMBER, // �ɹ�����Ϣ����
    ESMTI_TOTAL_FAILED_MSG_NUMBER, // ʧ�ܵ���Ϣ����
    ESMTI_TOTAL_TIMEOUT_NUMBER, // ��ʱ����Ϣ����
    ESMTI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMSTATMSGTYPEITEMINDEX;

typedef enum enmStatLoopItemIndex
{
    ESLI_LOOP_NUMBER = 0,  // Loop����
    ESLI_MAX_ITEM_NUMBER //!<�ܹ���Ҫͳ�Ƶ�ͳ�����������֤���ֵ���ᳬ��Section�����ɵ�Item���ֵ
} ENMSTATLOOPITEMINDEX;

extern const char* DEFAULT_MSG_STAT_DIRECTORY_PATH; //!<��Ϣͳ���ļ�������ļ���
extern const char* DEFAULT_MSG_STAT_FILE_NAME; //!<��Ϣͳ���ļ���
extern const char* g_apszMsgItemName[ESMI_MAX_ITEM_NUMBER];
extern const char* g_apszMsgTypeItemName[ESMTI_MAX_ITEM_NUMBER];
extern const char* g_apszLoopItemName[ESLI_MAX_ITEM_NUMBER];

class CMsgStatistic
{
public:
    CMsgStatistic();
    ~CMsgStatistic();

public:
    /**
    *��ʼ�����ڳ�ʼ��ʱ������ڴ��CStatistic���е�Section
    *@param[in] pszStatPath ͳ���ļ�·����Ĭ����../stat/
    *@param[in] pszStatFileName ͳ���ļ�����Ĭ����s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    //!���ͳ����Ϣ
    int AddMsgStat(int iMsgID, short shResult, int iMsgLength, timeval tvProcessTime, int iMsgType, int iMsgTimes = 1);

    // ���Loopͳ����Ϣ
    int AddLoopStat(int iLoopNumber);

    //!��ӡͳ����Ϣ
    void Print();

    //!���ͳ����Ϣ
    void Reset();

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iMsgErrorNO;
    }

    // ����������Ϣ
    int ReloadLogConfig(TLogConfig& rstLogConfig);

private:
    //!ͨ��MsgIDѰ�ҵ���Ӧ��TStatMsgInfo�ṹ
    TStatMsgInfo* GetStatMsgInfo(int iMsgID);

    TStatMsgTypeInfo* GetStatMsgTypeInfo(int iMsgType);

    //!����MsgID��ͳ��
    int AddMsgInfo(int iMsgID);

    // ���MsgTypeͳ��
    int AddMsgTypeInfoStat(int iMsgType, short shResult, int iMsgTimes = 1);

    // ����MsgType��ͳ��
    int AddMsgTypeInfo(int iMsgID);

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iMsgErrorNO = iErrorNO;
    }

private:

    CStatistic m_stMsgStatistic;
    int m_iMsgErrorNO; //!������

    short m_shMsgNum;
    TStatMsgInfo m_astMsgInfo[MAX_STAT_MSG_NUMBER];

    CStatistic m_stMsgTypeStatistic;

    short m_shMsgTypeNum;
    TStatMsgTypeInfo m_astMsgTypeInfo[MAX_STAT_MSG_TYPE];

    CStatistic m_stLoopStatistic;
    bool m_bAddedLoopFlag;
};

//!һ����˵ֻ���õ�һ��CMsgStatistic�࣬����ʵ��һ����������ʹ��
typedef CSingleton<CMsgStatistic> MsgStatisticSingleton;

}

#endif //__MSG_STATISTIC_HPP__
///:~
