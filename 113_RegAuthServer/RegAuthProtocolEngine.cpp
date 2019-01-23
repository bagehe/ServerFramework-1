#include <assert.h>

#include "GameProtocol.hpp"
#include "LogAdapter.hpp"

#include "RegAuthProtocolEngine.hpp"

using namespace ServerLib;

int CRegAuthProtocolEngine::Decode(const unsigned char* pszCodeBuff, int iCodeLen, GameProtocolMsg& stMsg, 
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
	if (!pszCodeBuff)
	{
		return -1;
	}

	int iRealCodeOffset = 0;
	if (enMsgPeer == GAME_SERVER_LOTUS)
	{
		//���صİ�
		if (!pNetHead)
		{
			return -2;
		}

		//���NetHead
		memcpy((void*)pNetHead, pszCodeBuff, NETHEAD_V2_SIZE);

		// �հ����ͻ������ӶϿ�
		if (NETHEAD_V2_SIZE == iCodeLen)
		{
			stMsg.mutable_sthead()->set_uimsgid(MSGID_LOGOUTSERVER_REQUEST);
			return 0;
		}
		
		iRealCodeOffset = NETHEAD_V2_SIZE + sizeof(unsigned short);
	}
	else
	{
		//������������
		iRealCodeOffset = sizeof(unsigned short);
	}

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

int CRegAuthProtocolEngine::Encode(const GameProtocolMsg& stMsg, unsigned char* pszCodeBuff, int iBuffLen, int& iCodeLen, 
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
	if (!pszCodeBuff)
	{
		return -1;
	}

	unsigned char* pszRealBuff = pszCodeBuff;
	if (enMsgPeer == GAME_SERVER_LOTUS)
	{
		//�������ط�����, NETHEAD_V2_SIZE(NetHead) + 2bytes(len) + msg
		if (!pNetHead)
		{
			return -2;
		}

		// ����NetHead
		memcpy(pszCodeBuff, pNetHead, NETHEAD_V2_SIZE);

		//��������Ϣ
		pszRealBuff = pszCodeBuff + NETHEAD_V2_SIZE + sizeof(unsigned short);

		//��䳤��
		iCodeLen = stMsg.ByteSize() + sizeof(unsigned short);
		pszCodeBuff[NETHEAD_V2_SIZE] = iCodeLen / 256;
		pszCodeBuff[NETHEAD_V2_SIZE + 1] = iCodeLen % 256;

		//��������ͷ����
		iCodeLen += NETHEAD_V2_SIZE;
	}
	else if (enMsgPeer == GAME_SERVER_REGAUTHDB)
	{
		//����RegAuthDB�İ� 2bytes(len) + 4bytes(uin) + msg

		//��������Ϣ
		pszRealBuff = pszCodeBuff + sizeof(unsigned short) + sizeof(unsigned int);

		//��䳤��
		iCodeLen = stMsg.ByteSize() + sizeof(unsigned int) + sizeof(unsigned short);
		pszCodeBuff[0] = iCodeLen / 256;
		pszCodeBuff[1] = iCodeLen % 256;

		//���uin
		*((unsigned int*)(pszCodeBuff + sizeof(unsigned short))) = stMsg.sthead().uin();
	}
	else
	{
		//������������İ� 2bytes(len) + msg

		//��������Ϣ
		pszRealBuff = pszCodeBuff + sizeof(unsigned short);

		//��䳤��
		iCodeLen = stMsg.ByteSize() + sizeof(unsigned short);
		pszCodeBuff[0] = iCodeLen / 256;
		pszCodeBuff[1] = iCodeLen % 256;
	}

	bool bRet = stMsg.SerializeToArray((char*)pszRealBuff, iBuffLen);
	if (!bRet)
	{
		return -3;
	}

	return 0;
}
