#pragma once

#include <time.h>

#include "GameProtocol.hpp"

class CWorldOnlineStatTick
{
public:
    int Initialize();
    int OnTick();

private:
    int SendWorldOnlineStat();

private:
    time_t m_stLastTickTime; 
    GameProtocolMsg m_stMsg;   
};
