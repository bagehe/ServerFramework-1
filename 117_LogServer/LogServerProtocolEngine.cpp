
#include "GameProtocol.hpp"
#include "ThreadLogManager.hpp"
#include "StringUtility.hpp"

#include "LogServerProtocolEngine.hpp"

//�߳���Ϣ����
int CLogServerProtocolEngine::Decode(const unsigned char* pszCodeBuff, int iCodeLen, GameProtocolMsg& stMsg,
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
    // ��ʹ��NetHead
    if (!pszCodeBuff)
	{
		return -1;
	}

    // ��������
	int iBuffLen = iCodeLen - sizeof(unsigned short);
	char* pszBuff = (char*)pszCodeBuff + sizeof(unsigned short);

    DEBUG_THREAD(iThreadIndex, "Decode Buff len: %d\n", iBuffLen);

	//����
	bool bRet = stMsg.ParseFromArray(pszBuff, iBuffLen);
	if(!bRet)
	{
		TRACE_THREAD(iThreadIndex, "Fail to parse proto from buff!\n");
		return -2;
	}

	DEBUG_THREAD(iThreadIndex, "Success to parse proto data!\n");

    return 0;
}

//�߳���Ϣ����
int CLogServerProtocolEngine::Encode(const GameProtocolMsg& stMsg, unsigned char* pszCodeBuff, int iBuffLen, int& iCodeLen,
	int iThreadIndex, const TNetHead_V2* pNetHead, EGameServerID enMsgPeer)
{
    // ��ʹ��NetHead
    if (!pszCodeBuff)
	{
		return -3;
	}

	//��protobuf���б���
	int iMsgLen = stMsg.ByteSize();
	if(iMsgLen >= iBuffLen)
	{
		TRACE_THREAD(iThreadIndex, "Fail to parse from data array, real size %d, buff size %d\n", iMsgLen, iBuffLen);
		return -4;
	}

    // ��������
	bool bRet = stMsg.SerializeToArray(pszCodeBuff +sizeof(unsigned short), iBuffLen);
	if(!bRet)
	{
		TRACE_THREAD(iThreadIndex, "Fail to serialize proto to array!");
		return -5;
	}

	iCodeLen = iMsgLen + sizeof(unsigned short);
	pszCodeBuff[0] = iCodeLen / 256;
	pszCodeBuff[1] = iCodeLen % 256;

	DEBUG_THREAD(iThreadIndex, "Success to serialize proto to array!");

    return 0;
}
