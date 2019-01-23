#ifndef __AES_CBC_ENCRYPTOR_HPP__
#define __AES_CBC_ENCRYPTOR_HPP__

#include <string>

#include "botan/botan.h"

//����ļ��Ƕ�AES�㷨�ķ�װ��ʹ�õ���botan�е�ʵ��
//ʹ�õ���ECB���ܣ�ֻʹ�ü���Key

using namespace Botan;

class CAESCbcEncryptor
{
public:
    CAESCbcEncryptor();
    ~CAESCbcEncryptor();

    //����AES����ʹ�õ� Key
    int SetAESKey(const char* pszAESKey, int iAESKeyLen);

    //ʹ��AES CBCģʽ�����Ĵ����м���
    std::string DoAESCbcEncryption(const std::string& strInput);

    //ʹ��AES CBCģʽ�Լ��ܴ����н���
    std::string DoAESCbcDecryption(const std::string& strInput);

private:

    //������
    Pipe* m_pstEncryptor;

    //������
    Pipe* m_pstDecryptor;
};

#endif
