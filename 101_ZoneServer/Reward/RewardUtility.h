#pragma once

#include "Kernel/GameRole.hpp"

//����������
class RewardConfig;
class CRewardUtility
{
public:
	
	//��ȡ����
	static int GetReward(CGameRoleObj& stRoleObj, int iMultiNum, const RewardConfig* pstConfig, int iRewardNum, int iItemChannel);
};