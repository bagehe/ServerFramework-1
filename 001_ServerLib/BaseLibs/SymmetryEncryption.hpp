/**
*@file SymmetryEncryption.hpp
*@author jasonxiong
*@date 2009-12-04
*@version 1.0
*@brief �ԳƼ���
*
*
*/

#ifndef __SYMMETRY_ENCRYPTION_HPP__
#define __SYMMETRY_ENCRYPTION_HPP__

namespace ServerLib
{

typedef enum enmSymmetryEncryptionAlgorithm
{
    ESEA_NO_ENCRYPT = 0, //!<ֱ�ӿ�����������
    ESEA_OI_SYMMETRY_ENCRYPT = 1, //!<ʹ��OI��ĶԳƼ���
} ENMSYMMETRYENCRYPTIONALGORITHM;

class CSymmetryEncryption
{
public:
    CSymmetryEncryption() {}
    ~CSymmetryEncryption() {}

public:
    /**
    *�ԳƼ��ܵļ��ܺ���
    *@param[in] nAlgorithm �����㷨
    *@param[in] pbyKey �Գ���Կ
    *@param[in] pbyIn ����
    *@param[in] nInLength ���ĳ���
    *@param[in] pnOutLength ������󳤶�
    *@param[out] pbyOut ����
    *@param[out] pnOutLength ����ʵ�ʳ���
    *@return 0 success
    */
    static int EncryptData(short nAlgorithm, const unsigned char *pbyKey,
                           const unsigned char *pbyIn, short nInLength,
                           unsigned char *pbyOut, short *pnOutLength);

    /**
    *�ԳƼ��ܵĽ��ܺ���
    *@param[in] nAlgorithm �����㷨
    *@param[in] pbyKey �Գ���Կ
    *@param[in] pbyIn ����
    *@param[in] nInLength ���ĳ���
    *@param[in] pnOutLength ������󳤶�
    *@param[out] pbyOut ����
    *@param[out] pnOutLength ����ʵ�ʳ���
    *@return 0 success
    */
    static int DecryptData(short nAlgorithm, const unsigned char *pbyKey,
                           const unsigned char *pbyIn, short nInLength,
                           unsigned char *pbyOut, short *pnOutLength);

};
}

#endif //__SYMMETRY_ENCRYPTION_HPP__
///:~
