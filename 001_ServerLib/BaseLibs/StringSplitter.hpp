/**
*@file StringSplitter.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief �ַ���������
*
*	��һ���ַ�������һ���ָ�����separtor���ָ�������������ȡ�ָ������Ĵ�
*	�� "derek yu"����ַ������������÷ָ���" "��Ϊderek��yu
*	���� "hello&every one����ַ��������Ը��ݷָ���"&"��Ϊhello��every one
*	ʹ�÷�����
*	��1������SetString()����Ҫ�ָ����ַ�������Splitter��ע�����ﴫ��ָ�룬��Splitter�ڲ�ʹ��ʱҲ��ֱ��ʹ�ø�ָ��
*	��2������GetNextToken()ָ���ָ�������ȡ��һ���ָ�����������ǰѰ�ҵ�ƫ��λ������ƣ��Ҳ���ʱ����-1
*/

#ifndef __STRING_SPLITTER_HPP__
#define __STRING_SPLITTER_HPP__

#include "ErrorDef.hpp"

namespace ServerLib
{
class CStringSplitter
{
public:
    CStringSplitter();
    ~CStringSplitter();

public:
    //!ָ��Ҫ���ָ����������ַ��������������ָ�뿽��
    int SetString(const char* pszString);

    //!����Tokenָ�뿪ʼλ��
    void ResetTokenBegPos()
    {
        m_iCurTokenBeg = 0;
    }

    /**
    *��ȡ���ָ����ָ����ַ�����������ǰѰ�ҵ�ƫ��λ�������
    *@param[in] pszSepartor �ָ���
    *@param[out] szToken ���ڴ���ҵ���Token��
    *@param[in] iMaxTokenLength szToken������ܴ�ŵĴ�С
    *@return 0 success, ��0��ʾ�Ҳ���
    */
    int GetNextToken(const char* pszSepartor, char* szToken, int iMaxTokenLength);

    //!�ڽӿڷ��ش���ʱ���������������ȡ�����
    int GetErrorNO() const
    {
        return m_iErrorNO;
    }

private:
    //!���ô����
    void SetErrorNO(int iErrorNO)
    {
        m_iErrorNO = iErrorNO;
    }

private:
    const char* m_pszString;
    int m_iCurTokenBeg; //��ǰTokenָ�뿪ʼλ��
    int m_iErrorNO; //!������

};
}

#endif //__STRING_SPLITTER_HPP__
///:~
