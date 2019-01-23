#include <stdio.h>

#include "StringUtility.hpp"
#include "DBClientWrapper.hpp"

DBClientWrapper::DBClientWrapper()
{
    m_pstRes = NULL;
    m_iResNum = 0;

    m_bMultiDBConn = false;

    memset(m_szErrString, 0, sizeof(m_szErrString));
}

DBClientWrapper::~DBClientWrapper()
{
    //�ͷŲ����Ľ����
    FreeResult();

    //�ر����е�MYSQL������������
    CloseMysqlDB();

    return;
}

void DBClientWrapper::Init(bool bMultiDBConn)
{
    mysql_init(&m_stDBLinkedList.stMysqlConnection.stMySql);

    m_bMultiDBConn = bMultiDBConn;

    m_stDBLinkedList.pstCurMysqlConnection = &(m_stDBLinkedList.stMysqlConnection);
    
    return;
}

int DBClientWrapper::SetMysqlDBInfo(const char* pszHostAddr, const char* pszUserName, const char* pszPasswd, const char* pszDBName)
{
    bool bSelectDB = false;

    //���DB Wrapper��֧�ֶ������
     if(!m_bMultiDBConn)
    {
        if(strcmp(m_stDBLinkedList.pstCurMysqlConnection->szHostAddress, pszHostAddr) != 0)
        {
            if(m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
            {
                CloseMysqlDB();
            }

            if(mysql_connect(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql),pszHostAddr, pszUserName, pszPasswd) == 0)
            {
                sprintf(m_szErrString, "Fail to Connect to Mysql: %s\n", mysql_error(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)));
                return -1;
            }

            m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect = true;
            bSelectDB = true;
        }
        else
        {
            if(!m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
            {
                //������ͬ��DB������ԭ��δ����
                if(mysql_connect(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql),pszHostAddr,pszUserName,pszPasswd) == 0)
                {
                    sprintf(m_szErrString, "Fail to Connect to Mysql: %s\n", mysql_error(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)));
                    return -1;
                }
                
                m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect = true;
                bSelectDB = true;   
            }
        }
    }
    else
    {
        //���֧�ֶ��MYSQL����
        if(strcmp(m_stDBLinkedList.pstCurMysqlConnection->szHostAddress,pszHostAddr) != 0)
        {
            //���ǵ�ǰDB���������Ҫ���ӵ�DB�Ƿ����
            if(m_pstRes)
            {
                mysql_free_result(m_pstRes);
                m_pstRes = NULL;
            }

            //�����ǰMYSQL�Ѿ����ӣ�������Ƿ�����������õ�
            if(m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
            {
                MysqlConnection* pstMyConn = &(m_stDBLinkedList.stMysqlConnection);
                MysqlConnection* pstTailMyConn = &(m_stDBLinkedList.stMysqlConnection);
                m_stDBLinkedList.pstCurMysqlConnection = NULL;

                while(pstMyConn)
                {
                    if(strcmp(pstMyConn->szHostAddress, pszHostAddr) == 0)
                    { m_stDBLinkedList.pstCurMysqlConnection = pstMyConn;
                        break;
                    }

                    pstTailMyConn = pstMyConn;
                    pstMyConn = pstMyConn->pNext;
                }

                //�����ǰû�ҵ����õģ������·���
                if(!m_stDBLinkedList.pstCurMysqlConnection)
                {
                    pstMyConn = new MysqlConnection;
                    mysql_init(&(pstMyConn->stMySql));

                    pstTailMyConn->pNext = pstMyConn;
                    pstMyConn->pNext = NULL;
                    m_stDBLinkedList.pstCurMysqlConnection = pstMyConn;
                }
            }

            MysqlConnection* pstCurrMyConn = m_stDBLinkedList.pstCurMysqlConnection;

            //�����Ѿ���֤current connection �ǿ��õģ����δ���ӣ�������
            if(!pstCurrMyConn->bDBIsConnect)
            {
                if(mysql_connect(&(pstCurrMyConn->stMySql), pszHostAddr,
                                  pszUserName, pszPasswd) == 0)
                {
                    sprintf(m_szErrString, "Fail to connect to Mysql: %s", mysql_error(&(pstCurrMyConn->stMySql)));
                    return -1;
                }

                pstCurrMyConn->bDBIsConnect = true;
                bSelectDB = true;
            }
        }
        else if(!m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
        {
            MysqlConnection* pstCurrMyConn = m_stDBLinkedList.pstCurMysqlConnection;

            //��ǰ����DB Host������ӵ�һ��������δ���ӣ�������
            if(mysql_connect(&(pstCurrMyConn->stMySql), pszHostAddr, pszUserName, pszPasswd) == 0)
            {
                sprintf(m_szErrString, "Fail to Connect to Mysql: %s", mysql_error(&(pstCurrMyConn->stMySql)));
                return -1;
            }
    
            pstCurrMyConn->bDBIsConnect = true;
            bSelectDB = true;
        }
    }

    if(bSelectDB || strcmp(m_stDBLinkedList.pstCurMysqlConnection->szDBName, pszDBName)!=0)
    {
        //��Ҫ����ѡ��DB
        if(mysql_select_db(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql), pszDBName) < 0)
        {
            sprintf(m_szErrString, "Cannot Select Database %s:%s", pszDBName, mysql_error(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)));
            return -1;
        }
    }

    //�����ӵ����ݸ�ֵ
    MysqlConnection* pstCurrConn = m_stDBLinkedList.pstCurMysqlConnection;
    SAFE_STRCPY(pstCurrConn->szDBName, pszDBName, sizeof(pstCurrConn->szDBName));
    SAFE_STRCPY(pstCurrConn->szHostAddress, pszHostAddr, sizeof(pstCurrConn->szHostAddress));
    SAFE_STRCPY(pstCurrConn->szUserName, pszUserName, sizeof(pstCurrConn->szUserName));
    SAFE_STRCPY(pstCurrConn->szUserPasswd, pszPasswd, sizeof(pstCurrConn->szUserPasswd));

    sprintf(m_szErrString, " ");

    return 0;
}

