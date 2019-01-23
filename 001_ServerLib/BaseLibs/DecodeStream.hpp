/**
*@file DecodeStream.hpp
*@author jasonxiong
*@date 2009-12-01
*@version 1.0
*@brief ������
*
*	��һ��Buffer���������ֽ�����������
*/

#ifndef __DECODE_STREAM_HPP__
#define __DECODE_STREAM_HPP__

#include <stdint.h>

namespace ServerLib
{

class CDecodeStream
{
public:
    CDecodeStream();
    ~CDecodeStream();

public:
public:
    /**
    *��ʼ��������
    *@param[in] pucCodeBuf ��ű��������������
    *@param[in] iCodeLen ���볤��
    *@return 0 success
    */
    int Initialize(unsigned char* pucCodeBuf, int iCodeLen);

    //!��ȡ����������
    unsigned char* GetCodeBuf() const
    {
        return m_aucCodeBuf;
    }

    //!��ȡ��ǰ����ƫ��
    int GetCurOffset() const
    {
        return m_pCurCodePos - m_aucCodeBuf;
    }

    //!���õ�ǰ����ƫ��
    void SetCurOffset(int iOffset)
    {
        m_pCurCodePos = m_aucCodeBuf + iOffset;
    }

    //!���ر���ʣ�೤��
    int GetLeftLength() const
    {
        return m_iCodeLen - (int)(m_pCurCodePos - m_aucCodeBuf);
    }

    //����char
    CDecodeStream& operator >>(char& rcChar);
    CDecodeStream& operator >>(unsigned char& rucChar);

    //����short16
    CDecodeStream& operator >>(short& rshShort16);
    CDecodeStream& operator >>(unsigned short& rushShort16);

    //����Int32
    CDecodeStream& operator >>(int& riInt32);
    CDecodeStream& operator >>(unsigned int& ruiInt32);

    //����Int64
    CDecodeStream& operator >>(uint64_t& ruiInt64);

    ////����long
    //CDecodeStream& operator >>(long& rlLong);
    //CDecodeStream& operator >>(unsigned long& rulLong);

    //����String
    int DecodeString(char *strDest, short shMaxStrLength);

    //����Mem
    int DecodeMem(char *pcDest, int iMemorySize);

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
    int m_iCodeLen; //!<���볤��
    unsigned char* m_aucCodeBuf; //!<��Ҫ���������������
    unsigned char* m_pCurCodePos; //!<��ǰ����λ��
};


}

#endif //__DECODE_STREAM_HPP__
///:~
