#ifndef __DB_CLIENT_WRAPPER_HPP__
#define __DB_CLIENT_WRAPPER_HPP__

#include <stdlib.h>
#include <string.h>

#include <mysql/mysql.h>

#if MYSQL_VERSION_ID >= 4000
#define mysql_connect(m,h,u,p) mysql_real_connect((m),(h),(u),(p),NULL,0,NULL,0)
#endif

//MYSQL ���ݿ������
struct MysqlConnection
{
    struct MysqlConnection* pNext;

    char szHostAddress[64];
    char szUserName[64];
    char szUserPasswd[128];

    char szDBName[64];

    MYSQL stMySql;

    bool bDBIsConnect; 

    MysqlConnection()
    {
        memset(this, 0 ,sizeof(*this));
    };
};

struct MysqlDBLinkedList
{
    MysqlConnection stMysqlConnection;
    MysqlConnection* pstCurMysqlConnection;

    MysqlDBLinkedList()
    {
        memset(this, 0, sizeof(*this));
    };
};

class DBClientWrapper
{
public:

    DBClientWrapper();

    ~DBClientWrapper();

    void Init(bool bMultiDBConn);

    int SetMysqlDBInfo(const char* pszHostAddr, const char* pszUserName, const char* pszPasswd, const char* pszDBName);

    int CloseMysqlDB();

    int CloseCurMysqlDB();

    MYSQL& GetCurMysqlConn();

    int ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect);

    int ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect, int& iErrNo);

    int FreeResult();

    //����select���ĵ��еĽ��������pstResult�����pLengths����������ÿ���ֶεĳ��ȣ�uFields���з����ֶε�����
    int FetchOneRow(MYSQL_ROW& pstResult, unsigned long*& pLengthes, unsigned int& uFields);

    int GetAffectedRows();
    int GetNumberRows();

    //��ȡ�ϴβ����AUTO_INCREMENT���ص�IDֵ
    unsigned GetLastInsertID();

    const char* GetDBErrString() { return m_szErrString; };

private:

     MysqlDBLinkedList m_stDBLinkedList;    //������MYSQL handler������

     MYSQL_RES* m_pstRes;           //��ǰ������RecordSet������

     int m_iResNum;                 //��ǰ������RecordSet����Ŀ

     bool m_bMultiDBConn;           //�Ƿ�֧�ֶ��MYSQL����

     char m_szErrString[1024];      //ִ��SQL�������в����Ĵ���
};

#endif
