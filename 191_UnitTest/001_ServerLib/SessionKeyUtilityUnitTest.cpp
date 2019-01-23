#include <stdlib.h>

#include "StringUtility.hpp"
#include "ErrorNumDef.hpp"
#include "SessionKeyUtility.hpp"
#include "PasswordEncryptionUtility.hpp"

#include "SessionKeyUtilityUnitTest.hpp"

using namespace ServerLib;

void SessionKeyUtilityUnitTest::SetUp()
{
    m_uin = 10000;

    m_iSessionKeyLen = 0;

    memset(m_szSessionKey, 0, sizeof(m_szSessionKey));
}

void SessionKeyUtilityUnitTest::TearDown()
{

}

TEST_F(SessionKeyUtilityUnitTest, GenerateTest)
{
    //����Session Key
    m_iSessionKeyLen = sizeof(m_szSessionKey)-1;

    static char szOriginalString[256] = "NewGameServer";
    int iRet = CSessionKeyUtility::GenerateSessionKey(szOriginalString, strlen(szOriginalString), m_szSessionKey, m_iSessionKeyLen);

    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    return;
}

TEST_F(SessionKeyUtilityUnitTest, DecryptTest)
{
    //����Session Key
    m_iSessionKeyLen = sizeof(m_szSessionKey)-1;

    static char szOriginalString[256] = "NewGameServer";
    static int iOriginalBuffLen = sizeof(szOriginalString);
    int iRet = CSessionKeyUtility::GenerateSessionKey(szOriginalString, strlen(szOriginalString), m_szSessionKey, m_iSessionKeyLen);
    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    //�������ɵ�SessionKey
    printf("Encrypt Session key: %s, len %d\n", m_szSessionKey, m_iSessionKeyLen);

    iRet = CSessionKeyUtility::DecryptSessionKey(m_szSessionKey, m_iSessionKeyLen, szOriginalString, iOriginalBuffLen);

    EXPECT_EQ(T_SERVER_SUCCESS, iRet);

    printf("original string %s\n", szOriginalString);

    return; 
}

TEST_F(SessionKeyUtilityUnitTest, PasswordEncryptTest)
{
    char szPassword[64] = {"xiong123_zhen111"};
    char szOutBuff1[128] = {0};
    int iOutBuff1Len = sizeof(szOutBuff1)-1;

    int iRet = CPasswordEncryptionUtility::DoPasswordEncryption(szPassword, strlen(szPassword), szOutBuff1, iOutBuff1Len);
    EXPECT_EQ(0, iRet);

    char szOutBuff2[128] = {0};
    int iOutBuff2Len = sizeof(szOutBuff2)-1;

    iRet = CPasswordEncryptionUtility::DoPasswordEncryption(szPassword, strlen(szPassword), szOutBuff2, iOutBuff2Len);
    EXPECT_EQ(0, iRet);
    
    EXPECT_EQ(0, SAFE_STRCMP(szOutBuff1,szOutBuff2, strlen(szOutBuff1)));

    printf("real password string: %s\n", szPassword);
    printf("encrypt password string: %s, len %d\n", szOutBuff1, iOutBuff1Len);

    return; 
}
