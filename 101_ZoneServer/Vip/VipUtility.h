#pragma once

#include "Kernel/GameRole.hpp"

//VIP������
class CVipUtility
{
public:
	
	//���¾ȼý���Ϣ
	static void UpdateAlmsInfo(CGameRoleObj& stRoleObj, int iAlmsNum, int iNextAlmsTime);

	//�����ȼý����
	static int TriggerAlmsUpdate(CGameRoleObj& stRoleObj);
};
