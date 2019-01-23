#ifndef __SESSION_KEY_UTILITY_HPP__
#define __SESSION_KEY_UTILITY_HPP__

#include "AESCbcEncryptor.hpp"
#include "Base64.hpp"

//Session Key ���ɺ�У�鹤���࣬���ڱ�֤��¼��ҵ� session ��ȫ

//˵���� Session Key �������㷨Ϊ��
//          1.�����߼�Ҫ�������ַ�������;
//          2.ʹ�� AES �㷨 ECBģʽ �����ɵ��ַ���ʹ�� Key ���м���;
//          3.ʹ�� Base64 �Զ����ƴ����� Base64 ���룬���㴫��;
//          4.SessionKey�Ľ����㷨Ϊ�����㷨�������; 

#define SESSION_ENCRYPT_KEY "FkuL9QD1IjWpbOh"

class CSessionKeyUtility
{
public:
    //���ݴ����ַ������ɵ�¼��ҵ�Session Key
    static int GenerateSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen);

    //����Session Key���봫����ַ���
    static int DecryptSessionKey(const char* pszInBuff, int iInBuffLen, char* pszOutBuff, int& iOutBuffLen);
};

#endif
