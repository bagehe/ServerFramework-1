#ifndef __HASH_UTILITY_HPP__
#define __HASH_UTILITY_HPP__

//�Ը��ֳ��õ�Hash�㷨�ķ�װ

class CHashUtility
{
public:

    //Robert Sedgwicks hash
    static long RsHash(const char* pszStr, int iLen);

    //Justin Sobel hash
    static long JSHash(const char* pszStr, int iLen);

    //PJW hash
    static long PJWHash(const char* pszStr, int iLen);

    //ELF hash, just like PJW
    static long ELFHash(const char* pszStr, int iLen);

    //BKDR hash
    static long BKDRHash(const char* pszStr, int iLen);

    //SDBM hash
    static long SDBMHash(const char* pszStr, int iLen);

    //DJB hash, Ŀǰ���ϵ�����Ч��hash�㷨
    static long DJBHash(const char* pszStr, int iLen);

    //DEK hash
    static long DEKHash(const char* pszStr, int iLen);
};

#endif
