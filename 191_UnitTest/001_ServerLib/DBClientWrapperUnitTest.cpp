#include <stdlib.h>

#include "google/protobuf/text_format.h"
#include "ErrorNumDef.hpp"
#include "StringUtility.hpp"
#include "GameProtocol.hpp"

#include "DBClientWrapperUnitTest.hpp"

using namespace ServerLib;

void DBClientWrapperUnitTest::SetUp()
{
    m_oDBClient.Init(true);

    return;
}

void DBClientWrapperUnitTest::TearDown()
{
    return;
}

TEST_F(DBClientWrapperUnitTest, DBClientTest)
{
    //��������
    int iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    //����ִ��SQL���
    iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
   
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);
}

TEST_F(DBClientWrapperUnitTest, DBClientSelectTest)
{
    //������MYSQL���ݿ������
    int iRet = m_oDBClient.SetMysqlDBInfo("192.168.78.144", "qmonster", "kingnetdevelop", "1_RoleDB");
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    //������ȡ�������
    static char szQueryString[4096] = {0};
    int iLength = SAFE_SPRINTF(szQueryString, sizeof(szQueryString)-1, "select %s from t_qmonster_userdata where uin = %d and worldID=%d", "base_info", 10001, 1);

    //��һ�����Ϣ base_info
    iRet = m_oDBClient.ExecuteRealQuery(szQueryString, iLength, true);
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    MYSQL_ROW pstResult = NULL;
    unsigned long* pLengths = NULL;
    unsigned int uFields = 0;

    iRet = m_oDBClient.FetchOneRow(pstResult, pLengths, uFields);
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);
    ASSERT_EQ(1u, uFields);

    BASEDBINFO stBaseInfo;
    bool bRet = stBaseInfo.ParseFromArray(pstResult[0], pLengths[0]);
    EXPECT_TRUE(bRet);

    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    //��ӡprotobuf���ݣ�ȷ������������
    std::string strTmp;
    ::google::protobuf::TextFormat::PrintToString(stBaseInfo, &strTmp);

    printf("\n\n%s\n\n\n", strTmp.c_str());

}
