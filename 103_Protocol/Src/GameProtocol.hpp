#ifndef __GAME_PROTOCOL_HPP__
#define __GAME_PROTOCOL_HPP__

//��Э���protobuf���з�װ������ҵ�����

#include "GameProtocol_CS.pb.h"

typedef GameProtocol::CS::ProtocolCSMsg GameProtocolMsg;
typedef GameProtocol::CS::CSMsgHead     GameCSMsgHead;
typedef GameProtocol::CS::CSMsgBody     GameCSMsgBody;

#endif
