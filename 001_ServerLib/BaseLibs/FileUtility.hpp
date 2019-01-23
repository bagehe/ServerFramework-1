/**
*@file FileUtility.hpp
*@author jasonxiong
*@date 2009-10-29
*@version 1.0
*@brief �ļ���ز�������������
*
*
*/

#ifndef __FILE_UTIL_HPP__
#define __FILE_UTIL_HPP__

namespace ServerLib
{

class CFileUtility
{
private:
    CFileUtility() {}
    ~CFileUtility() {}

public:
    /**
    *����һ���ļ���
    *@param[in] pszDirName ��Ҫ�������ļ�����
    *@return 0 success
    */
    static int MakeDir(const char* pszDirName);

    /**
    *��������Ŀ¼
    *@param[in] pszDirName ��Ҫ�ı䵽�Ĺ���Ŀ¼
    *@return 0 success
    */
    static int ChangeDir(const char* pszDirName);
};

}


#endif //__FILE_UTIL_HPP__
///:~
