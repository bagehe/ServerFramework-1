
#include "GameProtocol.hpp"
#include "LogAdapter.hpp"
#include "ErrorNumDef.hpp"
#include "Kernel/ModuleHelper.hpp"
#include "Kernel/ZoneMsgHelper.hpp"
#include "Kernel/UnitUtility.hpp"
#include "Kernel/ZoneOssLog.hpp"
#include "Reward/RewardUtility.h"
#include "Mail/MailUtility.h"
#include "Recharge/RechargeUtility.h"

#include "RepThingsUtility.hpp"
#include "RepThingsHandler.hpp"

using namespace ServerLib;

static GameProtocolMsg stMsg;

CRepThingsHandler::~CRepThingsHandler()
{

}

int CRepThingsHandler::OnClientMsg()
{
	switch (m_pRequestMsg->sthead().uimsgid())
	{
	case MSGID_ZONE_REPOPERA_REQUEST:
	{
		OnRequestOperaRepItem();
	}
	break;

	case MSGID_WORLD_ONLINEEXCHANGE_RESPONSE:
	{
		OnResponseUseRepItem();
	}
	break;

	default:
		break;
	}

	return 0;
}

//����ַ����Ƿ�ȫ����
static bool IsAllNum(const std::string& strChecked)
{
	if (strChecked.size() == 0)
	{
		return false;
	}

	for (unsigned i = 0; i < strChecked.size(); ++i)
	{
		if (strChecked[i]<'0' || strChecked[i]>'9')
		{
			return false;
		}
	}

	return true;
}

//�Ƿ��ƶ��ֻ���
static bool IsCMCCPhone(const std::string& strPhoneNum)
{
	std::string strPhoneType = strPhoneNum.substr(0, 3);
	if (strPhoneType == "134" || strPhoneType == "135" || strPhoneType == "136" || strPhoneType == "137" || 
		strPhoneType == "138" || strPhoneType == "139" || strPhoneType == "147" || strPhoneType == "150" ||
		strPhoneType == "151" || strPhoneType == "152" || strPhoneType == "157" || strPhoneType == "158" ||
		strPhoneType == "159" || strPhoneType == "178" || strPhoneType == "182" || strPhoneType == "183" ||
		strPhoneType == "184" || strPhoneType == "187" || strPhoneType == "188")
	{
		return true;
	}

	return false;
}