int DBClientWrapper::CloseMysqlDB()
{
    //�����ͷ�result
    if(m_pstRes)
    {
        mysql_free_result(m_pstRes);
        m_pstRes = NULL;
    }

    //���֧�ֶ��MYSQL����
    if(m_bMultiDBConn)
    {
        MysqlConnection* pstMyConn = m_stDBLinkedList.stMysqlConnection.pNext;
        MysqlConnection* pstNextMyConn;

        while(pstMyConn)
        {
            pstNextMyConn = pstMyConn->pNext;

            if(pstMyConn->bDBIsConnect)
            {
                mysql_close(&(pstMyConn->stMySql));
            }

            free(pstMyConn);

            pstMyConn = pstNextMyConn;
        }
    }

    //�ͷ����ݿ����ӵı�ͷ
    if(m_stDBLinkedList.stMysqlConnection.bDBIsConnect)
    {
        mysql_close(&(m_stDBLinkedList.stMysqlConnection.stMySql));

        m_stDBLinkedList.stMysqlConnection.bDBIsConnect = false;
    }

    m_stDBLinkedList.stMysqlConnection.pNext = NULL;

    m_stDBLinkedList.pstCurMysqlConnection = &(m_stDBLinkedList.stMysqlConnection);
    m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect = false;

    return 0;
}

int DBClientWrapper::CloseCurMysqlDB()
{
    mysql_close(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql));

    m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect = false;

    return 0;
}

MYSQL& DBClientWrapper::GetCurMysqlConn()
{
    return m_stDBLinkedList.pstCurMysqlConnection->stMySql;
}

