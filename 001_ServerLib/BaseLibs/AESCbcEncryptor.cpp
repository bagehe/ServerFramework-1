#include "LogAdapter.hpp"

#include "AESCbcEncryptor.hpp"

using namespace ServerLib;

CAESCbcEncryptor::CAESCbcEncryptor()
{
	m_pstEncryptor = NULL;
	m_pstDecryptor = NULL;
}

CAESCbcEncryptor::~CAESCbcEncryptor()
{
	if (m_pstEncryptor)
	{
		delete m_pstEncryptor;
		m_pstEncryptor = NULL;
	}

	if (m_pstDecryptor)
	{
		delete m_pstDecryptor;
		m_pstDecryptor = NULL;
	}
}

int CAESCbcEncryptor::SetAESKey(const char* pszAESKey, int iAESKeyLen)
{
	if (!pszAESKey)
	{
		TRACESVR("Failed to set AES key, invalid key param, key len %u\n", iAESKeyLen);
		return -1;
	}

	HashFunction* hash = get_hash("MD5"); 				//MD5�㷨���Խ����ⳤ�ȵ��ֽڴ�ת��Ϊ128λ���ȵ��ֽڴ�  
	SymmetricKey key = hash->process(std::string(pszAESKey, iAESKeyLen)); 		//����128λ���ֽڴ���Ϊ��Կ  
	SecureVector<byte> raw_iv = hash->process('0' + std::string(pszAESKey, iAESKeyLen)); //�ַ������,Ȼ��Խ����MD5,����128λ���ֽڴ�  
	InitializationVector iv(raw_iv, 16); //��ʼ������  

										 //AES-128���㷨����,�ֿ��С��128bit; CBC���㷨ģʽ.  
										 //AES�㷨��Կ�ͷֿ��С������128,192,256λ.  
										 //AES�㷨ģʽ������: ECB,CFB,OFB,CTR��.  
	m_pstEncryptor = new Pipe(get_cipher("AES-128/CBC", key, iv, ENCRYPTION));

	m_pstDecryptor = new Pipe(get_cipher("AES-128/CBC", key, iv, DECRYPTION));

	return 0;
}

std::string CAESCbcEncryptor::DoAESCbcEncryption(const std::string& strInput)
{
	if (!m_pstEncryptor || strInput.size() == 0)
	{
		return "";
	}

	try
	{
		m_pstEncryptor->process_msg(strInput); //encryption
	}
	catch (Botan::Decoding_Error &e)
	{
		//����ʧ�ܣ��׳�һ��Botan::Decoding_Error���͵��쳣  
		TRACESVR("Failed to do botan encryption, e: %s\n", e.what());

		return "";
	}

	return m_pstEncryptor->read_all_as_string();
}

std::string CAESCbcEncryptor::DoAESCbcDecryption(const std::string& strInput)
{
	if (!m_pstDecryptor || strInput.size() == 0)
	{
		return "";
	}

	try
	{
		m_pstDecryptor->process_msg(strInput); //encryption
	}
	catch (Botan::Decoding_Error &e)
	{
		//����ʧ�ܣ��׳�һ��Botan::Decoding_Error���͵��쳣  
		TRACESVR("Failed to do botan decryption, e: %s\n", e.what());

		return "";
	}

	return m_pstDecryptor->read_all_as_string();
}
