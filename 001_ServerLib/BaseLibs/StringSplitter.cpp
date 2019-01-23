/**
*@file StringSplitter.cpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief �ַ���������
*
*
*/

#include "StringSplitter.hpp"
#include "StringUtility.hpp"
using namespace ServerLib;

CStringSplitter::CStringSplitter()
{
    m_pszString = NULL;
    m_iCurTokenBeg = 0;
    m_iErrorNO = 0;
}

CStringSplitter::~CStringSplitter()
{
    m_pszString = NULL;
    m_iCurTokenBeg = 0;
    m_iErrorNO = 0;
}

int CStringSplitter::SetString(const char* pszString)
{
    if(pszString == NULL)
    {
        SetErrorNO(EEN_STRING_SPLITTER__NULL_POINTER);

        return -1;
    }

    m_pszString = pszString;

    ResetTokenBegPos();

    return 0;
}

int CStringSplitter::GetNextToken(const char* pszSepartor, char* szToken, int iMaxTokenLength)
{
    if(pszSepartor == NULL || szToken == NULL || iMaxTokenLength <= 0)
    {
        SetErrorNO(EEN_STRING_SPLITTER__NULL_POINTER);

        return -1;
    }

    if(m_iCurTokenBeg < 0)
    {
        szToken[0] = '\0';
        SetErrorNO(EEN_STRING_SPLITTER__BAD_TOKEN_BEG);

        return -2;
    }

    if(pszSepartor[0] == '\0')
    {
        szToken[0] = '\0';
        SetErrorNO(EEN_STRING_SPLITTER__BAD_SEPARTOR);

        return -3;
    }

    const char* pszTokenBeg = m_pszString + m_iCurTokenBeg;

    //�Ѿ������ַ���ĩβ��
    if(pszTokenBeg[0] == '\0')
    {
        szToken[0] = '\0';
        SetErrorNO(EEN_STRING_SPLITTER__REACH_END);

        return -4;
    }

    const char* pszFindBeg = strstr(pszTokenBeg, pszSepartor);

    //û���ҵ�
    if(pszFindBeg == NULL)
    {
        SAFE_STRCPY(szToken, pszTokenBeg, iMaxTokenLength-1);
        szToken[iMaxTokenLength-1] = '\0';
        m_iCurTokenBeg = strlen(m_pszString);
    }
    else
    {
        int iTokenLen = pszFindBeg - pszTokenBeg;

        if(iTokenLen < 0)
        {
            SetErrorNO(EEN_STRING_SPLITTER__STRSTR_FAILED);

            return -5;
        }

        if(iTokenLen > iMaxTokenLength - 1)
        {
            iTokenLen = iMaxTokenLength - 1;
        }

        if(iTokenLen > 0)
        {
            //��ȡToken��
            memcpy(szToken, pszTokenBeg, iTokenLen);
        }

        szToken[iTokenLen] = '\0';

        //���¶�λToken��ʼָ��
        m_iCurTokenBeg = pszFindBeg + 1 - m_pszString;
    }

    return 0;
}
