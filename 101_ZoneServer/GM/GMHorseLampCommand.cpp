
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Chat/ChatUtility.h"

#include "GMHorseLampCommand.hpp"
using namespace ServerLib;

CGMHorseLampCommand::CGMHorseLampCommand()
{
	m_pRoleObj = NULL;
}

//ִ����Ӧ��GM����Ĺ���
int CGMHorseLampCommand::Run(unsigned uin, const GameMaster_Request& stReq, GameMaster_Response& stResp)
{
	//ִ����Ӧ��GM����
	switch (stReq.ioperatype())
	{
	case GM_OPERA_HORSELAMP:
	{
		if (stReq.strparams_size() != 1)
		{
			LOGERROR("Failed to run horselamp gm command, invalid param!\n");
			return T_ZONE_PARA_ERROR;
		}

		//��ȡ����
		BaseConfigManager& stBaseCfMgr = CConfigManager::Instance()->GetBaseCfgMgr();

		//�������������
		const HorseLampConfig* pstHorseLampConfig = stBaseCfMgr.GetHorseLampConfig(HORSELAMP_TYPE_GM);
		if (pstHorseLampConfig)
		{
			//���������
			SendGMHorseLamp(pstHorseLampConfig->iID, stReq.strparams(0));
		}
		
	}
	break;

	default: 
	{
		return T_ZONE_PARA_ERROR;
	}
	break;
	}

	return T_SERVER_SUCCESS;
}

void CGMHorseLampCommand::SendGMHorseLamp(int iLampID, const std::string& strHorseLamp)
{
	std::vector<HorseLampData> vDatas;

	HorseLampData stOneData;
	stOneData.iLampID = iLampID;
	stOneData.vParams.push_back(strHorseLamp);

	vDatas.push_back(stOneData);
	CChatUtility::SendHorseLamp(vDatas);

	return;
}
