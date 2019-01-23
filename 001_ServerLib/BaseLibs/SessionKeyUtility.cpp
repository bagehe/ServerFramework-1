#include <stdlib.h>

#include "StringUtility.hpp"
#include "LogAdapter.hpp"

#include "SessionKeyUtility.hpp"

using namespace ServerLib;

//����uin���ɵ�¼��ҵ�Session Key
int CSessionKeyUtility::GenerateSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen)
{
    if(!pszOutBuff || !pszInBuff)
    {
        TRACESVR("Failed to generate session key, invalid param!");
        return -1;
    }

    std::string strPlain;
    strPlain.assign(pszInBuff, iInBuffLen);

    //ʹ����ԿKey���м���
    CAESCbcEncryptor stEncryptor;
    int iRet = stEncryptor.SetAESKey(SESSION_ENCRYPT_KEY, sizeof(SESSION_ENCRYPT_KEY));
    if(iRet)
    {
        TRACESVR("Failed to set AES encryptor key, ret %d\n", iRet);
        return -1;
    }

    std::string strClipher = stEncryptor.DoAESCbcEncryption(strPlain);
    if(strClipher.size() == 0)
    {
        TRACESVR("Failed to do AES encryption, ret %d\n", iRet);
        return -2;
    }

    //ʹ��Base64���б���
    iRet = base64Encode(strClipher.c_str(), strClipher.size(), pszOutBuff, iOutBuffLen);
    if(iRet)
    {
        TRACESVR("Failed to do base64 encode, ret %d\n", iRet);
        return -3;
    }

    return 0;
}

//����Session Key���봫����ַ���
int CSessionKeyUtility::DecryptSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen)
{
    if(!pszInBuff || !pszOutBuff)
    {
        TRACESVR("Failed to decrypt session key, invalid param!\n");
        return -1;
    }

    //����Base64����
    char szDecodeData[1024]={0};
    int iDecodeBuffLen = sizeof(szDecodeData)-1;

    int iRet = base64Decode(pszInBuff, iInBuffLen, szDecodeData, iDecodeBuffLen);
    if(iRet)
    {
        TRACESVR("Failed to base64 decode data, ret %d\n", iRet);
        return iRet;
    }

    std::string strClipher;
    strClipher.assign(szDecodeData, iDecodeBuffLen);

    //ʹ����ԿKey���н���
    CAESCbcEncryptor stDecryptor;
    iRet = stDecryptor.SetAESKey(SESSION_ENCRYPT_KEY, sizeof(SESSION_ENCRYPT_KEY));
    if(iRet)
    {
        TRACESVR("Failed to set AES decryptor key, ret %d\n", iRet);
        return -1;
    }

    std::string strPlain = stDecryptor.DoAESCbcDecryption(strClipher);;
    if(strPlain.size() == 0)
    {
        TRACESVR("Failed to do AES decryption, ret %d\n", iRet);
        return -2;
    }

    iOutBuffLen = strPlain.size();
    SAFE_STRCPY(pszOutBuff, strPlain.c_str(), iOutBuffLen);

    return 0;
}
