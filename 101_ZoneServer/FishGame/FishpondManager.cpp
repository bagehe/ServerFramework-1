
#include "GameProtocol.hpp"
#include "Kernel/GameRole.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "FishUtility.h"

#include "FishpondManager.h"

CFishpondManager* CFishpondManager::Instance()
{
	static CFishpondManager* pInstance = NULL;
	if (!pInstance)
	{
		pInstance = new CFishpondManager;
	}

	return pInstance;
}

CFishpondManager::CFishpondManager()
{
	m_mpFreeHead.clear();
}

CFishpondManager::~CFishpondManager()
{

}

//�������
CFishpondObj* CFishpondManager::CreateFishpond(int iFishRoomID)
{
	//��ȡ����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();
	const FishRoomConfig* pstRoomConfig = stBaseCfgMgr.GetFishRoomConfig(iFishRoomID);
	if (!pstRoomConfig)
	{
		LOGERROR("Failed to get fish room config, room id %d\n", iFishRoomID);
		return NULL;
	}

	//�ȴӿ��еĻ�ȡ
	CFishpondObj* pstFishpondObj = NewFromFishpondList(*pstRoomConfig);
	if (pstFishpondObj)
	{
		//�п��е�
		return pstFishpondObj;
	}

	//û�п��еģ�����һ��
	unsigned uTableUniqID = CFishUtility::GetFishUniqID();
	pstFishpondObj = GameTypeK32<CFishpondObj>::CreateByKey(uTableUniqID);
	if (!pstFishpondObj)
	{
		LOGERROR("Failed to create fishpond obj!\n");
		return NULL;
	}

	//����������Ϣ
	pstFishpondObj->SetTableInfo(uTableUniqID, iFishRoomID, *pstRoomConfig);

	//��ӵ���������
	AddToFishpondList(*pstRoomConfig, *pstFishpondObj);

	return pstFishpondObj;
}

//�������
void CFishpondManager::DelFishpond(CFishpondObj& stFishpondObj)
{
	const FishRoomConfig* pstConfig = stFishpondObj.GetRoomConfig();
	if (!pstConfig)
	{
		LOGERROR("Failed to del fishpond, invalid fish config!\n");
		return;
	}

	if (stFishpondObj.GetPlayerNum() == 0)
	{
		//����Ѿ�û�����ˣ���������
		if (stFishpondObj.m_pNext)
		{
			stFishpondObj.m_pNext->m_pPrev = stFishpondObj.m_pPrev;
		}

		if (stFishpondObj.m_pPrev)
		{
			stFishpondObj.m_pPrev->m_pNext = stFishpondObj.m_pNext;
		}

		if (m_mpFreeHead[pstConfig->iRoomID] == &stFishpondObj)
		{
			m_mpFreeHead[pstConfig->iRoomID] = stFishpondObj.m_pNext;
		}

		stFishpondObj.m_pPrev = NULL;
		stFishpondObj.m_pNext = NULL;

		unsigned uTableID = stFishpondObj.GetTableID();

		//����Fishpond
		stFishpondObj.ResetFishpond();

		//ɾ��Fishpond
		GameTypeK32<CFishpondObj>::DeleteByKey(uTableID);
	}
	else if ((stFishpondObj.GetPlayerNum() + 1) == pstConfig->iPlayerNum)
	{
		//��������Ϊ�˲���

		//���ӵ���������
		stFishpondObj.m_pNext = m_mpFreeHead[pstConfig->iRoomID];
		stFishpondObj.m_pPrev = NULL;
		if (m_mpFreeHead[pstConfig->iRoomID])
		{
			m_mpFreeHead[pstConfig->iRoomID]->m_pPrev = &stFishpondObj;
		}

		m_mpFreeHead[pstConfig->iRoomID] = &stFishpondObj;
	}

	return;
}

//���¼����������
void CFishpondManager::ReloadFishConfig()
{
	int iFishpondIndex = GameTypeK32<CFishpondObj>::GetUsedHead();
	while (iFishpondIndex >= 0)
	{
		CFishpondObj* pstFishpondObj = GameTypeK32<CFishpondObj>::GetByIdx(iFishpondIndex);
		if (pstFishpondObj)
		{
			unsigned uTableUniqID = pstFishpondObj->GetTableID();
			int iFishRoomID = pstFishpondObj->GetFishRoomID();

			const FishRoomConfig* pstRoomConfig = CConfigManager::Instance()->GetBaseCfgMgr().GetFishRoomConfig(iFishRoomID);
			if (pstRoomConfig)
			{
				pstFishpondObj->SetTableInfo(uTableUniqID, iFishRoomID, *pstRoomConfig);
			}
			else
			{
				LOGERROR("Failed to reload fish room config, invalid room id %d\n", iFishRoomID);
			}
		}

		iFishpondIndex = GameTypeK32<CFishpondObj>::GetNextIdx(iFishpondIndex);
	}

	return;
}

//��ȡ�������
CFishpondObj* CFishpondManager::NewFromFishpondList(const FishRoomConfig& stConfig)
{
	if (m_mpFreeHead.find(stConfig.iRoomID) == m_mpFreeHead.end() || m_mpFreeHead[stConfig.iRoomID]==NULL)
	{
		//���������в�����
		return NULL;
	}

	CFishpondObj* pstTempObj = m_mpFreeHead[stConfig.iRoomID];
	if ((pstTempObj->GetPlayerNum() + 1) < stConfig.iPlayerNum)
	{
		//��ػ�δ����
		return pstTempObj;
	}

	//���������,�ӿ�������ȡ��
	m_mpFreeHead[stConfig.iRoomID] = pstTempObj->m_pNext;
	pstTempObj->m_pNext = NULL;
	pstTempObj->m_pPrev = NULL;

	return pstTempObj;
}

//������ص���������
void CFishpondManager::AddToFishpondList(const FishRoomConfig& stConfig, CFishpondObj& stFishpondObj)
{
	if ((stFishpondObj.GetPlayerNum() + 1) >= stConfig.iPlayerNum)
	{
		//������
		return;
	}

	if (m_mpFreeHead.find(stConfig.iRoomID) != m_mpFreeHead.end())
	{
		//�Ѿ�����
		if (m_mpFreeHead[stConfig.iRoomID] != NULL)
		{
			stFishpondObj.m_pNext = m_mpFreeHead[stConfig.iRoomID];
			stFishpondObj.m_pPrev = NULL;

			m_mpFreeHead[stConfig.iRoomID]->m_pPrev = &stFishpondObj;

			m_mpFreeHead[stConfig.iRoomID] = &stFishpondObj;
		}
		else
		{
			stFishpondObj.m_pNext = NULL;
			stFishpondObj.m_pPrev = NULL;
			m_mpFreeHead[stConfig.iRoomID] = &stFishpondObj;
		}
	}
	else
	{
		stFishpondObj.m_pNext = NULL;
		stFishpondObj.m_pPrev = NULL;
		m_mpFreeHead[stConfig.iRoomID] = &stFishpondObj;
	}

	return;
}
