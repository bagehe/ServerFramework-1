
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ConfigManager.hpp"
#include "RepThings/RepThingsUtility.hpp"
#include "Resource/ResourceUtility.h"

#include "RewardUtility.h"

using namespace ServerLib;

//��ȡ����
int CRewardUtility::GetReward(CGameRoleObj& stRoleObj, int iMultiNum, const RewardConfig* pstConfig, int iRewardNum, int iItemChannel)
{
	if (!pstConfig || iRewardNum <= 0 || iMultiNum<=0)
	{
		return T_ZONE_PARA_ERROR;
	}

	for (int i = 0; i < iRewardNum; ++i)
	{
		if (pstConfig[i].iType == 0)
		{
			break;
		}

		//���Ӷһ����
		switch (pstConfig[i].iType)
		{
		case REWARD_TYPE_RES:
		{
			//�һ������Դ
			if (!CResourceUtility::AddUserRes(stRoleObj, pstConfig[i].iRewardID, pstConfig[i].iRewardNum*iMultiNum))
			{
				LOGERROR("Failed to add reward resource, uin %u, reward id %d, num %d\n", stRoleObj.GetUin(), pstConfig[i].iRewardID, pstConfig[i].iRewardNum*iMultiNum);
				return T_ZONE_PARA_ERROR;
			}
		}
		break;

		case REWARD_TYPE_ITEM:
		{
			//�һ���õ���
			int iRet = CRepThingsUtility::AddItemNum(stRoleObj, pstConfig[i].iRewardID, pstConfig[i].iRewardNum*iMultiNum, iItemChannel);
			if (iRet)
			{
				LOGERROR("Failed to add reward item, uin %u, reward id %d, num %d\n", stRoleObj.GetUin(), pstConfig[i].iRewardID, pstConfig[i].iRewardNum*iMultiNum);
				return iRet;
			}
		}
		break;

		case REWARD_TYPE_ENTITY:
		{
			//�һ����ʵ��
			;
		}
		break;

		default:
			break;
		}
	}

	return T_SERVER_SUCCESS;
};