//��ұ�����Ʒ�Ĳ���
int CRepThingsHandler::OnRequestOperaRepItem()
{
	int iRet = SecurityCheck();
	if (iRet < 0)
	{
		LOGERROR("Security Check Failed: Uin = %u, iRet = %d\n", m_pRequestMsg->sthead().uin(), iRet);
		return -1;
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REPOPERA_RESPONSE);
	Zone_RepOpera_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_repopera_response();

	//�������ù�����
	BaseConfigManager& stBaseCfgMgr = CConfigManager::Instance()->GetBaseCfgMgr();

	//��ұ�����Ʒ�Ĳ���
	const Zone_RepOpera_Request& stReq = m_pRequestMsg->stbody().m_stzone_repopera_request();
	int iItemID = stReq.iitemid();
	int iItemNum = stReq.iitemnum();

	//���÷���
	pstResp->set_etype(stReq.etype());
	pstResp->set_iitemid(iItemID);
	pstResp->set_iitemnum(iItemNum);
	pstResp->set_utouin(stReq.utouin());

	//�������
	const FishItemConfig* pstItemConfig = stBaseCfgMgr.GetFishItemConfig(iItemID);
	if (!pstItemConfig || iItemNum <= 0)
	{
		LOGERROR("Failed to opera fish item, invalid param, uin %u, item id %d, num %d\n", m_pRoleObj->GetUin(), iItemID, iItemNum);
		
		pstResp->set_iresult(T_ZONE_PARA_ERROR);
		CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
		return -2;
	}

	switch (stReq.etype())
	{
	case REQ_OPERA_TYPE_OPEN:
	{
		iRet = CRepThingsUtility::OpenBoxGift(*m_pRoleObj, iItemID, iItemNum, true);
		if (iRet)
		{
			LOGERROR("Failed to open box item, ret %d, uin %u, box id %d\n", iRet, m_pRoleObj->GetUin(), iItemID);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}
	}
	break;

	case REQ_OPERA_TYPE_USE:
	{
		if (pstItemConfig->iType != FISH_ITEM_USE)
		{
			//�õ��߲���ʹ��
			LOGERROR("Failed to use fish item, uin %u, item id %d\n", m_pRoleObj->GetUin(), iItemID);

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -3;
		}

		//���VIP�ȼ��Ƿ�����
		if (m_pRoleObj->GetVIPLevel() < (stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_USEITEM_VIPLIMIT)+1))
		{
			//VIP�ȼ�������
			LOGERROR("Failed to use fish item, uin %u, vip level real:need %d:%d\n", m_pRoleObj->GetUin(), m_pRoleObj->GetVIPLevel(), (stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_USEITEM_VIPLIMIT) + 1));

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -4;
		}

		//�Ƿ������ƶ��ֻ���
		if (stBaseCfgMgr.GetGlobalConfig(GLOBAL_TYPE_CMCCLIMIT) && IsCMCCPhone(stReq.strphonenum()))
		{
			//�����ƶ��ֻ��ų�ֵ
			LOGERROR("Failed to user fish item, uin %u, invalid phone num %s\n", m_pRoleObj->GetUin(), stReq.strphonenum().c_str());

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -20;
		}

		//����ֻ���
		if (stReq.strphonenum().size() != 11 || !IsAllNum(stReq.strphonenum()))
		{
			//�����ֻ��ŷǷ�
			LOGERROR("Failed to use fish item, uin %u, invalid phone num %s\n", m_pRoleObj->GetUin(), stReq.strphonenum().c_str());

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -4;
		}

		//�۳�����
		iRet = CRepThingsUtility::AddItemNum(*m_pRoleObj, iItemID, -iItemNum, ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			LOGERROR("Failed to delete fish item, ret %d, uin %u, item id:num %d:%d\n", iRet, m_pRoleObj->GetUin(), iItemID, iItemNum);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}

		//����ʹ������
		static GameProtocolMsg stUseMsg;
		CZoneMsgHelper::GenerateMsgHead(stUseMsg, MSGID_WORLD_ONLINEEXCHANGE_REQUEST);
		
		World_OnlineExchange_Request* pstUseReq = stUseMsg.mutable_stbody()->mutable_m_stworld_onlineexchange_request();
		pstUseReq->set_uin(m_pRoleObj->GetUin());
		pstUseReq->set_ifromzoneid(CModuleHelper::GetZoneID());
		pstUseReq->mutable_stinfo()->set_iitemid(iItemID);
		pstUseReq->mutable_stinfo()->set_iitemnum(iItemNum);
		pstUseReq->mutable_stinfo()->set_strphonenum(stReq.strphonenum());
		pstUseReq->mutable_stinfo()->set_itype(pstItemConfig->aiParam[0]);	//����0Ϊʹ������
		pstUseReq->mutable_stinfo()->set_iexchangenum(pstItemConfig->aiParam[1]*iItemNum);	//����1Ϊ��ȡ����

		//���͵�World
		CZoneMsgHelper::SendZoneMsgToWorld(stUseMsg);

		//ֱ�ӷ���
		return T_SERVER_SUCCESS;
	}
	break;

	case REQ_OPERA_TYPE_SENDGIFT:
	{
		//�Ƿ������,�������͸��Լ�
		if (!m_pRoleObj->HasVIPPriv(VIP_PRIV_SENDGIFT) || !pstItemConfig->iCanSend || stReq.utouin() == m_pRoleObj->GetUin())
		{
			//��������
			LOGERROR("Failed to send item gift, item id %d, uin %u, vip level %d\n", iItemID, m_pRoleObj->GetUin(), m_pRoleObj->GetVIPLevel());

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return T_ZONE_PARA_ERROR;
		}

		//�۳�����
		iRet = CRepThingsUtility::AddItemNum(*m_pRoleObj, iItemID, -iItemNum, ITEM_CHANNEL_SENDGIFT);
		if (iRet)
		{
			LOGERROR("Failed to delete gift item, ret %d, uin %u, item id:num %d:%d\n", iRet, m_pRoleObj->GetUin(), iItemID, iItemNum);
			
			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}

		//�����ʼ�
		iRet = CMailUtility::SendGiftMail(*m_pRoleObj, stReq.utouin(), iItemID, iItemNum);
		if (iRet)
		{
			LOGERROR("Failed to send gift mail, ret %d, uin %u, item id:num %d:%d\n", iRet, m_pRoleObj->GetUin(), iItemID, iItemNum);

			//����ʧ�ܣ��������߸����
			CRepThingsUtility::OnSendGiftFailed(m_pRoleObj->GetUin(), stReq.utouin(), iItemID, iItemNum, iRet);

			return iRet;
		}

		CGameRoleObj* pstToRoleObj = CUnitUtility::GetRoleByUin(stReq.utouin());
		if (!pstToRoleObj)
		{
			//Ŀ����Ҳ��ڱ��ߣ��Ȳ����ͷ���
			return T_SERVER_SUCCESS;
		}
	}
	break;

	case REQ_OPERA_TYPE_STYLE:
	{
		if (pstItemConfig->iType != FISH_ITEM_STYLE)
		{
			//���Ƕ�Ӧ����
			LOGERROR("Failed to unlock weapon style item, uin %u, item id %d\n", m_pRoleObj->GetUin(), iItemID);

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -3;
		}

		//�۳�����
		iRet = CRepThingsUtility::AddItemNum(*m_pRoleObj, iItemID, -iItemNum, ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			LOGERROR("Failed to delete fish item, ret %d, uin %u, item id:num %d:%d\n", iRet, m_pRoleObj->GetUin(), iItemID, iItemNum);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}

		//���ӽ�������̨
		m_pRoleObj->UnlockWeaponStyle(pstItemConfig->aiParam[0]);
	}
	break;

	case REQ_OPERA_TYPE_MONTH:
	{
		if (pstItemConfig->iType != FISH_ITEM_MONTH)
		{
			//�õ��߲���ʹ��
			LOGERROR("Failed to use month card item, uin %u, item id %d\n", m_pRoleObj->GetUin(), iItemID);

			pstResp->set_iresult(T_ZONE_PARA_ERROR);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return -3;
		}

		//�۳�����
		iRet = CRepThingsUtility::AddItemNum(*m_pRoleObj, iItemID, -iItemNum, ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			LOGERROR("Failed to delete fish item, ret %d, uin %u, item id:num %d:%d\n", iRet, m_pRoleObj->GetUin(), iItemID, iItemNum);

			pstResp->set_iresult(iRet);
			CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);
			return iRet;
		}

		//�����¿�ʱ��
		CRechargeUtility::AddMonthEndTime(*m_pRoleObj, pstItemConfig->aiParam[0]);
	}
	break;

	default:
		break;
	}

	pstResp->set_iresult(T_SERVER_SUCCESS);
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, m_pRoleObj);

	return T_SERVER_SUCCESS;
}