int DBClientWrapper::ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect)
{
    int iErrNo = 0;

    return ExecuteRealQuery(pszQuery, iLength, bIsSelect, iErrNo);
}

int DBClientWrapper::ExecuteRealQuery(const char* pszQuery, int iLength, bool bIsSelect, int& iErrNo)
{
    iErrNo = 0;

    if(bIsSelect)
    {
        //�����select���ͣ���������Ƿ���ȷ
        if(pszQuery[0]!='s' && pszQuery[0]!='S')
        {
            sprintf(m_szErrString, "Invalid Query Type, SQL Not Select!");
            return -1;
        }
    }

    //���ͷ�֮ǰQuery�Ľ��
    if(m_pstRes)
    {
        mysql_free_result(m_pstRes);
        m_pstRes = NULL;
    }

    if(!m_stDBLinkedList.pstCurMysqlConnection || !m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
    {
        sprintf(m_szErrString, "Has Not Connect to MYSQL DB Server!");
        return -1;
    }

    int iRet = 0;
    if(mysql_ping(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)))
    {
        CloseCurMysqlDB();
        
        MysqlConnection* pCurrConn = m_stDBLinkedList.pstCurMysqlConnection;

        //����ѡ��ǰDB
        iRet = SetMysqlDBInfo(pCurrConn->szHostAddress, pCurrConn->szUserName, pCurrConn->szUserPasswd, pCurrConn->szDBName);
        if(iRet)
        {
            return -1;
        }
    }

    //ȷ�ϵ�ǰDB���ã�ִ��query
    iRet = mysql_real_query(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql), pszQuery, iLength);
    if(iRet != 0)
    {
        iErrNo = mysql_errno(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql));
        sprintf(m_szErrString, "Fail to Execute SQL: %s", mysql_error(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)));

        return -1;
    }

    if(bIsSelect)
    {
        m_pstRes = mysql_store_result(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql));
        if(!m_pstRes)
        {
            //��ȡ�����ʧ��
            iErrNo = mysql_errno(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql));
            sprintf(m_szErrString, "Fail to store result: %s", mysql_error(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql)));

            return -2;
        }

        m_iResNum = mysql_num_rows(m_pstRes);
    }

    return 0;
}

int DBClientWrapper::FreeResult()
{
    if(m_pstRes)
    {
        mysql_free_result(m_pstRes);
        m_pstRes = NULL;
    }

    return 0;
}

int DBClientWrapper::FetchOneRow(MYSQL_ROW& pstResult, unsigned long*& pLengthes, unsigned int& uFields)
{
    if(!m_pstRes)
    {
        sprintf(m_szErrString, "RecordSet is NULL!");
        return -1;
    }

    if(m_iResNum == 0)
    {
        sprintf(m_szErrString, "RecordSet count=0!");
        return -2;
    }

    pstResult = mysql_fetch_row(m_pstRes);
    pLengthes = mysql_fetch_lengths(m_pstRes);
    uFields = mysql_num_fields(m_pstRes);

    if(!pstResult || !pLengthes)
    {
        sprintf(m_szErrString, "Fail to fetch record row or length!");
        return -3;
    }

    return 0;
}

int DBClientWrapper::GetAffectedRows()
{
    if(!m_stDBLinkedList.pstCurMysqlConnection || !m_stDBLinkedList.pstCurMysqlConnection->bDBIsConnect)
    {
        sprintf(m_szErrString, "Has Not Connect to DB Server!");
        return 0;
    }

    return mysql_affected_rows(&(m_stDBLinkedList.pstCurMysqlConnection->stMySql));
}

int DBClientWrapper::GetNumberRows()
{
    return m_iResNum;
}

//��ȡ�ϴβ����AUTO_INCREMENT���ص�IDֵ
unsigned DBClientWrapper::GetLastInsertID()
{
    return mysql_insert_id(&m_stDBLinkedList.pstCurMysqlConnection->stMySql);
}
