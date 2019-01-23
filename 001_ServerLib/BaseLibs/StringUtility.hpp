/**
*@file StringUtility.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief �ַ�������������
*
*
*/

#ifndef __STRING_UTIL_HPP__
#define __STRING_UTIL_HPP__

#define SAFE_STRCPY(pszDest, pszSrc, iDestBufLen) \
({ \
    char* pStr = strncpy(pszDest, pszSrc, (iDestBufLen)); \
    (pszDest)[(iDestBufLen) - 1] = '\0'; \
    pStr; \
})

#define SAFE_STRCAT(pszDest, pszSrc, iDestBufLen)               \
({                                                              \
    int iLeftLen = iDestBufLen - strlen(pszDest) - 1;           \
    char* pStr = strncat(pszDest, pszSrc, iLeftLen);        \
    (pszDest)[(iDestBufLen) - 1] = '\0';                        \
    pStr;                                                       \
})                                                          \
 
#define SAFE_STRLEN(pszStr, iStrBufLen) \
({ \
    strnlen(pszStr, (iStrBufLen)); \
})

#define SAFE_STRCMP(pszStr1, pszStr2, iMinStrBufLen) \
({ \
    strncmp(pszStr1, pszStr2, (iMinStrBufLen)); \
})

//NOTE: ����ط����봫��ʵ��ʣ�໺�����Ĵ�С�������п��ܱ���
#define SAFE_SPRINTF(pszBuf, iBufLen, pszFormat, ...) \
({ \
    int iSafeRet = snprintf(pszBuf, iBufLen, pszFormat, ##__VA_ARGS__); \
    (pszBuf)[iBufLen - 1] = '\0'; \
    (unsigned int)iSafeRet < (unsigned int)iBufLen? iSafeRet : iBufLen - 1; \
})

#define SAFE_VSPRINTF(pszBuf, iBufLen, pszFormat, valist) \
    ({ \
    int iSafeRet = vsnprintf(pszBuf, iBufLen, pszFormat, valist); \
    (pszBuf)[iBufLen - 1] = '\0'; \
    (unsigned int)iSafeRet < (unsigned int)iBufLen? iSafeRet : iBufLen - 1; \
})

namespace ServerLib
{

class CStringUtility
{
public:
    CStringUtility() {}
    ~CStringUtility() {}

public:
    /**
    *ɾ���ַ����ײ���β���Ŀհ��ַ�
    *@param[in] szTrimString ��Ҫɾ���հ׵��ַ���
    *@return 0 success
    */
    static int TrimString(char* szTrimString);

    /**
    *�жϴ�����ַ��Ƿ��ǿհ��ַ�
    *@param[in] c �����ַ�
    *@return true or false
    */
    static bool IsBlankChar(char c);

    /**
    *ת���ַ�������
    *@param[in] szInBuffer ��Ҫת�����ַ���
    *@param[in] iInLength ��Ҫת�����ַ�����ʵ�ʳ���
    *@param[out] szOutBuffer ת������ַ�������
    *@param[in/out] iOutLength szOutBuffer����󳤶ȣ�outΪת����ĳ���
    *@param[in] szToCode ת����ĸ�ʽ
    *@param[in] szFromCode ת��ǰ�ĸ�ʽ
    */
    static int ConvertCode(char* szInBuffer, size_t uiInLength, char* szOutBuffer, size_t  uiOutLength, const char* szToCode, const char* szFromCode);

};

}


#endif //__STRING_UTIL_HPP__
///:~