//��ҵ���ʹ�÷���
int CRepThingsHandler::OnResponseUseRepItem()
{
	//��ȡ������Ϣ
	const World_OnlineExchange_Response& stExchangeResp = m_pRequestMsg->stbody().m_stworld_onlineexchange_response();
	const OnlineExchange& stInfo = stExchangeResp.stinfo();

	//��ȡ���
	CGameRoleObj* pstRoleObj = CUnitUtility::GetRoleByUin(stExchangeResp.uin());
	if (!pstRoleObj)
	{
		if (stExchangeResp.iresult())
		{
			//����ʧ�ܣ���ӡ������־
			LOGERROR("Failed to return back used item, uin %u, item id:num %d:%d\n", stExchangeResp.uin(), stInfo.iitemid(), stInfo.iitemnum());
		}

		//�����ͷ���
		return -1;
	}
	
	if (stExchangeResp.iresult())
	{
		//�������ʧ�ܷ�������
		int iRet = CRepThingsUtility::AddItemNum(*pstRoleObj, stInfo.iitemid(), stInfo.iitemnum(), ITEM_CHANNEL_USEITEM);
		if (iRet)
		{
			//����ʧ�ܣ���ӡ������־
			LOGERROR("Failed to return back used item, uin %u, item id:num %d:%d\n", pstRoleObj->GetUin(), stInfo.iitemid(), stInfo.iitemnum());
		}
	}
	else if (stInfo.itype() == ONLINE_EXCHANGE_BILL)
	{
		//��ȡ��������
		int iNewNum = pstRoleObj->GetRepThingsManager().GetRepItemNum(stInfo.iitemid());

		//���Ѷһ��ɹ��� ��ӡ��Ӫ��־
		CZoneOssLog::TraceExchangeBill(pstRoleObj->GetUin(), pstRoleObj->GetChannel(), pstRoleObj->GetNickName(), stInfo.iitemid(),
			stInfo.iitemnum(), stInfo.iexchangenum(), stInfo.strphonenum().c_str(), iNewNum-stInfo.iitemnum(), iNewNum);
	}

	//������Ϣ
	CZoneMsgHelper::GenerateMsgHead(stMsg, MSGID_ZONE_REPOPERA_RESPONSE);
	Zone_RepOpera_Response* pstResp = stMsg.mutable_stbody()->mutable_m_stzone_repopera_response();
	pstResp->set_etype(REQ_OPERA_TYPE_USE);
	pstResp->set_iitemid(stExchangeResp.stinfo().iitemid());
	pstResp->set_iitemnum(stExchangeResp.stinfo().iitemnum());
	pstResp->set_iresult(stExchangeResp.iresult());

	//���͸����
	CZoneMsgHelper::SendZoneMsgToRole(stMsg, pstRoleObj);

	return T_SERVER_SUCCESS;
}
