#ifndef __PASSWORD_ENCRYPTION_UTILITY_HPP__
#define __PASSWORD_ENCRYPTION_UTILITY_HPP__

#include "AESCbcEncryptor.hpp"

//�û� Password ���ܹ�����

//ֱ��ʹ��MD5�㷨���ܣ�������

//�����õ�Key
#define PASSWORD_ENCRYPT_KEY "FkuL+QD1IjWp-Oh"

class CPasswordEncryptionUtility
{
public:
    //��������м��ܣ��������������
    static int DoPasswordEncryption(const char* pszPasswd, int iPasswdLen, char* pszOutBuff, int& iOutBuffLen);
};

#endif
