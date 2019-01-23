
#ifndef __OBJ_STATISTIC_HPP__
#define __OBJ_STATISTIC_HPP__


#include "Statistic.hpp"
#include "SingletonTemplate.hpp"

namespace ServerLib
{
typedef struct tagObjectStatInfo
{
    int m_iObjectType;
    int m_iObjectIndex;
} TObjectStatInfo;

enum enmStatObjectTypeIndex
{
    ESOTI_OBJECT_ITEM_RESERVER1,
    ESOTI_OBJECT_ITEM_RESERVER2,
    ESOTI_OBJECT_ITEM_RESERVER3,
    ESOTI_OBJECT_ITEM_RESERVER4,
    ESOTI_OBJECT_ITEM_RESERVER5,
    ESOTI_OBJECT_ITEM_RESERVER6,
    ESOTI_OBJECT_ITEM_RESERVER7,
    ESOTI_OBJECT_ITEM_RESERVER8,
    ESOTI_OBJECT_ITEM_RESERVER9,
    ESOTI_OBJECT_ITEM_RESERVER10,
    ESOTI_OBJECT_ITEM_RESERVER11,
    ESOTI_OBJECT_ITEM_RESERVER12,
    ESOTI_OBJECT_ITEM_RESERVER13,
    ESOTI_OBJECT_ITEM_RESERVER14,
    ESOTI_OBJECT_ITEM_RESERVER15,
    ESOTI_OBJECT_ITEM_RESERVER16,
    ESOTI_OBJECT_ITEM_RESERVER17,
    ESOTI_OBJECT_ITEM_RESERVER18,
    ESOTI_OBJECT_ITEM_RESERVER19,
    ESOTI_OBJECT_ITEM_RESERVER20,
    ESOTI_OBJECT_ITEM_RESERVER21,
    ESOTI_OBJECT_ITEM_RESERVER22,
    ESOTI_OBJECT_ITEM_RESERVER23,
    ESOTI_OBJECT_ITEM_RESERVER24,
    ESOTI_OBJECT_ITEM_RESERVER25,
    ESOTI_OBJECT_ITEM_RESERVER26,
    ESOTI_OBJECT_ITEM_RESERVER27,
    ESOTI_OBJECT_ITEM_RESERVER28,
    ESOTI_OBJECT_ITEM_RESERVER_MAX,
};

enum enmStatObjectItemIndex
{
    ESOII_OBJECT_ITEM_NUMBER = 0,
    ESOII_OBJECT_ITEM_RESERVER_MAX
};

extern const char* DEFAULT_OBJECT_STAT_DIRECTORY_PATH; //!<����ͳ���ļ�������ļ���
extern const char* DEFAULT_OBJECT_STAT_FILE_NAME; //!<����ͳ���ļ���

const int MAX_STAT_OBJECT_NAME_LENGTH = 64;
extern char g_aszObjectName[ESOTI_OBJECT_ITEM_RESERVER_MAX][MAX_STAT_OBJECT_NAME_LENGTH];

extern const char* g_apszObjectItemName[ESOII_OBJECT_ITEM_RESERVER_MAX];

class CObjStatistic
{
public:
    CObjStatistic(void);
public:
    ~CObjStatistic(void);

public:
    /**
    *��ʼ�����ڳ�ʼ��ʱ������ڴ��CStatistic���е�Section
    *@param[in] pszStatPath ͳ���ļ�·����Ĭ����../stat/
    *@param[in] pszStatFileName ͳ���ļ�����Ĭ����s
    *@return 0 success
    */
    int Initialize(const char* pszStatPath = NULL, const char* pszStatFileName = NULL);

    // ����ͳ������
    void SetObjectStatName(const char** apszObjectName, const int iObjectNumber);

    // ���ĳ����ͳ��
    int AddObjectStat(int iObjectType, int iObjectNumber);

    //!��ӡͳ����Ϣ
    void Print();

    //!���ͳ����Ϣ
    void Reset();

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iObjectErrorNO;
    }

    // ����������Ϣ
    int ReloadLogConfig(TLogConfig& rstLogConfig);

private:

    int AddObjectStatInfo(int iObjectType);

    TObjectStatInfo* GetObjectStatInfo(int iObjectType);

    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iObjectErrorNO = iErrorNO;
    }

private:

    CStatistic m_stObjectStatistic;
    int m_iObjectErrorNO; //!������

    short m_shObjectTypeNum;
    TObjectStatInfo m_astObjectTypeInfo[ESOTI_OBJECT_ITEM_RESERVER_MAX];


};


//!һ����˵ֻ���õ�һ��CObjStatistic�࣬����ʵ��һ����������ʹ��
typedef CSingleton<CObjStatistic> ObjectStatisticSingleton;

}

#endif

