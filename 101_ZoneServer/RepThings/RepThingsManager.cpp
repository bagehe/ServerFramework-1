
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ZoneObjectHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/ConfigManager.hpp"
#include "Kernel/GameRole.hpp"
#include "RepThingsUtility.hpp"

#include "RepThingsManager.hpp"

using namespace ServerLib;

//��ұ�����Ʒ�����仯��֪ͨ
GameProtocolMsg CRepThingsManager::m_stRepChangeNotify;

CRepThingsManager::CRepThingsManager()
{
    Initialize();
}

CRepThingsManager::~CRepThingsManager()
{

}

//��ʼ��
int CRepThingsManager::Initialize()
{
    m_uiUin = 0;
	m_vstRepItems.clear();

    return T_SERVER_SUCCESS;
}

void CRepThingsManager::SetOwner(unsigned int uin)
{
    m_uiUin = uin;
}

CGameRoleObj* CRepThingsManager::GetOwner()
{
    return GameTypeK32<CGameRoleObj>::GetByKey(m_uiUin);
}

//����������Ʒ
int CRepThingsManager::AddRepItem(int iItemID, int iItemNum, int iItemChannel)
{
    if(iItemID<=0 || iItemNum<=0)
    {
        return T_ZONE_PARA_ERROR;
    }

	//�Ȳ����Ƿ����
	RepItem* pstItemInfo = GetRepItemInfo(iItemID);
	if (!pstItemInfo)
	{
		//�Ҳ�������һ��
		if (m_vstRepItems.size() >= (unsigned)MAX_REP_BLOCK_NUM)
		{
			//����������������������
			return T_ZONE_PARA_ERROR;
		}

		RepItem stItem;
		stItem.iItemID = iItemID;
		stItem.iItemNum = iItemNum;

		m_vstRepItems.push_back(stItem);
	}
	else
	{
		pstItemInfo->iItemNum += iItemNum;
	}

    //���ͱ����仯��֪ͨ
    SendRepChangeNotify(iItemID, iItemNum, iItemChannel);

    return T_SERVER_SUCCESS;
}

//����ɾ���̶���������Ʒ
int CRepThingsManager::DeleteRepItem(int iItemID, int iItemNum, int iItemChannel)
{
	if (iItemID <= 0 || iItemNum <= 0)
	{
		return T_ZONE_PARA_ERROR;
	}

    //���ж������Ƿ��㹻
	RepItem* pstItemInfo = GetRepItemInfo(iItemID);
	if (!pstItemInfo || pstItemInfo->iItemNum < iItemNum)
	{
		return T_ZONE_PARA_ERROR;
	}

    //��ɾ��ʵ��ɾ����Ʒ
	pstItemInfo->iItemNum -= iItemNum;
	if (pstItemInfo->iItemNum <= 0)
	{
		//�ӱ�������ɾ��
		for (unsigned i = 0; i < m_vstRepItems.size(); ++i)
		{
			if (m_vstRepItems[i].iItemID == iItemID)
			{
				m_vstRepItems.erase(m_vstRepItems.begin()+i);
				break;
			}
		}
	}

	//������Ʒ֪ͨ
	SendRepChangeNotify(iItemID, -iItemNum, iItemChannel);

    return T_SERVER_SUCCESS;
}

//��ȡ��������Ʒ������
int CRepThingsManager::GetRepItemNum(int iItemID)
{
	RepItem* pstItemInfo = GetRepItemInfo(iItemID);
	if (!pstItemInfo)
	{
		return 0;
	}

	return pstItemInfo->iItemNum;
}

//������Ʒ���ݿ��������
void CRepThingsManager::UpdateRepItemToDB(ITEMDBINFO& rstItemInfo)
{
    ItemSlotInfo* pstItemInfo = rstItemInfo.mutable_stitemslot();
    for(unsigned i=0; i<m_vstRepItems.size(); ++i)
    {
        OneSlotInfo* pstOneItemInfo = pstItemInfo->add_stslots();
		pstOneItemInfo->set_iitemid(m_vstRepItems[i].iItemID);
		pstOneItemInfo->set_iitemnum(m_vstRepItems[i].iItemNum);
    }

    return;
}

void CRepThingsManager::InitRepItemFromDB(const ITEMDBINFO& stItemInfo)
{
	m_vstRepItems.clear();
	
    const ItemSlotInfo& stSlotInfo = stItemInfo.stitemslot();
	RepItem stItem;
    for(int i=0; i<stSlotInfo.stslots_size(); ++i)
    {
		stItem.iItemID = stSlotInfo.stslots(i).iitemid();
		stItem.iItemNum = stSlotInfo.stslots(i).iitemnum();

		m_vstRepItems.push_back(stItem);
    }

    return;
}

RepItem* CRepThingsManager::GetRepItemInfo(int iItemID)
{
	for (unsigned i = 0; i < m_vstRepItems.size(); ++i)
	{
		if (m_vstRepItems[i].iItemID == iItemID)
		{
			return &m_vstRepItems[i];
		}
	}

	return NULL;
}

//���ͱ�����Ʒ�仯��֪ͨ
void CRepThingsManager::SendRepChangeNotify(int iItemID, int iItemNum, int iItemChannel)
{
	static GameProtocolMsg stMsg;

    CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REPCHANGE_NOTIFY);

    Zone_RepChange_Notify* pstNotify = stMsg.mutable_stbody()->mutable_m_stzone_repchange_notify();
	
	OneRepSlot* pstRepSlot = pstNotify->add_stitems();
	pstRepSlot->set_ichangechannel(iItemChannel);

	//��Ʒ��ϸ��Ϣ
	OneSlotInfo* pstSlotInfo = pstRepSlot->mutable_stslotinfo();
	pstSlotInfo->set_iitemid(iItemID);
	pstSlotInfo->set_iitemnum(iItemNum);

    //������Ϣ���ͻ���
    CZoneMsgHelper::SendZoneMsgToRole(stMsg, GetOwner());

    return;
}
