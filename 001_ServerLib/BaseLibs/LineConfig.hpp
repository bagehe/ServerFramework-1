/**
*@file LineConfig.hpp
*@author jasonxiong
*@date 2009-11-18
*@version 1.0
*@brief ���������ļ���
*
*	���ж�ȡ�������ļ���ÿ����һ�������ɸ����������������֮���ÿո��TAB���뿪
*/

#ifndef __LINE_CONFIG_HPP__
#define __LINE_CONFIG_HPP__

#include <stdio.h>

#include "ErrorDef.hpp"

namespace ServerLib
{

class CLineConfig
{
public:
    CLineConfig();
    ~CLineConfig();

public:
    //!���������ļ����ڴ�
    int  OpenFile(const char *pszFilename);

    //!�ͷ������ļ����ص��ڴ��ռ�õ���Դ
    void CloseFile();

    //!��ȡһ�����ã����ض�ȡ�����������
    int GetItemValue(const char* pcContent, ...);

    //!�ж��Ƿ��Ѿ���ȡ����
    bool IsReachEOF() const
    {
        return m_bReachEOF;
    }

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
    int m_iErrorNO; //!������
    FILE* m_pfCfgFile; //!<�򿪵������ļ�ָ��
    bool m_bReachEOF; //!<�ļ���ȡ��ĩβ
};

}

#endif //__LINE_CONFIG_HPP__
///:~
