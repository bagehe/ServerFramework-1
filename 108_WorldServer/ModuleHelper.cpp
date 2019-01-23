#include "SectionConfig.hpp"
#include "AppDefW.hpp"
#include "WorldMsgHelper.hpp"

#include "ModuleHelper.hpp"

CWorldProtocolEngineW* CModuleHelper::m_pWorldProtocolEngine = 0;
CAppLoopW* CModuleHelper::m_pAppLoop = 0;

int CModuleHelper::m_iWorldID;
CZoneTick* CModuleHelper::m_pZoneTick;

CWorldProtocolEngineW* CModuleHelper::GetWorldProtocolEngine()
{
	return m_pWorldProtocolEngine;
};

void CModuleHelper::RegisterWorldProtocolEngine(CWorldProtocolEngineW* pWorldProtocolEngine)
{
	m_pWorldProtocolEngine = pWorldProtocolEngine;
};

bool CModuleHelper::IsRealGM(unsigned int uiUin)
{
	if (uiUin < MAX_TEST_UIN)
	{
		return false;
	}

	//todo jasonxiong ��ʱ��ȥ���ⲿ���߼���������ͳһ���
	// GM�ʺŲ������а���
	/*
	bool bGM = GetConfigManager()->GetWhiteListConfigManager().IsInGMList(uiUin);
	if (bGM)
	{
		return true;
	}
	*/

	return false;
}

