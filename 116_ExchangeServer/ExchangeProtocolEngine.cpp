#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"

#include "ExchangeProtocolEngine.hpp"

using namespace ServerLib;

int CExchangeProtocolEngine::Decode(const unsigned char* pszCodeBuff, int iCodeLen, GameProtocolMsg& stMsg,
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
	if (!pszCodeBuff)
	{
		return -1;
	}

	//������������
	int iRealCodeOffset = sizeof(unsigned short);

	// ��������
	int iBuffLen = iCodeLen - iRealCodeOffset;
	char* pszBuff = (char*)pszCodeBuff + iRealCodeOffset;

	// ����
	if (!stMsg.ParseFromArray(pszBuff, iBuffLen))
	{
		return -3;
	}

	return 0;
}

int CExchangeProtocolEngine::Encode(const GameProtocolMsg& stMsg, unsigned char* pszCodeBuff, int iBuffLen, int& iCodeLen,
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
	if (!pszCodeBuff)
	{
		return -1;
	}

	unsigned char* pszRealBuff = pszCodeBuff;

	//������������İ� 2bytes(len) + msg

	//��������Ϣ
	pszRealBuff = pszCodeBuff + sizeof(unsigned short);

	//��䳤��
	iCodeLen = stMsg.ByteSize() + sizeof(unsigned short);
	pszCodeBuff[0] = iCodeLen / 256;
	pszCodeBuff[1] = iCodeLen % 256;

	bool bRet = stMsg.SerializeToArray((char*)pszRealBuff, iBuffLen);
	if (!bRet)
	{
		return -3;
	}

	return 0;
}
