#ifndef __MSG_OVERVIEW_STATISTIC_HPP__
#define __MSG_OVERVIEW_STATISTIC_HPP__


#include "SingletonTemplate.hpp"

namespace ServerLib
{
class CMsgOverViewStatistic
{

public:

    CMsgOverViewStatistic(void);

public:

    ~CMsgOverViewStatistic(void);

public:

    void ClearAllStat();

    int RecordAllStat(int iIntervalTime);

    inline void AddMsgNumberFromClient(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberFromClient += iMsgNumber;
        m_iMsgSizeFromClient += iPacketSize;
    };

    inline void AddMsgNumberFromServer(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberFromServer += iMsgNumber;
        m_iMsgSizeFromServer += iPacketSize;
    };


    inline void AddMsgNumberToClient(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberToClient += iMsgNumber;
        m_iMsgSizeToClient += iPacketSize;
    };

    inline void AddMsgNumberToServer(int iMsgNumber, int iPacketSize)
    {
        m_iMsgNumberToServer += iMsgNumber;
        m_iMsgSizeToServer += iPacketSize;
    };

    inline void AddLoopNumber(int iLoopNumber)
    {
        m_iLoopNumber += iLoopNumber;
    }

public:

    // ���Կͻ��˵���Ϣ���Ͱ���С
    int m_iMsgNumberFromClient;
    int m_iMsgSizeFromClient;

    // ���Է���������Ϣ���Ͱ���С
    int m_iMsgNumberFromServer;
    int m_iMsgSizeFromServer;

    // ���͸��ͻ��˵���Ϣ���Ͱ���С
    int m_iMsgNumberToClient;
    int m_iMsgSizeToClient;

    // ���͸�����������Ϣ���Ͱ���С
    int m_iMsgNumberToServer;
    int m_iMsgSizeToServer;

    // ����Loopѭ��
    int m_iLoopNumber;
};

typedef CSingleton<CMsgOverViewStatistic> MsgOverViewStatisticSingleton;

}


#endif



