#ifndef __PROTO_DATA_UTILITY_HPP__
#define __PROTO_DATA_UTILITY_HPP__

//���ڶ�DB�д洢��protobuf���ݵĴ���

#include <string>

#include "lz4.hpp"
#include "google/protobuf/message.h"

//ѹ��������Ϊ��
//      1.�Ƚ�protobuf�ṹ�������л���
//      2.�����л���Ķ���������ͨ��lz4����ѹ��;

//��ѹ������Ϊ��
//      1.��ͨ��lz4��ѹ����ȡ���������ݣ�
//      2.�����������ݷ����л���protobuf�ṹ��

namespace ServerLib
{

#define LZ4_COMPRESS_BUFF_LEN 10*1024     //ѹ�����ݵĻ�������СΪ10K

extern inline bool CompressData(const std::string& strSource, std::string& strDest)
{
    if(LZ4_compressBound(strSource.size()) > LZ4_COMPRESS_BUFF_LEN)
	{
		return false;
	}

    //ѹ������
	char szCompressedBuff[LZ4_COMPRESS_BUFF_LEN];
    int iCompressedLen = LZ4_compress(strSource.c_str(), szCompressedBuff, strSource.size());
    if(iCompressedLen == 0)
    {
        return false;
    }

    strDest.assign(szCompressedBuff, iCompressedLen);

    return true;
}

extern inline bool UnCompressData(const std::string& strSource, std::string& strDest)
{
    //��ѹ������
	char szUnCompressedBuff[LZ4_COMPRESS_BUFF_LEN];
    int iUnCompressedLen = LZ4_decompress_safe(strSource.c_str(), szUnCompressedBuff, strSource.size(), sizeof(szUnCompressedBuff)-1);
    if(iUnCompressedLen < 0)
    {
        return false;
    }

    strDest.assign(szUnCompressedBuff, iUnCompressedLen);

    return true;
}

//���л�protobuf���ݲ�ѹ��
template <class ProtoType>
bool EncodeProtoData(const ProtoType& stProtoMsg, std::string& strCompressedData)
{		
    //�����л�����
    std::string strSerialized;
    if(!stProtoMsg.SerializeToString(&strSerialized))
    {
        return false;
    }

	return CompressData(strSerialized, strCompressedData);
};

//��ѹ���ݲ������л���protobuf��
template <class ProtoType>
bool DecodeProtoData(const std::string& strCompressData, ProtoType& stProtoMsg)
{
    std::string strUnCompressData;
    if(!UnCompressData(strCompressData, strUnCompressData))
    {
        return false;
    }

    //�����л�protobuf
    stProtoMsg.Clear();
    if(!stProtoMsg.ParseFromString(strUnCompressData))
    {
        return false;
    }

    return true;
};

}

#endif
