/**
*@file EncodeStream.hpp
*@author jasonxiong
*@date 2009-11-30
*@version 1.0
*@brief ������
*
*	���������������������ֽ�����뵽һ��Buffer��
*/

#ifndef __ENCODE_STREAM_HPP__
#define __ENCODE_STREAM_HPP__

#include <stdint.h>

namespace ServerLib
{

class CEncodeStream
{
public:
    CEncodeStream();
    ~CEncodeStream();

public:
    /**
    *��ʼ��������
    *@param[in] pucCodeBuf ��ű��������������
    *@param[in] iMaxCodeLen ������������󳤶�
    *@return 0 success
    */
    int Initialize(unsigned char* pucCodeBuf, int iMaxCodeLen);

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

    //����char
    CEncodeStream& operator <<(char cChar);
    CEncodeStream& operator <<(unsigned char ucChar);

    //����short16
    CEncodeStream& operator <<(short shShort16);
    CEncodeStream& operator <<(unsigned short ushShort16);

    //����Int32
    CEncodeStream& operator <<(int iInt32);
    CEncodeStream& operator <<(unsigned int uiInt32);

    //����Int64
    CEncodeStream& operator <<(uint64_t uiInt64);

    ////����long
    //CEncodeStream& operator <<(long lLong);
    //CEncodeStream& operator <<(unsigned long ulLong);

    //����String
    int EncodeString(char *strSrc, short sMaxStrLength);

    //����Mem
    int EncodeMem(char *pcSrc, int iMemorySize);

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
    int m_iMaxCodeLen; //!<������������󳤶�
    unsigned char* m_aucCodeBuf; //!<��ű��������������
    unsigned char* m_pCurCodePos; //!<��ǰ����λ��
};

}

#endif //__ENCODE_STREAM_HPP__
///:~
